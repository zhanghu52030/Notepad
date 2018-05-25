#ifndef NOTEPAD_NOTEPAD_TEXT_BUFFER_H_
#define NOTEPAD_NOTEPAD_TEXT_BUFFER_H_

#include <vector>
#include <list>

#include "wx/gdicmn.h"
#include "wx/txtstrm.h"

#include "notepad/defs.h"
#include "notepad/text_action.h"

namespace csi_training {

class TextAction;
class DeleteAction;
class InsertAction;

class TextBuffer{
 public:
  TextBuffer();
  virtual ~TextBuffer();

  void DoClear();
  void DoLoadFile(const wxString& file_path);
  void DoSaveFile(const wxString& file_path);

  int GetRowCount() const;

  // gets the char count at the longest row
  int GetMaxRowCharCount() const;

  wxString GetRowString(size_t row) const;
  // gets the string without "\r"
  wxString GetRowDisplayString(size_t row) const;

  int GetRowCharCount(int row) const;
  // gets the char count without "\r"
  int GetRowDisplayCharCount(int row) const;

  // if is_undo_redo = false, we should add this action to undo action list.
  wxPoint InsertChar(const wxPoint& point,
                     wxChar c,
                     ActionDir dir,
                     bool is_undo_redo = false);

  // handle insert regular char.
  // for example:abc...xyz, 123...789
  wxPoint InsertRegularChar(const wxPoint& point,
                            wxChar c,
                            ActionDir dir,
                            bool is_undo = false);

  // handle insert "enter"
  wxPoint InsertEnter(const wxPoint& point,
                     wxChar c,
                     ActionDir dir,
                     bool is_undo_redo = false);

  wxPoint DeleteChar(const wxPoint& point,
                     ActionDir dir,
                     bool is_undo_redo = false);

  void ClearBuffer();

  // if is_insert_or_delete = true, means this is insert action,
  // otherwise it is delete action.
  void AddUndoAction(const wxPoint& action_point,
                     const wxPoint& after_action_point,
                     wxChar c,
                     ActionDir dir,
                     bool is_insert_or_delete);

  void DoUndo();
  void DoRedo();
  bool CanUndo() const;
  bool CanRedo() const;
  void ClearActionList(std::list<TextAction*>* action_list);

  wxPoint new_caret_position() const { return new_caret_position_; }

 protected:
  wxChar DeleteCharInRow(int row, int pos, int len);

 private:
  wxPoint new_caret_position_;
  std::vector<wxString> text_content_;
  std::list<TextAction*> undo_actions_;
  std::list<TextAction*> redo_actions_;
};

}  // namespace csi_training

#endif  // NOTEPAD_NOTEPAD_TEXT_BUFFER_H_
