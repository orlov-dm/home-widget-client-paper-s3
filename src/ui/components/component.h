#pragma once

#include "types.h"

class Component
{
public:
    Component(Position position, Size size) : position(position), size(size) {};
    virtual ~Component() {};

    void render()
    {
        if (this->isRendered)
            return;

        this->doRender();
        this->isRendered = true;
    }

    bool isTouched(int32_t x, int32_t y) const
    {
        return (x > position.x && x < position.x + size.w &&
                y > position.y && y < position.y + size.h);
    }

    const Position &getPosition() const { return position; }
    const Size &getSize() const { return size; }

    void setNeedsRender()
    {
        this->isRendered = false;
    }

protected:
    virtual void doRender() = 0;

private:
    Position position;
    Size size;
    bool isRendered = false;
};