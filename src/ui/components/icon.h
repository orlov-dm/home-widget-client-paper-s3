#pragma once

#include <Arduino.h>

#include "component.h"
#include "../../icons/index.h"

class Icon : public Component
{
public:
    Icon(IconName iconName, IconSize iconSize) : Component(), iconName(iconName), iconSize(iconSize)
    {
        switch (iconSize)
        {
        case ICON_SIZE_256:
            this->setSize({256, 256});
            break;
        case ICON_SIZE_128:
            this->setSize({128, 128});
            break;
        case ICON_SIZE_64:
            this->setSize({64, 64});
            break;
        case ICON_SIZE_32:
        default:
            this->setSize({32, 32});
            break;
        }
    };

protected:
    void doRender() override;

private:
    IconName iconName;
    IconSize iconSize;
};