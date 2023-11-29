#ifndef ASTAR_UI_MY_FRAME_H
#define ASTAR_UI_MY_FRAME_H
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/wx.h>

#include "maze-panel.h"
namespace astar::ui {
class MyFrame : public wxFrame {
 public:
  MyFrame();

 protected:
  void OnExit(wxCommandEvent& event);
  void OnOpenFile(wxCommandEvent& event);
  void OnResize(wxSizeEvent& event);
  void ProcessBitmap(const wxString& path);

  void PreparePanel();
  void PrepareMenu();
  void BindEvents();

 private:
  MazePanel* panel_;
};
}  // namespace astar::ui
#endif
