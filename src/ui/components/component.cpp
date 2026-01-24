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
    const auto needsRender = this->needsRenderCheck() || force;
    if (!needsRender)
        return;

    Serial.println("Rendering Component: " + this->getName() + " (" + this->getId() + ")");

    this->doRender();
    this->isRendered = true;
    this->hasChildThatNeedsRender = false;
}

bool Component::isTouched(int32_t x, int32_t y) const
{
    return (x > this->position.x && x < this->position.x + this->size.w &&
            y > this->position.y && y < this->position.y + this->size.h);
}

void Component::setNeedsRender()
{
    this->isRendered = false;
    this->markParentsChildNeedsRender();
}

void Component::markParentsChildNeedsRender()
{
    Component *current = this->parent;
    while (current != nullptr)
    {
        if (current->hasChildThatNeedsRender)
        {
            break; // Already marked, no need to continue
        }
        current->hasChildThatNeedsRender = true;
        current = current->parent;
    }
}

void Component::setParent(Component *p)
{
    this->parent = p;
}
