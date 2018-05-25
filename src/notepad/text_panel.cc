#include "notepad/text_panel.h"

#include "wx/caret.h"
#include "wx/dc.h"
#include "wx/dcbuffer.h"
#include "wx/log.h"
#include "wx/sizer.h"

#include "notepad/defs.h"
#include "notepad/text_buffer.h"

namespace csi_training {
/////////////////////////////////////////

BEGIN_EVENT_TABLE(TextPanel, wxScrolledWindow)
EVT_PAINT(TextPanel::OnPaint)
EVT_SIZE(TextPanel::OnSize)
EVT_MOUSE_EVENTS(TextPanel::OnMouseEvents)
EVT_SET_FOCUS(TextPanel::OnSetFocus)
EVT_KILL_FOCUS(TextPanel::OnKillFocus)
EVT_MOUSE_CAPTURE_LOST(TextPanel::OnMouseCaptureLost)
EVT_KEY_DOWN(TextPanel::OnKeyDown)
EVT_CHAR(TextPanel::OnChar)
EVT_SCROLLWIN(TextPanel::OnScroll)
END_EVENT_TABLE();

TextPanel::TextPanel(TextBuffer* buffer,
                     wxWindow *parent,
                     wxWindowID winid,
                     const wxPoint& pos,
                     const wxSize& size,
                     long style,  // NOLINT
                     const wxString& name)
    : wxScrolledWindow(parent, winid, pos, size, style, name)
    , text_buffer_(buffer)
    , line_height_(0)
    , line_padding_(0) {
  Init();
}

TextPanel::~TextPanel() {
}

void TextPanel::Init() {
  SetBackgroundStyle(wxBG_STYLE_PAINT);
  SetFont(wxFont(wxString("Consolas")));

  int cw = 0;
  int ch = 0;
  wxMemoryDC dc;
  dc.SetFont(GetFont());
  dc.GetTextExtent(wxString("a"), &cw, &ch, 0, nullptr);

  char_size_.Set(cw, ch);

  wxCaret* caret = new wxCaret(this, 1, ch);
  SetCaret(caret);
  caret->Show();

  line_padding_ = 1;

  line_height_ = line_padding_ + char_size_.y + line_padding_;

  FitInside();
  SetScrollRate(char_size_.x, line_height_);
  DoShowScrollbar();
}

void TextPanel::Clear() {
  assert(text_buffer_ != nullptr);
  text_buffer_->DoClear();

  click_position_.x = 0;
  click_position_.y = 0;
  UpdateCaretPosition();
  Refresh();
}

void TextPanel::LoadFile(const wxString& file) {
  assert(text_buffer_ != nullptr);
  text_buffer_->DoLoadFile(file);
  UpdateVirtualSize();
  Refresh();
}

void TextPanel::SaveFile(const wxString& file) {
  assert(text_buffer_ != nullptr);
  text_buffer_->DoSaveFile(file);
  Refresh();
}

void TextPanel::OnPaint(wxPaintEvent& evt) {
  wxLogDebug("TextPanel::OnPaint");

  wxAutoBufferedPaintDC dc(this);
  dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW)));
  dc.Clear();

  dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
  dc.SetFont(GetFont());

  PrepareDC(dc);

  wxRect rect = GetClientRect();
  int x = rect.x;
  int y = rect.y;

  for (int i = 0; i < text_buffer_->GetRowCount(); ++i) {
    dc.DrawText(
      text_buffer_->GetRowString(i),
      x,
      y);
    y += line_height_;
  }
}

void TextPanel::OnSize(wxSizeEvent& evt) {
  wxLogDebug("TextPanel::OnSize");
  UpdateVirtualSize();
  UpdateCaretPosition();
  evt.Skip();
}

void TextPanel::OnScroll(wxScrollWinEvent& event) {
  event.Skip();
}

void TextPanel::OnMouseEvents(wxMouseEvent& evt) {
  wxEventType evt_type = evt.GetEventType();

  if (evt_type == wxEVT_LEFT_DOWN) {
    DoMouseLeftDown(evt);
  } else if (evt_type == wxEVT_LEFT_UP) {
    DoMouseLeftUp(evt);
  } else if (evt_type == wxEVT_MOTION) {
    DoMouseMotion(evt);
  } else if (evt_type == wxEVT_RIGHT_DOWN) {
    DoMouseRightDown(evt);
  } else if (evt_type == wxEVT_RIGHT_UP) {
    DoMouseRightUp(evt);
  } else if (evt_type == wxEVT_MIDDLE_DOWN) {
    DoMouseMiddleDown(evt);
  } else if (evt_type == wxEVT_MIDDLE_UP) {
    DoMouseMiddleUp(evt);
  } else if (evt_type == wxEVT_LEFT_DCLICK) {
    DoMouseLeftDClick(evt);
  }
  Refresh();
  evt.Skip();
}

void TextPanel::OnKeyDown(wxKeyEvent& evt) {
  if (!DoKeyDown(evt)) {
    evt.Skip();
  }
  wxLogDebug("TextPanel::OnKeyDown");
}

void TextPanel::OnChar(wxKeyEvent& evt) {
  wxLogDebug("TextPanel::OnKeyDown");

  if (evt.AltDown() || evt.CmdDown()) {
    evt.Skip();
    return;
  }

  wxChar c = evt.GetUnicodeKey();
  if (c >= 0x20) {  // "< 0x20" is CONTROL
    InsertChar(c);
    UpdateVirtualSize();
    Refresh();
  }
}

// About the return value:
// - true: the event won't be skipped
// - false: the event will be skipped
// If the key down event is not skipped, no char event.
bool TextPanel::DoKeyDown(wxKeyEvent& evt) {
  wxLogDebug("TextPanel::DoKeyDown");

  int code = evt.GetKeyCode();
  int modifiers = evt.GetModifiers();

  if (code == WXK_NONE
      || code == WXK_CONTROL
      || code == WXK_SHIFT
      || code == WXK_ALT
      || code == WXK_ESCAPE) {
    return false;
  }

  if (modifiers == 0) {
    // Standard ASCII characters || ASCII extended characters.
    if ((code >= 33 && code <= 126) || (code >= 128 && code <= 255)) {
      return false;
    }
  } else if (modifiers == wxMOD_SHIFT && code < 127) {
    if (wxString(kNonShiftCharArray).find(static_cast<char>(code))
        != std::string::npos) {
      // ~!@#$%^&*()_+<>?:"{}|
      return false;
    }
  }

  return HandleSpecialKeyDown(evt);
}

bool TextPanel::HandleSpecialKeyDown(wxKeyEvent& evt) {
  int code = evt.GetKeyCode();
  int modifiers = evt.GetModifiers();

  if (code == WXK_TAB && modifiers == 0) {
    // Input a space instead of tab.
    InsertChar(kSpaceChar);
    return true;
  }

  if (code == WXK_RETURN && modifiers == 0) {
    wxChar c = evt.GetUnicodeKey();
    InsertChar(c);
    return true;
  }

  if (code == WXK_BACK && modifiers == 0) {
    DeleteChar(kForward);
    return true;
  }

  if (code == WXK_DELETE && modifiers == 0) {
    DeleteChar(kBackward);
    return true;
  }

  if (code == WXK_UP && modifiers == 0) {
    click_position_.y -= 1;
    UpdateCaretPoint(click_position_);
    return true;
  }

  if (code == WXK_DOWN && modifiers == 0) {
    click_position_.y += 1;
    UpdateCaretPoint(click_position_);
      return true;
  }

  if (code == WXK_LEFT && modifiers == 0) {
    click_position_.x -= 1;
    UpdateCaretPoint(click_position_);
    return true;
  }

  if (code == WXK_RIGHT && modifiers == 0) {
    click_position_.x += 1;
    UpdateCaretPoint(click_position_);
    return true;
  }

  return false;
}

void TextPanel::DoMouseLeftDown(wxMouseEvent& evt) {
  wxLogDebug("TextPanel::DoMouseLeftDown");

  SetFocus();
  if (!HasCapture()) {
    CaptureMouse();
  }

  click_position_ = CalcCaretPoint(evt.GetPosition());

  HandleLeftDownNoAccel();
}

void TextPanel::DoMouseLeftUp(wxMouseEvent& evt) {
  wxLogDebug("TextPanel::DoMouseLeftUp");

  if (HasCapture()) {
    ReleaseMouse();
  }
}

void TextPanel::DoMouseMotion(wxMouseEvent& evt) {
  wxLogDebug("TextPanel::DoMouseMotion");

  if (!HasCapture()) {
    return;
  }
  if (!evt.LeftIsDown()) {
    return;
  }
}

void TextPanel::DoMouseRightDown(wxMouseEvent& evt) {
  wxLogDebug("TextPanel::DoMouseRightDown");
}

void TextPanel::DoMouseRightUp(wxMouseEvent& evt) {
  wxLogDebug("TextPanel::DoMouseRightUp");
}

void TextPanel::DoMouseMiddleDown(wxMouseEvent& evt) {
  wxLogDebug("TextPanel::DoMouseMiddleDown");
}

void TextPanel::DoMouseMiddleUp(wxMouseEvent& evt) {
  wxLogDebug("TextPanel::DoMouseMiddleUp");
}

void TextPanel::DoMouseLeftDClick(wxMouseEvent& evt) {
  wxLogDebug("TextPanel::DoMouseLeftDClick");
}

void TextPanel::OnSetFocus(wxFocusEvent& evt) {
  wxLogDebug("TextPanel::OnSetFocus");
  evt.Skip();
}

void TextPanel::OnKillFocus(wxFocusEvent& evt) {
  wxLogDebug("TextPanel::OnKillFocus");
  evt.Skip();
}

void TextPanel::OnMouseCaptureLost(wxMouseCaptureLostEvent& evt) {
  wxLogDebug("TextPanel::OnMouseCaptureLost");
}

void TextPanel::HandleLeftDownNoAccel() {
  UpdateCaretPoint(click_position_);
}

void TextPanel::UpdateVirtualSize() {
  int vw = text_buffer_->GetMaxRowCharCount()*char_size_.x + char_size_.x;
  int vh = text_buffer_->GetRowCount()*line_height_ + line_height_;
  SetVirtualSize(vw, vh);
  AdjustScrollbars();
}

void TextPanel::DoShowScrollbar() {
  ShowScrollbars(wxSHOW_SB_ALWAYS, wxSHOW_SB_ALWAYS);
}

void TextPanel::UpdateCaretPosition() {
  int x_off = 0;
  int y = click_position_.y;

  int caret_x = GetLineWidth(text_buffer_->GetRowDisplayString(y),
                             x_off,
                             click_position_.x);
  int caret_y = y * line_height_;

  wxPoint p;
  CalcScrolledPosition(caret_x, caret_y, &p.x, &p.y);

  GetCaret()->Move(p);
  Refresh();
}

void TextPanel::UpdateCaretPoint(const wxPoint& point) {
  wxPoint p(point);

  if (p.y >= text_buffer_->GetRowCount())
    p.y = text_buffer_->GetRowCount() - 1;

  if (p.y < 0)
    p.y = 0;

  // Adjust the new caret point.
  int line_length = text_buffer_->GetRowDisplayCharCount(p.y);
  if (p.x > line_length) {
    p.x = line_length;
  }

  if (p.x < 0)
    p.x = 0;

  click_position_ = p;

  UpdateCaretPosition();
}

wxPoint TextPanel::CalcCaretPoint(const wxPoint& pos) {
  const wxRect client_rect = GetClientRect();
  wxPoint adjusted_pos = pos;
  if (adjusted_pos.y > client_rect.GetBottom()) {
    adjusted_pos.y = client_rect.GetBottom();
  } else if (adjusted_pos.y < client_rect.GetTop()) {
    adjusted_pos.y = client_rect.GetTop();
  }

  // Calculate unscrolled position.
  wxPoint unscrolled_pos = CalcUnscrolledPosition(adjusted_pos);

  // Calculate caret point from text buffer.
  wxPoint caret_point = unscrolled_pos;
  caret_point.y = unscrolled_pos.y / line_height_;

  if (caret_point.y > text_buffer_->GetRowCount() - 1) {
    caret_point.y = text_buffer_->GetRowCount() - 1;
  }

  if (caret_point.y < 0) {
    caret_point.y = 0;
  }

  caret_point.x = GetCharIndex(caret_point.y, unscrolled_pos.x);

  return caret_point;
}

int TextPanel::GetCharIndex(int ln, int client_x) const {
  return IndexChar(text_buffer_->GetRowDisplayString(ln), 0, client_x);
}

int TextPanel::IndexChar(const wxString& line,
                         int base,
                         int client_x) const {
  if (line.IsEmpty())
    return 0;

  if (base >= static_cast<int>(line.Length()))
      return 0;

  return IndexCharRecursively(line, base, 0, line.Length() - base, client_x);
}

int TextPanel::IndexCharRecursively(const wxString& line,
                                    int base,
                                    int begin,
                                    int end,
                                    int client_x) const {
  if (begin >= end) {
    return begin;
  }

  int m = begin + (end - begin) / 2;
  int width = GetLineWidth(line, 0, m, base);

  if (abs(width - client_x) < (char_size_.x / 2)) {
    return m;
  } else if (client_x > width) {
    return IndexCharRecursively(line, base, m + 1, end, client_x);
  } else {  // client_x < width
    return IndexCharRecursively(line, base, begin, m, client_x);
  }
}

int TextPanel::GetLineWidth(const wxString& line,
                            int off,
                            int len,
                            int base) const {
  return GetWidth(line.t_str(), base + off, len);
}

int TextPanel::GetWidth(const std::wstring& text, int off, int len) const {
  int x = 0;
  wxMemoryDC dc;
  dc.SetFont(GetFont());
  dc.GetTextExtent(wxString(text.c_str() + off, len), &x, nullptr, 0, nullptr);
  return x;
}

void TextPanel::InsertChar(wxChar c) {
  wxPoint point = click_position_;
  InsertChar(point, c, kForward);
}

void TextPanel::InsertChar(const wxPoint& point, wxChar c, ActionDir dir) {
  click_position_ = text_buffer_->InsertChar(point, c, dir);
  UpdateCaretPoint(click_position_);
}

void TextPanel::DeleteChar(ActionDir dir) {
  click_position_ = text_buffer_->DeleteChar(click_position_, dir);
  UpdateCaretPoint(click_position_);
}

void TextPanel::DoUndo() {
  text_buffer_->DoUndo();
  UpdateCaretPoint(text_buffer_->new_caret_position());
}

void TextPanel::DoRedo() {
  text_buffer_->DoRedo();
  UpdateCaretPoint(text_buffer_->new_caret_position());
}

bool TextPanel::CanUndo() const {
  return text_buffer_->CanUndo();
}

bool TextPanel::CanRedo() const {
  return text_buffer_->CanRedo();
}

}  // namespace csi_training
