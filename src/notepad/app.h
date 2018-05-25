#ifndef NOTEPAD_NOTEPAD_APP_H_
#define NOTEPAD_NOTEPAD_APP_H_

#include "wx/app.h"
#include "wx/event.h"
#include "wx/frame.h"
#include "wx/string.h"

#include "notepad/text_panel.h"

namespace csi_training {

//----------------------------------------------------------------------
// class definitions
//----------------------------------------------------------------------

class NotepadApp : public wxApp {
 public:
  NotepadApp();
  virtual ~NotepadApp();

  virtual bool OnInit();
};

DECLARE_APP(NotepadApp)

enum {
  ID_Quit = wxID_EXIT,
  ID_About = wxID_ABOUT,
  ID_Text = 100,
  ID_New = 101,
  ID_Open = 102,
  ID_Save = 103,
  ID_Save_As = 104,
  ID_Undo = 105,
  ID_Redo = 106
};

class MyFrame : public wxFrame {
  DECLARE_EVENT_TABLE()

 public:
  explicit MyFrame(const wxString& title);

  void OnSize(wxSizeEvent& evt);  // NOLINT
  void OnQuit(wxCommandEvent&event);  // NOLINT
  void OnAbout(wxCommandEvent&event);  // NOLINT
  void OnNewFile(wxCommandEvent&event);  // NOLINT
  void OnOpenFile(wxCommandEvent&event);  // NOLINT
  void OnSaveFile(wxCommandEvent&event);  // NOLINT
  void OnSaveAsFile(wxCommandEvent&event);  // NOLINT

  void OnUndo(wxCommandEvent&event);  // NOLINT
  void OnRedo(wxCommandEvent&event);  // NOLINT
  void OnEditMenuUpdate(wxUpdateUIEvent& evt);  // NOLINT

  bool CanUndo();
  bool CanRedo();

 private:
  TextPanel* text_ctrl_;
  wxString file_path_;
};

}  // namespace csi_training
#endif  // NOTEPAD_NOTEPAD_APP_H_
