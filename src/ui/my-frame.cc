#include "my-frame.h"

#include <wx/filedlg.h>
#include <wx/progdlg.h>

#include "wx/log.h"
namespace astar::ui {
using astar::common::CellType;
using astar::common::Grid;
const wxSize MyFrame::kResolutions[] = {
    wxSize(640, 480),  wxSize(800, 600),   wxSize(1024, 768),
    wxSize(1280, 720), wxSize(1280, 1024), wxSize(1366, 768),
    wxSize(1600, 900), wxSize(1920, 1080), wxSize(2560, 1440)};
const wxColour MyFrame::kBackgroundColour{wxColour(192, 192, 192, 255)};
MyFrame::MyFrame() : wxFrame(nullptr, wxID_ANY, "Astar", wxDefaultPosition) {
#ifdef ASTAR_DEBUG
  logger_ = new wxLogWindow(this, "Log", true, true);
  logger_->SetLogLevel(wxLOG_Debug);
  wxLog::SetActiveTarget(logger_);
#endif
  PreparePanel();
  PrepareMenu();
  BindEvents();
  PrepareForScreen();
}
MyFrame::~MyFrame() {
#ifdef ASTAR_DEBUG
  if (logger_ != nullptr) {
    wxLog::SetActiveTarget(nullptr);
    delete logger_;
    logger_ = nullptr;
  }
#endif
  Destroy();
}
void MyFrame::PreparePanel() {
  panel_ = new MazePanel(this, kBackgroundColour);
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
// literally the same as full screening on supported resolutions
// should calculate aspect ratio and pick a resolution that's smaller
void MyFrame::PrepareForScreen() {
  wxSize screenSize = wxGetDisplaySize();
  wxSize bestSize = kResolutions[0];
  for (auto& size : kResolutions) {
    if (screenSize.x >= size.x && screenSize.y >= size.y) {
      bestSize = size;
    }
  }
  SetSize(bestSize);
  Centre();
}
void MyFrame::OnExit(wxCommandEvent& event) {
  Close(true);
}
void MyFrame::OnOpenFile(wxCommandEvent& event) {
  wxFileDialog openFileDialog(this, _("Open Bitmap file"), "", "",
                              "Bitmap files (*.bmp)|*.bmp",
                              wxFD_OPEN | wxFD_FILE_MUST_EXIST);
  if (openFileDialog.ShowModal() == wxID_CANCEL)
    return;
  wxString path = openFileDialog.GetPath();
  ProcessBitmap(path);
}
void MyFrame::OnResize(wxSizeEvent& event) {
  event.Skip();
}
void MyFrame::ProcessBitmap(const wxString& path) {
  wxProgressDialog progressDialog(
      "Processing bitmap", "Please wait while the bitmap is being loaded...",
      100, this, wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_ELAPSED_TIME);
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
  panel_->Refresh();
}
}  // namespace astar::ui
