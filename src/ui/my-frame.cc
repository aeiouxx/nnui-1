#include "my-frame.h"

#include <wx/filedlg.h>
#include <wx/progdlg.h>
namespace astar::ui {
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Hello World") {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(wxID_OPEN, "&Open...\tCtrl-O", "Open a bitmap file");
  menuFile->Append(wxID_EXIT);
  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  SetMenuBar(menuBar);

  bitmap_panel_ = new BitmapPanel(this);
  SetSizeHints(800, 600);

  CreateStatusBar();
  SetStatusText("Welcome to wxWidgets!");

  Bind(wxEVT_MENU, &MyFrame::OnOpenFile, this, wxID_OPEN);
  Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnExit(wxCommandEvent& event) { Close(true); }
void MyFrame::OnOpenFile(wxCommandEvent& event) {
  wxFileDialog openFileDialog(this, _("Open Bitmap file"), "", "",
                              "Bitmap files (*.bmp)|*.bmp",
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL) return;

  // proceed loading the file chosen by the user;
  // this can be done with e.g. wxWidgets input streams:
  wxString path = openFileDialog.GetPath();
  LoadBitmap(path);
}
void MyFrame::LoadBitmap(const wxString& path) {
  wxProgressDialog progressDialog(
      "Loading Bitmap", "Please wait while the bitmap is being loaded...",
      100,  // Maximum value
      this, wxPD_CAN_ABORT | wxPD_APP_MODAL | wxPD_ELAPSED_TIME);
  for (int i = 0; i <= 100; i += 10) {
    if (!progressDialog.Update(i)) {
      break;
    }
  }
  DisplayBitmap(path);
}
void MyFrame::DisplayBitmap(const wxString& path) {
  wxBitmap bitmap(path, wxBITMAP_TYPE_BMP);
  bitmap.LoadFile(path, wxBITMAP_TYPE_BMP);
  bitmap_panel_->SetBitmap(bitmap);
  Refresh();
}
}  // namespace astar::ui
