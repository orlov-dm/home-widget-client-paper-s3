#pragma once

#include <map>
#include <memory>

#include "const.h"
#include "../views/view.h"
#include "../views/status_bar.h"
#include "../views/nav_bar.h"

constexpr int32_t STATUS_BAR_HEIGHT = 40;
constexpr int32_t NAV_BAR_HEIGHT = 60;
class ScreenManager
{
public:
    static ScreenManager &getInstance()
    {
        static ScreenManager instance;
        return instance;
    }

    void init();

    void renderCurrentScreen();
    void showScreen(ScreenID screenId);
    void refreshScreen();
    void addScreen(std::unique_ptr<View> screen, ScreenID screenId);

    void setStatus(const String &status);
    const View *getCurrentScreen() const
    {
        return this->currentScreen;
    }

protected:
    const int32_t getAvailableHeight() const
    {
        return M5.Display.height() - STATUS_BAR_HEIGHT - NAV_BAR_HEIGHT;
    }

private:
    ScreenManager()
    {
    }

    ScreenManager(const ScreenManager &) = delete;
    ScreenManager &operator=(const ScreenManager &) = delete;

    std::map<ScreenID, std::unique_ptr<View>> screens;
    View *currentScreen = nullptr;

    std::unique_ptr<StatusBar> statusBar = nullptr;
    std::unique_ptr<NavBar> navBar = nullptr;

    ScreenID currentScreenId = ScreenID::NONE;
};