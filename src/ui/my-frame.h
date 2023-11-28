#ifndef ASTAR_UI_MY_FRAME_H
#define ASTAR_UI_MY_FRAME_H
#include <wx/event.h>
#include <wx/frame.h>
#include <wx/wx.h>

#include "bitmap-panel.h"
namespace astar::ui {
enum {
  kIdHello = 1
};
class MyFrame : public wxFrame {
 public:
  MyFrame();

 private:
  void OnExit(wxCommandEvent& event);
  void OnOpenFile(wxCommandEvent& event);
  void LoadBitmap(const wxString& path);
  void DisplayBitmap(const wxString& path);

 private:
  BitmapPanel* bitmap_panel_;
};
}  // namespace astar::ui
#endif
