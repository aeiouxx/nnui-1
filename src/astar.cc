#include "astar.h"

#include "ui/my-frame.h"
namespace astar {
bool Astar::OnInit() {
  ui::MyFrame *frame = new ui::MyFrame();
  frame->Show(true);
  return true;
}
}  // namespace astar
