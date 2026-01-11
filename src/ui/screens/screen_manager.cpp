#include "screen_manager.h"

void ScreenManager::renderCurrentScreen()
{
    this->statusBar->render();
    if (this->currentScreen)
    {
        this->currentScreen->render();
    }
}

void ScreenManager::showScreen(ScreenID screenId)
{
    this->statusBar->render(true);
    auto it = this->screens.find(screenId);
    if (it != this->screens.end())
    {
        this->currentScreen = it->second.get();
        this->currentScreen->setPosition({0, STATUS_BAR_HEIGHT});
        this->currentScreen->setSize({M5.Display.width(), this->getAvailableHeight()});
        this->currentScreen->render(true);
    }
}

void ScreenManager::addScreen(std::unique_ptr<View> screen, ScreenID screenId)
{
    this->screens[screenId] = std::move(screen);
}

void ScreenManager::setStatus(const String &status)
{
    if (!this->statusBar)
        return;
    this->statusBar->setValue(status);
}
