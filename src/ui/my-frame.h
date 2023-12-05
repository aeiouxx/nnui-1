#ifndef ASTAR_UI_MY_FRAME_H
#define ASTAR_UI_MY_FRAME_H
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/wx.h>

#include "maze-panel.h"
namespace astar::ui {
class MyFrame : public wxFrame {
 private:
  static const wxSize kResolutions[];
  static const wxColour kBackgroundColour;

 public:
  MyFrame();
  ~MyFrame();

 protected:
  void OnExit(wxCommandEvent& event);
  void OnOpenFile(wxCommandEvent& event);
  void OnResize(wxSizeEvent& event);
  void ProcessBitmap(const wxString& path);

  void PreparePanel();
  void PrepareMenu();
  void BindEvents();
  void PrepareForScreen();

 private:
  MazeCanvas* panel_;
#ifdef ASTAR_DEBUG
  wxLog* logger_;
#endif
};
}  // namespace astar::ui
#endif
