#include "screen_manager.h"

#include "../../app/component_ids.h"

void ScreenManager::init()
{
    this->statusBar = std::make_unique<StatusBar>(Position{0, 0}, Size{M5.Display.width(), STATUS_BAR_HEIGHT});
    this->navBar = std::make_unique<NavBar>(ComponentID::NAV_BAR);
    this->navBar->setPosition({0, M5.Display.height() - NAV_BAR_HEIGHT});
    this->navBar->setSize({M5.Display.width(), NAV_BAR_HEIGHT});
}

void ScreenManager::renderCurrentScreen()
{
    this->statusBar->render();
    if (this->currentScreen)
    {
        this->currentScreen->render();
    }
    this->navBar->render();
}

void ScreenManager::showScreen(ScreenID screenId)
{
    if (screenId == this->currentScreenId)
    {
        this->refreshScreen();
        return; // Already showing this screen
    }
    if (this->currentScreen)
    {
        this->currentScreen->onExit();
        // Clear the old screen area before switching
        M5.Display.fillRect(0, STATUS_BAR_HEIGHT, M5.Display.width(), this->getAvailableHeight(), TFT_WHITE);
        this->currentScreen = nullptr;
    }
    this->currentScreenId = ScreenID::NONE;
    auto it = this->screens.find(screenId);
    if (it != this->screens.end())
    {
        this->currentScreenId = screenId;
        this->currentScreen = it->second.get();
        this->currentScreen->setPosition({0, STATUS_BAR_HEIGHT});
        this->currentScreen->setSize({M5.Display.width(), this->getAvailableHeight()});
        this->currentScreen->setNeedsRender(); // Force full render when switching screens
        this->currentScreen->onEnter();
    }
}

void ScreenManager::refreshScreen()
{
    if (this->currentScreen)
    {
        this->currentScreen->onRefresh();
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
