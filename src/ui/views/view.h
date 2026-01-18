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

            int32_t effectiveSpacing = this->getEffectiveSpacing();
            if (this->direction == LayoutDirection::Vertical)
                currentY += childSize.h + effectiveSpacing;
            else
                currentX += childSize.w + effectiveSpacing;
        }
    }

    virtual void onEnter() {}
    virtual void onExit() {}
    virtual void onRefresh() {}

protected:
    void normalizeAllChildrenSizes()
    {
        Serial.printf("Normalizing child sizes %s\n", this->getId().c_str());
        if (this->direction == LayoutDirection::Vertical)
        {
            int32_t totalFixedHeight = 0;
            int32_t numAutoHeightChildren = 0;
            for (auto &child : children)
            {
                if (child == nullptr)
                    continue;
                auto childSize = child->getSize();
                if (child->hasAutoHeight())
                {
                    numAutoHeightChildren++;
                }
                else
                {
                    totalFixedHeight += childSize.h;
                }
            }
            int32_t effectiveSpacing = this->getEffectiveSpacing();
            int32_t availableHeight = this->getSize().h - (this->padding * 2) - (effectiveSpacing * (children.size() - 1)) - totalFixedHeight;
            int32_t autoHeight = numAutoHeightChildren > 0 ? availableHeight / numAutoHeightChildren : 0;

            for (auto &child : children)
            {
                if (child == nullptr)
                    continue;
                auto childSize = child->getSize();
                auto childHeight = child->hasAutoHeight() ? autoHeight : childSize.h;
                auto childWidth = child->hasAutoWidth() ? this->getSize().w - (this->padding * 2) : childSize.w;
                child->setSize({childWidth, childHeight});
            }

            Serial.printf("Auto height: %d, Available height: %d, Total fixed height: %d, Num auto height children: %d\n",
                          autoHeight, availableHeight, totalFixedHeight, numAutoHeightChildren);
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
                if (child->hasAutoWidth())
                {
                    numAutoWidthChildren++;
                }
                else
                {
                    totalFixedWidth += childSize.w;
                }
            }
            int32_t effectiveSpacing = this->getEffectiveSpacing();
            int32_t availableWidth = this->getSize().w - (this->padding * 2) - (effectiveSpacing * (children.size() - 1)) - totalFixedWidth;
            int32_t autoWidth = numAutoWidthChildren > 0 ? availableWidth / numAutoWidthChildren : 0;

            for (auto &child : children)
            {
                if (child == nullptr)
                    continue;
                auto childSize = child->getSize();
                auto childWidth = child->hasAutoWidth() ? autoWidth : childSize.w;
                auto childHeight = child->hasAutoHeight() ? this->getSize().h - (this->padding * 2) : childSize.h;
                child->setSize({childWidth, childHeight});
            }
            Serial.printf("Auto width: %d, Available width: %d, Total fixed width: %d, Num auto width children: %d\n",
                          autoWidth, availableWidth, totalFixedWidth, numAutoWidthChildren);
        }
    }

    void renderSeparator(int32_t x, int32_t y, const Size &childSize)
    {
        bool hasSeparator = (this->separatorSize.w >= 0 && this->separatorSize.h >= 0);
        Serial.printf("Rendering separator at (%d, %d) after child size (%d, %d) - Has separator: %d\n",
                      x, y, childSize.w, childSize.h, hasSeparator);
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

        if (this->separatorSize.w <= 0 || this->separatorSize.h <= 0)
        {
            Serial.println("Invalid separator size");
            return;
        }

        const auto &position = this->getPosition();
        int32_t effectiveSpacing = this->getEffectiveSpacing();
        if (this->direction == LayoutDirection::Vertical)
        {
            M5.Display.fillRect(
                position.x + this->padding,
                y + childSize.h + (effectiveSpacing - this->separatorSize.h) / 2,
                this->separatorSize.w,
                this->separatorSize.h,
                TFT_DARKGRAY);
        }
        else
        {
            M5.Display.fillRect(
                x + childSize.w + (effectiveSpacing - this->separatorSize.w) / 2,
                position.y + this->padding,
                this->separatorSize.w,
                this->separatorSize.h,
                TFT_DARKGRAY);
        }
    }

    void recalculateLayout()
    {
        setNeedsRender();
    }

    int32_t getEffectiveSpacing() const
    {
        if (this->spacing != 0)
            return this->spacing;
        if (this->direction == LayoutDirection::Vertical && this->separatorSize.h > 0)
            return this->separatorSize.h;
        if (this->direction == LayoutDirection::Horizontal && this->separatorSize.w > 0)
            return this->separatorSize.w;
        return 0;
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