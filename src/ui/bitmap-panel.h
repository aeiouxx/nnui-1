#ifndef ASTAR_UI_BITMAP_PANEL_H
#define ASTAR_UI_BITMAP_PANEL_H
#include <wx/bitmap.h>
#include <wx/panel.h>
namespace astar::ui {
class BitmapPanel : public wxPanel {
 public:
  BitmapPanel(wxWindow* parent);
  void SetBitmap(const wxBitmap& bitmap);

 protected:
  void OnPaint(wxPaintEvent& event);

 private:
  wxBitmap bitmap_;
  wxDECLARE_EVENT_TABLE();
};
}  // namespace astar::ui
#endif
