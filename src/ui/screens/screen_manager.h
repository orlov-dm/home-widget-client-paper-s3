#pragma once

#include <map>
#include <memory>

#include "const.h"
#include "../views/view.h"
#include "../views/status_bar.h"

constexpr int32_t STATUS_BAR_HEIGHT = 40;
class ScreenManager
{
public:
    static ScreenManager &getInstance()
    {
        static ScreenManager instance;
        return instance;
    }

    void init()
    {
        this->statusBar = std::make_unique<StatusBar>(Position{0, 0}, Size{M5.Display.width(), STATUS_BAR_HEIGHT});
    }

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
        return M5.Display.height() - STATUS_BAR_HEIGHT;
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
};