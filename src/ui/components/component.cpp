#include "component.h"
#include "../../app/component_manager.h"

Component::Component(const Position &position, const Size &size)
    : position(position), size(size)
{
}

Component::Component(const Size &size, const String &id)
    : position({0, 0}), size(size), id(id)
{
    ComponentManager::getInstance().registerComponent(id, this);
}

Component::Component(const String &id)
    : position({0, 0}), size({0, 0}), id(id)
{
    ComponentManager::getInstance().registerComponent(id, this);
}

Component::Component(const Size &size)
    : position({0, 0}), size(size)
{
}

Component::Component()
    : position({0, 0}), size({0, 0})
{
}

Component::~Component()
{
    if (id.length() > 0)
        ComponentManager::getInstance().unregisterComponent(id);
}

void Component::render(bool force)
{
    if (this->isRendered && !force)
        return;

    this->doRender();
    this->isRendered = true;
}

bool Component::isTouched(int32_t x, int32_t y) const
{
    return (x > position.x && x < position.x + size.w &&
            y > position.y && y < position.y + size.h);
}

void Component::setNeedsRender()
{
    this->isRendered = false;
    if (this->parent)
    {
        this->parent->setNeedsRender();
    }
}

void Component::setParent(Component *p)
{
    this->parent = p;
}
