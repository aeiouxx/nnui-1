#include "application.h"
namespace astar {
Application::Application() : window_(sf::VideoMode(800, 600), "A*") {}
void Application::Run() {
  while (window_.isOpen()) {
    sf::Event event;
    while (window_.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        HandleClose();
      }
    }
  }
}
void Application::HandleClose() { window_.close(); }
}  // namespace astar
