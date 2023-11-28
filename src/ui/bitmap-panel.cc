#include "bitmap-panel.h"

#include "wx/dcclient.h"

namespace astar::ui {
BitmapPanel::BitmapPanel(wxWindow* parent) : wxPanel(parent) {
  Bind(wxEVT_PAINT, &BitmapPanel::OnPaint, this);
}
void BitmapPanel::OnPaint(wxPaintEvent& event) {
  if (!bitmap_.IsOk()) {
    return;
  }
  wxPaintDC dc(this);
  dc.DrawBitmap(bitmap_, 0, 0, false);
}
void BitmapPanel::SetBitmap(const wxBitmap& bitmap) {
  bitmap_ = bitmap;
  Refresh();
}
// clang-format off
wxBEGIN_EVENT_TABLE(BitmapPanel, wxPanel) 
  EVT_PAINT(BitmapPanel::OnPaint)
wxEND_EVENT_TABLE()
// clang-format on
}  // namespace astar::ui
