#pragma once

#include <Arduino.h>

#include "view.h"

class NavBar : public View
{
public:
    NavBar(const String &id);
    virtual ~NavBar() {}
};