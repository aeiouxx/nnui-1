#include "my-frame.h"

#include <wx/filedlg.h>
#include <wx/progdlg.h>

#include "wx/log.h"
namespace astar::ui {
using astar::common::CellType;
using astar::common::Grid;

MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Astar") {
  PreparePanel();
  PrepareMenu();
  BindEvents();
}
void MyFrame::PreparePanel() {
  panel_ = new MazePanel(this);
  wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);
  sizer->Add(panel_, 1, wxEXPAND);
  SetSizer(sizer);
  Layout();
}
void MyFrame::PrepareMenu() {
  wxMenu* menuFile = new wxMenu;
  menuFile->Append(wxID_OPEN, "&Open...\tCtrl-O", "Open a bitmap file");
  menuFile->Append(wxID_EXIT);
  wxMenuBar* menuBar = new wxMenuBar;
  menuBar->Append(menuFile, "&File");
  SetMenuBar(menuBar);
}
void MyFrame::BindEvents() {
  Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
  Bind(wxEVT_MENU, &MyFrame::OnOpenFile, this, wxID_OPEN);
  Bind(wxEVT_SIZE, &MyFrame::OnResize, this);
}
void MyFrame::OnExit(wxCommandEvent& event) { Close(true); }
void MyFrame::OnOpenFile(wxCommandEvent& event) {
  wxFileDialog openFileDialog(this, _("Open Bitmap file"), "", "",
                              "Bitmap files (*.bmp)|*.bmp",
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL) return;
  wxString path = openFileDialog.GetPath();
  ProcessBitmap(path);
}
void MyFrame::OnResize(wxSizeEvent& event) {}
void MyFrame::ProcessBitmap(const wxString& path) {
  wxProgressDialog progressDialog(
      "Processing bitmap", "Please wait while the bitmap is being loaded...",
      100,  // Maximum value
      this, wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME);
  wxImage image(path);
  if (!image.IsOk()) {
    wxLogError("Cannot load file '%s'.", path);
    return;
  }
  progressDialog.Update(10, "Bitmap loaded, processing...");
  int width = image.GetWidth();
  int height = image.GetHeight();

  Grid grid(height, width);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      unsigned char red = image.GetRed(x, y);
      unsigned char green = image.GetGreen(x, y);
      unsigned char blue = image.GetBlue(x, y);
      if (red | green | blue) {
        grid.At(y, x) = CellType::kEmpty;
      } else {
        grid.At(y, x) = CellType::kWall;
      }
    }
    progressDialog.Update(10 + 90 * y / height);
  }
  panel_->SetGrid(std::move(grid));
}
}  // namespace astar::ui
