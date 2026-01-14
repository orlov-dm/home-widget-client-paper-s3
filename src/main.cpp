#include "app/application.h"

void setup()
{
  Application::getInstance().setup();
}

void loop()
{
  Application::getInstance().loop();
}