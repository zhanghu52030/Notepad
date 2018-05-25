#ifndef NOTEPAD_NOTEPAD_TEXT_PANEL_H_
#define NOTEPAD_NOTEPAD_TEXT_PANEL_H_

#include "wx/panel.h"
#include "wx/pen.h"
#include "wx/scrolwin.h"
#include "wx/timer.h"

#include "notepad/defs.h"
#include "notepad/text_buffer.h"

namespace csi_training {

class TextPanel : public wxScrolledWindow {
  DECLARE_EVENT_TABLE()

 public:
  TextPanel(TextBuffer* buffer,
            wxWindow* parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxScrolledWindowStyle,  // NOLINT
            const wxString& name = wxPanelNameStr);

  virtual ~TextPanel();

  void Init();
  void Clear();
  void LoadFile(const wxString& file);
  void SaveFile(const wxString& file);

  void DoUndo();
  void DoRedo();
  bool CanUndo() const;
  bool CanRedo() const;

  void UpdateVirtualSize();

 protected:
  void OnPaint(wxPaintEvent& evt);  // NOLINT
  void OnSize(wxSizeEvent& evt);  // NOLINT
  void OnMouseEvents(wxMouseEvent& evt);  // NOLINT
  void OnKeyDown(wxKeyEvent& evt);  // NOLINT
  void OnSetFocus(wxFocusEvent& evt);  // NOLINT
  void OnKillFocus(wxFocusEvent& evt);  // NOLINT
  void OnMouseCaptureLost(wxMouseCaptureLostEvent& evt);  // NOLINT
  void OnChar(wxKeyEvent& evt);  // NOLINT
  void OnScroll(wxScrollWinEvent& event);  // NOLINT

  void DoMouseLeftDown(wxMouseEvent& evt);  // NOLINT
  void DoMouseLeftUp(wxMouseEvent& evt);  // NOLINT
  void DoMouseMotion(wxMouseEvent& evt);  // NOLINT
  void DoMouseRightDown(wxMouseEvent& evt);  // NOLINT
  void DoMouseRightUp(wxMouseEvent& evt);  // NOLINT
  void DoMouseMiddleDown(wxMouseEvent& evt);  // NOLINT
  void DoMouseMiddleUp(wxMouseEvent& evt);  // NOLINT
  void DoMouseLeftDClick(wxMouseEvent& evt);  // NOLINT
  bool DoKeyDown(wxKeyEvent& evt);  // NOLINT

  // Shows/hides scrollbar according to the current option.
  void DoShowScrollbar();

  // Sets new caret point and update caret position.
  void UpdateCaretPoint(const wxPoint& point);

  // Sets caret position according to current caret point.
  void UpdateCaretPosition();

  void HandleLeftDownNoAccel();

  bool HandleSpecialKeyDown(wxKeyEvent& evt);  // NOLINT

  // Calculate the caret point according to the text area client position.
  wxPoint CalcCaretPoint(const wxPoint& pos);

  // Gets the index of the char at position client_x
  int GetCharIndex(int ln, int client_x) const;

  // Binary search to get the char index with the given client x coordinate.
  int IndexChar(const wxString& line,
                int base,
                int client_x) const;

  // Binary search to get the char index.
  // The range is STL-style: [begin, end).
  // Called by IndexChar().
  int IndexCharRecursively(const wxString& line,
                           int base,
                           int begin,
                           int end,
                           int client_x) const;

  // Get the sub line width.
  int GetLineWidth(const wxString& line,
                   int off,
                   int len,
                   int base = 0) const;

  int GetWidth(const std::wstring& text, int off, int len) const;

  // Insert a char at the caret point.
  void InsertChar(wxChar c);
  void InsertChar(const wxPoint& point, wxChar c, ActionDir dir);

  void DeleteChar(ActionDir dir);

 private:
  TextBuffer* text_buffer_;
  wxSize char_size_;
  wxPoint click_position_;
  int line_height_;
  int line_padding_;  // Spacing at the top and bottom of a line.
};

}  // namespace csi_training

#endif  // NOTEPAD_NOTEPAD_TEXT_PANEL_H_
