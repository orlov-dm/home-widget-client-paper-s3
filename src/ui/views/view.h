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

constexpr int8_t DEFAULT_SEPARATOR_THICKNESS = 2;

template <typename Derived>
class ViewBase : public Component
{
public:
    ViewBase(const Position &position, const Size &size, LayoutDirection direction = LayoutDirection::Vertical) : Component(position, size), direction(direction) {};
    ViewBase(const Size &size, LayoutDirection direction = LayoutDirection::Vertical) : Component(size), direction(direction) {};
    ViewBase(const String &id, LayoutDirection direction = LayoutDirection::Vertical) : Component(id), direction(direction) {};

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

    Derived *setSeparatorSize(const Size &s)
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

        child->setParent(this);
        children.push_back(std::move(child));
        setNeedsRender();
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

        M5.Display.fillRect(position.x, position.y, size.w, size.h, this->backgroundColor);

        this->normalizeAllChildrenSizes();
        for (int i = 0; i < children.size(); i++)
        {
            auto &child = children[i];
            if (child == nullptr)
                continue;
            child->setPosition({currentX, currentY});
            auto childSize = child.get()->getSize();
            this->doRenderChild(*child);

            if (i < children.size() - 1)
                this->renderSeparator(currentX, currentY, childSize);

            if (this->direction == LayoutDirection::Vertical)
                currentY += childSize.h + this->spacing;
            else
                currentX += childSize.w + this->spacing;
        }
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

    void normalizeAllChildrenSizes()
    {
        if (this->direction == LayoutDirection::Vertical)
        {
            int32_t totalFixedHeight = 0;
            int32_t numAutoHeightChildren = 0;
            for (auto &child : children)
            {
                if (child == nullptr)
                    continue;
                auto childSize = child->getSize();
                if (childSize.h == 0)
                {
                    numAutoHeightChildren++;
                }
                else
                {
                    totalFixedHeight += childSize.h;
                }
            }
            int32_t availableHeight = this->getSize().h - (this->padding * 2) - (this->spacing * (children.size() - 1)) - totalFixedHeight;
            int32_t autoHeight = numAutoHeightChildren > 0 ? availableHeight / numAutoHeightChildren : 0;

            for (auto &child : children)
            {
                if (child == nullptr)
                    continue;
                auto childSize = child->getSize();
                auto childHeight = childSize.h == 0 ? autoHeight : childSize.h;
                auto childWidth = childSize.w == 0 ? this->getSize().w - (this->padding * 2) : childSize.w;
                child->setSize({childWidth, childHeight});
            }
        }
        else
        {
            int32_t totalFixedWidth = 0;
            int32_t numAutoWidthChildren = 0;
            for (auto &child : children)
            {
                if (child == nullptr)
                    continue;
                auto childSize = child->getSize();
                if (childSize.w == 0)
                {
                    numAutoWidthChildren++;
                }
                else
                {
                    totalFixedWidth += childSize.w;
                }
            }
            int32_t availableWidth = this->getSize().w - (this->padding * 2) - (this->spacing * (children.size() - 1)) - totalFixedWidth;
            int32_t autoWidth = numAutoWidthChildren > 0 ? availableWidth / numAutoWidthChildren : 0;

            for (auto &child : children)
            {
                if (child == nullptr)
                    continue;
                auto childSize = child->getSize();
                auto childWidth = childSize.w == 0 ? autoWidth : childSize.w;
                auto childHeight = childSize.h == 0 ? this->getSize().h - (this->padding * 2) : childSize.h;
                child->setSize({childWidth, childHeight});
            }
        }
    }

    void renderSeparator(int32_t x, int32_t y, const Size &childSize)
    {
        bool hasSeparator = (this->separatorSize.w >= 0 && this->separatorSize.h >= 0);
        if (!hasSeparator)
            return;
        if (this->separatorSize.w != 0 || this->separatorSize.h != 0)
        {
            Size separatorSize = this->separatorSize;
            if (this->direction == LayoutDirection::Vertical)
            {
                if (separatorSize.w == 0)
                    separatorSize.w = this->getSize().w - (this->padding * 2);
                if (separatorSize.h == 0)
                    separatorSize.h = DEFAULT_SEPARATOR_THICKNESS;
            }
            else
            {
                if (separatorSize.h == 0)
                    separatorSize.h = this->getSize().h - (this->padding * 2);
                if (separatorSize.w == 0)
                    separatorSize.w = DEFAULT_SEPARATOR_THICKNESS;
            }
            this->separatorSize = separatorSize;
        }

        if (this->separatorSize.w <= 0 || this->separatorSize.h <= 0) {
            Serial.println("Invalid separator size");
            return;
        }

        const auto &position = this->getPosition();
        if (this->direction == LayoutDirection::Vertical)
        {
            M5.Display.fillRect(
                position.x + this->padding,
                y + childSize.h + (this->spacing / 2),
                this->separatorSize.w,
                this->separatorSize.h,
                TFT_DARKGRAY);
        }
        else
        {
            M5.Display.fillRect(
                x + childSize.w + (this->spacing / 2),
                position.y + this->padding,
                this->separatorSize.w,
                this->separatorSize.h,
                TFT_DARKGRAY);
        }
    }

private:
    std::vector<std::unique_ptr<Component>> children;

    LayoutDirection direction = LayoutDirection::Vertical;
    int32_t backgroundColor = TFT_WHITE;

    Size separatorSize = {-1, -1};
    int32_t spacing = 0;
    int32_t padding = 0;
};

// Non-template View class for simple usage
class View : public ViewBase<View>
{
public:
    using ViewBase<View>::ViewBase;
};