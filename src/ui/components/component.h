#pragma once

#include "../types.h"
#include "../../app/component_manager.h"

class Component
{
public:
    Component(const Position &position, const Size &size) : position(position), size(size) {};
    Component(const Size &size, const String &id) : position({0, 0}), size(size), id(id)
    {
        ComponentManager::getInstance().registerComponent(id, this);
    };
    Component(const String &id) : position({0, 0}), size({0, 0}), id(id)
    {
        ComponentManager::getInstance().registerComponent(id, this);
    };
    Component(const Size &size) : position({0, 0}), size(size) {};
    Component() : position({0, 0}), size({0, 0}) {};

    virtual ~Component()
    {
        if (id.length() > 0)
            ComponentManager::getInstance().unregisterComponent(id);
    };

    void render(bool force = false)
    {
        if (this->isRendered && !force)
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
        setNeedsRender();
    }

    void setWidth(int32_t w)
    {
        size.w = w;
        setNeedsRender();
    }

    void setHeight(int32_t h)
    {
        size.h = h;
        setNeedsRender();
    }

    void setNeedsRender()
    {
        this->isRendered = false;
        if (this->parent)
        {
            this->parent->setNeedsRender();
        }
    }

    void setParent(Component *p)
    {
        this->parent = p;
    }

protected:
    virtual void doRender() = 0;

private:
    Position position = {0, 0};
    Size size = {0, 0};
    bool isRendered = false;
    String id = "";
    Component *parent = nullptr;
};