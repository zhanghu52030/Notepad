#include "notepad/text_buffer.h"

#include "wx/wfstream.h"

namespace csi_training {
/////////////////////////////////////////

TextBuffer::TextBuffer() {
}

TextBuffer::~TextBuffer() {
  ClearBuffer();
}

void TextBuffer::ClearBuffer() {
  text_content_.clear();

  ClearActionList(&undo_actions_);
  ClearActionList(&redo_actions_);
}
int TextBuffer::GetRowCount() const {
  return text_content_.size();
}


wxString TextBuffer::GetRowString(size_t row) const {
  if (row >= 0 && row < text_content_.size()) {
    return text_content_[row];
  } else {
    return wxString("");
  }
}

wxString TextBuffer::GetRowDisplayString(size_t row) const {
  if (row >= 0 && row < text_content_.size()) {
    return wxString(GetRowString(row), GetRowDisplayCharCount(row));
  } else {
    return wxString("");
  }
}

void TextBuffer::DoClear() {
  ClearBuffer();
}


void TextBuffer::DoLoadFile(const wxString& file_path) {
  ClearBuffer();

  wxFileInputStream file_input(file_path);
  wxTextInputStream text_input_stream(file_input);

  wxString line = text_input_stream.ReadLine();
  while (!line.IsEmpty()) {
    text_content_.push_back(line);
    line = text_input_stream.ReadLine();
  }
}

void TextBuffer::DoSaveFile(const wxString& file_path) {
  assert(!file_path.empty());

  FILE* file = wxFopen(file_path, wxT("wb"));
  if (file != nullptr) {
    for (auto str : text_content_) {
      size_t write_size = fwrite(str.c_str(), 1, str.size(), file);
    }
  }
  fclose(file);
}

wxPoint TextBuffer::InsertChar(const wxPoint& point,
                               wxChar c,
                               ActionDir dir,
                               bool is_undo_redo) {
  new_caret_position_ = point;
  if (c == CR) {
    new_caret_position_ = InsertEnter(point, c, dir, is_undo_redo);
  } else {
    new_caret_position_ = InsertRegularChar(point, c, dir, is_undo_redo);
  }
  return new_caret_position_;
}

wxPoint TextBuffer::InsertRegularChar(const wxPoint& point,
                                      wxChar c,
                                      ActionDir dir,
                                      bool is_undo_redo) {
  int new_x = point.x;
  int new_y = point.y;
  bool action_done = false;

  if (text_content_.empty()) {  // add new line
    if (point.x == 0 && point.y == 0) {
      text_content_.push_back(wxString(c));
      action_done = true;
    }
  } else {  // edit existed line
    if (point.y < static_cast<int>(text_content_.size())) {
      wxString* line = &(text_content_[point.y]);
      if (line != nullptr && point.x <= static_cast<int>(line->Length())) {
        line->insert(point.x, 1, c);
        action_done = true;
      }
    }
  }
  new_x++;

  // collect insert action to support undo/redo
  if (action_done) {
    bool is_insert = true;
    if (!is_undo_redo) {
      AddUndoAction(point, wxPoint(new_x, new_y), c, dir, is_insert);
    }
  }

  return wxPoint(new_x, new_y);
}
wxPoint TextBuffer::InsertEnter(const wxPoint& point,
                                wxChar c,
                                ActionDir dir,
                                bool is_undo_redo) {
  int new_x = point.x;
  int new_y = point.y;
  bool action_done = false;
  if (c == CR) {  // insert a new line and jump to the new line
    if (text_content_.empty()) {  // insert c at the first line
      if (point.x == 0 && point.y == 0) {
        text_content_.push_back(wxString(c));
        text_content_.push_back(wxString(L""));
        action_done = true;
      }
    } else {  // split the line to two
      if (point.y < static_cast<int>(text_content_.size())) {
        wxString* line = &(text_content_[point.y]);
        if (line != nullptr && point.x <= static_cast<int>(line->size())) {
          wxString new_line(*line, point.x, line->size() - point.x);
          line->erase(point.x);
          line->Append(&c, 1);
          text_content_.insert(text_content_.begin() + point.y + 1, new_line);
          action_done = true;
        }
      }
    }
    new_x = 0;
    new_y = point.y + 1;
  }

  // collect insert action to support undo/redo
  if (action_done) {
    bool is_insert = true;
    if (!is_undo_redo) {
      AddUndoAction(point, wxPoint(new_x, new_y), c, dir, is_insert);
    }
  }
  return wxPoint(new_x, new_y);
}

wxPoint TextBuffer::DeleteChar(const wxPoint& point,
                               ActionDir dir,
                               bool is_undo_redo) {
  new_caret_position_ = point;
  int delete_x = point.x;
  if (dir == kForward) {
    delete_x = point.x - 1;
  }

  int delete_y = point.y;
  if (delete_x < 0) {
    delete_y = delete_y - 1;
    delete_x = GetRowCharCount(delete_y) - 1;
  }

  wxChar deleted_char = DeleteCharInRow(delete_y, delete_x, 1);
  if (deleted_char != L'\0') {
    bool is_insert = false;
    if (!is_undo_redo) {
      AddUndoAction(point, wxPoint(delete_x, delete_y),
                    deleted_char, dir, is_insert);
    }

    new_caret_position_ = wxPoint(delete_x, delete_y);
  }

  return new_caret_position_;
}

wxChar TextBuffer::DeleteCharInRow(int row, int pos, int len) {
  wxChar deleted_char = L'\0';
  if (row >= 0 && row < GetRowCount()) {
    wxString* line = &text_content_[row];
    if (pos >= 0 && pos < GetRowCharCount(row)) {
      if (line->at(pos) == CR) {
        int next_row = row + 1;
        if (next_row < GetRowCount()) {
          wxString next_line = GetRowString(row + 1);
          line->append(next_line);
          text_content_.erase(text_content_.begin() + row + 1);
        }
      }
      deleted_char = line->at(pos);;
      line->erase(pos, 1);
    }
  }
  return deleted_char;
}

int TextBuffer::GetRowDisplayCharCount(int row) const {
  wxString line = GetRowString(row);
  int count = line.size();
  if (count > 0 && line.at(count - 1) == CR) {
    count--;
  }
  return count;
}

int TextBuffer::GetRowCharCount(int row) const {
  wxString line = GetRowString(row);
  return line.size();
}

int TextBuffer::GetMaxRowCharCount() const {
  int max_count = GetRowCharCount(0);
  for (int i = 1; i < GetRowCount(); i++) {
    if (max_count < GetRowCharCount(i)) {
      max_count = GetRowCharCount(i);
    }
  }
  return max_count;
}

void TextBuffer::AddUndoAction(const wxPoint& action_point,
                               const wxPoint& after_action_point,
                               wxChar c,
                               ActionDir dir,
                               bool is_insert_or_delete) {
  if (c != L'\0') {
    TextAction* text_action = nullptr;
    if (is_insert_or_delete) {
      text_action = new InsertAction(this, dir, c, action_point,
                                     after_action_point);
    } else {
      text_action = new DeleteAction(this, dir, c, action_point,
                                     after_action_point);
    }

    if (text_action != nullptr) {
      undo_actions_.push_back(text_action);
      ClearActionList(&redo_actions_);
    }
  }
}

void TextBuffer::DoUndo() {
  if (CanUndo()) {
    TextAction* undo_action = undo_actions_.back();
    undo_action->Undo();
    new_caret_position_ = undo_action->action_position();
    redo_actions_.push_back(undo_action);
    undo_actions_.pop_back();
  }
}

void TextBuffer::DoRedo() {
  if (CanRedo()) {
    TextAction* redo_action = redo_actions_.back();
    redo_action->Redo();
    new_caret_position_ = redo_action->after_action_position();
    undo_actions_.push_back(redo_action);
    redo_actions_.pop_back();
  }
}

bool TextBuffer::CanUndo() const {
  return !undo_actions_.empty();
}

bool TextBuffer::CanRedo() const {
  return !redo_actions_.empty();
}

void TextBuffer::ClearActionList(std::list<TextAction*>* action_list) {
  if (action_list != nullptr && !action_list->empty()) {
    for (auto it : *action_list) {
      delete it;
    }
    action_list->clear();
  }
}

}  // namespace csi_training
