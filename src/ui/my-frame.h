#ifndef ASTAR_UI_MY_FRAME_H
#define ASTAR_UI_MY_FRAME_H
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/wx.h>

#include "maze-canvas.h"
namespace astar::ui {
class MyFrame : public wxFrame {
 private:
  static const wxSize kResolutions[];
  static const wxColour kBackgroundColour;
  static const int kIdOrientationStart = wxID_HIGHEST + 1;

 public:
  MyFrame();
  ~MyFrame();

 protected:
  void OnExit(wxCommandEvent &event);
  void OnOpenFile(wxCommandEvent &event);
  void OnResize(wxSizeEvent &event);
  void OnOrientationChange(wxCommandEvent &event);
  void ProcessBitmap(const wxString &path);

  void PreparePanel();
  void PrepareMenu();
  void BindEvents();
  void PrepareForScreen();

 private:
  MazeCanvas *panel_;
#ifdef ASTAR_LOGGER_ENABLE
  wxLog *logger_;
#endif
};
}  // namespace astar::ui
#endif
