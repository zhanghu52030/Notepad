#include "notepad/app.h"

#include "wx/defs.h"
#include "wx/dir.h"
#include "wx/filedlg.h"
#include "wx/menu.h"
#include "wx/msgdlg.h"
#include "wx/panel.h"
#include "wx/sizer.h"
#include "wx/stdpaths.h"
#include "wx/stopwatch.h"
#include "wx/textctrl.h"

namespace csi_training {

IMPLEMENT_APP(NotepadApp)

NotepadApp::NotepadApp() {
}

NotepadApp::~NotepadApp() {
}

bool NotepadApp::OnInit() {
  if (!wxApp::OnInit()) {
    return false;
  }

  MyFrame *frame = new MyFrame(wxT("Notepad"));
  frame->Show(true);

  return true;
}

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_SIZE(MyFrame::OnSize)
EVT_MENU(ID_Quit, MyFrame::OnQuit)
EVT_MENU(ID_About, MyFrame::OnAbout)
EVT_MENU(ID_New, MyFrame::OnNewFile)
EVT_MENU(ID_Open, MyFrame::OnOpenFile)
EVT_MENU(ID_Save, MyFrame::OnSaveFile)
EVT_MENU(ID_Save_As, MyFrame::OnSaveAsFile)
EVT_MENU(ID_Undo, MyFrame::OnUndo)
EVT_MENU(ID_Redo, MyFrame::OnRedo)
EVT_UPDATE_UI_RANGE(ID_Undo, ID_Redo, MyFrame::OnEditMenuUpdate)
END_EVENT_TABLE();

MyFrame::MyFrame(const wxString& title)
    : wxFrame(NULL, wxID_ANY, title)
    , file_path_("") {
  // create a menu bar
  wxMenu *file_menu = new wxMenu;
  file_menu->Append(ID_New, "N&ew\tCtrl-N", "New File");
  file_menu->Append(ID_Open, "O&pen...\tCtrl-O", "Open File");
  file_menu->Append(ID_Save, "&Save\tCtrl-S", "Save File");
  file_menu->Append(ID_Save_As, "Sa&ve As...\t", "Save File As");
  file_menu->Append(ID_Quit, "E&xit\tAlt-X", "Quit Notepad");

  wxMenu *edit_menu = new wxMenu;
  edit_menu->Append(ID_Undo, "&Undo\tCtrl-Z", "Undo");
  edit_menu->Append(ID_Redo, "Red&o\tCtrl-Y", "Redo");

  // the "About" item should be in the help menu
  wxMenu *help_menu = new wxMenu;
  help_menu->Append(ID_About, "&About\tF1", "Show about notepad");

  // now append the freshly created menu to the menu bar...
  wxMenuBar *menu_bar = new wxMenuBar();
  menu_bar->Append(file_menu, "&File");
  menu_bar->Append(edit_menu, "Ed&it");
  menu_bar->Append(help_menu, "&Help");

  // ... and attach this menu bar to the frame
  SetMenuBar(menu_bar);

  TextBuffer *text_buffer = new TextBuffer();

  text_ctrl_ = new TextPanel(text_buffer,
                             this,
                             ID_Text,
                             wxDefaultPosition,
                             wxSize(100, 100),
                             wxScrolledWindowStyle,
                             wxPanelNameStr);
}

void MyFrame::OnSize(wxSizeEvent& evt) {
  wxLogDebug("MyFrame::OnSize");

  wxRect client_rect = GetClientRect();
  text_ctrl_->SetSize(client_rect.GetSize());
  text_ctrl_->UpdateVirtualSize();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event)) {
  // true is to force the frame to close
  Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event)) {
  wxMessageBox("Notepad", "About Notepad", wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnNewFile(wxCommandEvent& WXUNUSED(event)) {
  file_path_ = "";
  text_ctrl_->Clear();
  text_ctrl_->UpdateVirtualSize();
}

void MyFrame::OnOpenFile(wxCommandEvent& WXUNUSED(event)) {
  wxFileDialog dialog(this,
                      wxFileSelectorPromptStr,
                      wxEmptyString,
                      wxEmptyString,
                      wxFileSelectorDefaultWildcardStr,
                      wxFD_OPEN,
                      wxDefaultPosition,
                      wxDefaultSize,
                      wxFileDialogNameStr);

  if (dialog.ShowModal() == wxID_OK) {
    file_path_ = dialog.GetPath();

    if (!file_path_.empty()) {
      text_ctrl_->LoadFile(file_path_);
    }
  }
}

void MyFrame::OnSaveFile(wxCommandEvent& event) {
  if (file_path_.empty()) {
    OnSaveAsFile(event);
  } else {
    text_ctrl_->SaveFile(file_path_);
  }
}

void MyFrame::OnSaveAsFile(wxCommandEvent& WXUNUSED(event)) {
  wxFileDialog dialog(this,
                      wxFileSelectorPromptStr,
                      wxEmptyString,
                      wxEmptyString,
                      wxFileSelectorDefaultWildcardStr,
                      wxFD_SAVE,
                      wxDefaultPosition,
                      wxDefaultSize,
                      wxFileDialogNameStr);

  if (dialog.ShowModal() == wxID_OK) {
    file_path_ = dialog.GetPath();

    if (!file_path_.empty()) {
      text_ctrl_->SaveFile(file_path_);
    }
  }
}

void MyFrame::OnUndo(wxCommandEvent& WXUNUSED(event)) {
  if (CanUndo())
    text_ctrl_->DoUndo();
}

void MyFrame::OnRedo(wxCommandEvent& WXUNUSED(event)) {
  if (CanRedo())
    text_ctrl_->DoRedo();
}

void MyFrame::OnEditMenuUpdate(wxUpdateUIEvent& evt) {
  int menu_id = evt.GetId();
  bool state = false;
  if (menu_id == ID_Undo) {
    state = CanUndo();
  } else if (menu_id == ID_Redo) {
    state = CanRedo();
  }
  evt.Enable(state);
}

bool MyFrame::CanUndo() {
  return text_ctrl_->CanUndo();
}

bool MyFrame::CanRedo() {
  return text_ctrl_->CanRedo();
}

}  // namespace csi_training
