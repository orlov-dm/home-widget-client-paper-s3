#pragma once

#include "../types.h"

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
        position = pos;
        setNeedsRender();
    }

    void setX(int32_t x)
    {
        position.x = x;
        setNeedsRender();
    }

    void setY(int32_t y)
    {
        position.y = y;
        setNeedsRender();
    }

    void setSize(const Size &s)
    {
        size = s;
        this->initAutoSizeFlags();
        setNeedsRender();
    }

    void setWidth(int32_t w)
    {
        size.w = w;
        this->initAutoSizeFlags();
        setNeedsRender();
    }

    void setHeight(int32_t h)
    {
        size.h = h;
        this->initAutoSizeFlags();
        setNeedsRender();
    }

    String getId() const { return id; }

    int32_t getWidth() const { return size.w; }
    int32_t getHeight() const { return size.h; }

    void setNeedsRender();

    void setParent(Component *p);

    bool hasAutoWidth() const { return hasAutoWidthEnabled; }
    bool hasAutoHeight() const { return hasAutoHeightEnabled; }

protected:
    virtual void doRender() = 0;
    void init();
    void initAutoSizeFlags()
    {
        this->hasAutoWidthEnabled = (size.w == 0);
        this->hasAutoHeightEnabled = (size.h == 0);
    }

private:
    Position position = {0, 0};
    Size size = {0, 0};
    bool isRendered = false;
    String id = "";
    Component *parent = nullptr;
    bool hasAutoWidthEnabled = false;
    bool hasAutoHeightEnabled = false;
};