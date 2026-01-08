#pragma once

#include <vector>
#include <memory>
#include <M5Unified.hpp>
#include <Arduino.h>

#include "../components/component.h"

enum class LayoutDirection
{
    Vertical,
    Horizontal
};

template <typename Derived>
class ViewBase : public Component
{
public:
    ViewBase(const Position &position, const Size &size, LayoutDirection direction = LayoutDirection::Vertical) : Component(position, size), direction(direction) {};
    ViewBase(const Size &size, LayoutDirection direction = LayoutDirection::Vertical) : Component(size), direction(direction) {};

    virtual ~ViewBase()
    {
        resetChildren();
    };

    Derived *setSpacing(int32_t s)
    {
        this->spacing = s;
        setNeedsRender();
        return static_cast<Derived *>(this);
    }

    Derived *setSeparatorSize(Size s)
    {
        this->separatorSize = s;
        setNeedsRender();
        return static_cast<Derived *>(this);
    }

    Derived *setBackgroundColor(uint32_t color)
    {
        this->backgroundColor = color;
        setNeedsRender();
        return static_cast<Derived *>(this);
    }

    Derived *setPadding(int32_t p)
    {
        this->padding = p;
        setNeedsRender();
        return static_cast<Derived *>(this);
    }

    void resetChildren()
    {
        children.clear();
        setNeedsRender();
    }

    void addChild(std::unique_ptr<Component> child)
    {
        if (child == nullptr)
            return;

        children.push_back(std::move(child));
    }

    virtual void doRenderChild(Component &child)
    {
        child.render();
    }

    void doRender() override
    {
        auto position = this->getPosition();
        auto size = this->getSize();
        int32_t currentY = position.y + this->padding;
        int32_t currentX = position.x + this->padding;

        for (int i = 0; i < children.size(); i++)
        {
            auto &child = children[i];
            if (child == nullptr)
                continue;
            child->setPosition({currentX, currentY});
            auto childSize = this->normalizeChildSize(child.get());
            Serial.printf("Rendering child at (%d, %d) size (%d, %d) %d\n", currentX, currentY, childSize.w, childSize.h, static_cast<int>(this->direction));
            this->doRenderChild(*child);

            if (this->direction == LayoutDirection::Vertical)
            {
                // draw a separator if needed starting from x + padding in the between of 2 children
                if (separatorSize.w > 0 && separatorSize.h > 0 && i < children.size() - 1)
                {
                    M5.Display.fillRect(
                        position.x + this->padding,
                        currentY + childSize.h + (this->spacing / 2),
                        separatorSize.w,
                        separatorSize.h,
                        TFT_DARKGRAY);
                }

                currentY += childSize.h + this->spacing;
            }
            else
            {
                // draw a separator if needed starting from x + padding in the between of 2 children
                if (separatorSize.w > 0 && separatorSize.h > 0 && i < children.size() - 1)
                {
                    M5.Display.fillRect(
                        currentX + childSize.w + (this->spacing / 2),
                        position.y + this->padding,
                        separatorSize.w,
                        separatorSize.h,
                        TFT_DARKGRAY);
                }
                currentX += childSize.w + this->spacing;
            }
        }

        if (this->direction == LayoutDirection::Vertical)
        {
            currentY -= this->spacing; // Remove last spacing
        }
        else
        {
            currentX -= this->spacing; // Remove last spacing
        }
        clearRemainingArea(this->direction == LayoutDirection::Vertical ? currentY : currentX);
    }

protected:
    Size normalizeChildSize(Component *child)
    {
        if (child == nullptr)
            return {0, 0};
        auto viewSize = this->getSize();
        auto viewPosition = this->getPosition();
        auto childSize = child->getSize();
        if (direction == LayoutDirection::Vertical)
        {
            if (childSize.w == 0)
            {
                childSize.w = viewSize.w - (this->padding * 2);
                child->setSize({childSize.w, childSize.h});
            }
            if (childSize.h == 0)
            {
                childSize.h = (viewSize.h - (this->padding * 2) - (children.size() - 1) * this->spacing) / children.size();
                child->setSize({childSize.w, childSize.h});
            }
        }
        else
        {
            if (childSize.h == 0)
            {
                childSize.h = viewSize.h - (this->padding * 2);
                child->setSize({childSize.w, childSize.h});
            }
            if (childSize.w == 0)
            {
                childSize.w = (viewSize.w - (this->padding * 2) - (children.size() - 1) * this->spacing) / children.size();
                child->setSize({childSize.w, childSize.h});
            }
        }
        return childSize;
    }

    void clearRemainingArea(int32_t lastCoordinate)
    {
        auto position = this->getPosition();
        auto size = this->getSize();
        if (this->direction == LayoutDirection::Horizontal)
        {
            if (lastCoordinate < position.x + size.w)
            {
                M5.Display.fillRect(
                    lastCoordinate,
                    position.y,
                    (position.x + size.w) - lastCoordinate,
                    size.h,
                    this->backgroundColor);
            }
        }
        else
        {
            if (lastCoordinate < position.y + size.h)
            {
                M5.Display.fillRect(
                    position.x,
                    lastCoordinate,
                    size.w,
                    (position.y + size.h) - lastCoordinate,
                    this->backgroundColor);
            }
        }
    }

private:
    std::vector<std::unique_ptr<Component>> children;

    LayoutDirection direction = LayoutDirection::Vertical;
    int32_t backgroundColor = TFT_WHITE;

    Size separatorSize = {0, 0};
    int32_t spacing = 0;
    int32_t padding = 0;
};

// Non-template View class for simple usage
class View : public ViewBase<View>
{
public:
    using ViewBase<View>::ViewBase;
};