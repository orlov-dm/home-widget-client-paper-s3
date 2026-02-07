#pragma once

#include <M5Unified.hpp>

#include "../types.h"

enum Alignment
{
    LEFT,
    CENTER,
    RIGHT
};

class Component
{
public:
    Component(const Position &position, const Size &size);
    Component(const Size &size, const String &id);
    Component(const String &id);
    Component(const Size &size);
    Component();

    virtual ~Component();

    void render(bool force = false);

    bool isTouched(int32_t x, int32_t y) const;

    const Position &getPosition() const { return position; }
    const Size &getSize() const { return size; }

    void setPosition(const Position &pos)
    {
        if (position.x == pos.x && position.y == pos.y)
            return;
        position = pos;
        setNeedsRender();
    }

    void setX(int32_t x)
    {
        if (position.x == x)
            return;
        position.x = x;
        setNeedsRender();
    }

    void setY(int32_t y)
    {
        if (position.y == y)
            return;
        position.y = y;
        setNeedsRender();
    }

    void setSize(const Size &s)
    {
        if (size.w == s.w && size.h == s.h)
            return;
        size = s;
        this->initAutoSizeFlags();
        setNeedsRender();
    }

    void setWidth(int32_t w)
    {
        if (size.w == w)
            return;
        size.w = w;
        this->initAutoSizeFlags();
        setNeedsRender();
    }

    void setHeight(int32_t h)
    {
        if (size.h == h)
            return;
        size.h = h;
        this->initAutoSizeFlags();
        setNeedsRender();
    }

    String getId() const { return id; }
    String getName() const { return name; }
    void setName(const String &n) { name = n; }

    int32_t getWidth() const { return size.w; }
    int32_t getHeight() const { return size.h; }

    void setNeedsRender();

    void setParent(Component *p);

    bool hasAutoWidth() const { return hasAutoWidthEnabled; }
    bool hasAutoHeight() const { return hasAutoHeightEnabled; }

    void setVisible(bool visible)
    {
        if (this->isVisibleValue == visible)
            return;
        this->isVisibleValue = visible;
        // Visibility changes require parent to fully re-render to clear old graphics
        if (this->parent)
        {
            this->parent->setNeedsRender();
        }
    }

    bool isVisible() const { return this->isVisibleValue; }

    bool needsRenderCheck() const { return !this->isRendered || this->hasChildThatNeedsRender; }
    bool getIsRendered() const { return this->isRendered; }

    virtual Size calculateMinimumSize() const { return this->size; }

    bool isDisabled() const { return this->isDisabledValue; }
    void setDisabled(bool disabled)
    {
        if (this->isDisabledValue == disabled)
            return;
        this->isDisabledValue = disabled;
        setNeedsRender();
    }

    Alignment getAlignment() const { return this->alignment; }
    void setAlignment(Alignment alignment)
    {
        if (this->alignment == alignment)
            return;
        this->alignment = alignment;
        setNeedsRender();
    }

    bool isCenterAligned() const { return this->isCenterAlignedValue; }
    void setCenterAlignment(bool centerAligned)
    {
        if (this->isCenterAlignedValue == centerAligned)
            return;
        this->isCenterAlignedValue = centerAligned;
        setNeedsRender();
    }

protected:
    virtual void doRender() = 0;
    void init();
    void initAutoSizeFlags()
    {
        this->hasAutoWidthEnabled = (size.w == 0);
        this->hasAutoHeightEnabled = (size.h == 0);
    }

    bool hasChildThatNeedsRender = false;

private:
    void markParentsChildNeedsRender();

    Position position = {0, 0};
    Size size = {0, 0};
    bool isRendered = false;
    String id = "";
    Component *parent = nullptr;
    bool hasAutoWidthEnabled = false;
    bool hasAutoHeightEnabled = false;
    bool isVisibleValue = true;
    String name = "";

    bool isDisabledValue = false;
    bool isCenterAlignedValue = false;

    Alignment alignment = Alignment::LEFT;
};