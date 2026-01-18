#include "component.h"
#include "../../app/component_manager.h"

Component::Component(const Position &position, const Size &size)
    : position(position), size(size)
{
    this->init();
}

Component::Component(const Size &size, const String &id)
    : position({0, 0}), size(size), id(id)
{
    this->init();
}

Component::Component(const String &id)
    : position({0, 0}), size({0, 0}), id(id)
{
    this->init();
}

Component::Component(const Size &size)
    : position({0, 0}), size(size)
{
    this->init();
}

Component::Component()
    : position({0, 0}), size({0, 0})
{
    this->init();
}

Component::~Component()
{
    if (id.length() > 0)
        ComponentManager::getInstance().unregisterComponent(id);
}

void Component::init()
{
    if (this->id.length() > 0)
    {
        ComponentManager::getInstance().registerComponent(this->id, this);
    }
    this->initAutoSizeFlags();
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
