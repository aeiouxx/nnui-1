#ifndef ASTAR_APPLICATION_H_
#define ASTAR_APPLICATION_H_
#include <SFML/Graphics.hpp>

#include "common/definitions.h"
namespace astar {
class Application {
 public:
  Application();
  void Run();

 private:
  void Update();
  void Render();
  void HandleClose();

 private:
  sf::RenderWindow window_;
  sf::Clock clock_;
  float deltaTime_;

  ASTAR_NOCOPY_NOMOVE(Application)
};
}  // namespace astar
#endif
