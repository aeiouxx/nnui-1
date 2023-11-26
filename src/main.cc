#include "application.h"
#include "common/logger.h"

int main() {
  LOG_INFO("Starting application.");
  astar::Application app;
  app.Run();
  return 0;
}
