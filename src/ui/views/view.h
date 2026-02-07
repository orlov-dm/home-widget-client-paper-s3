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
        if (this->spacing == s)
            return static_cast<Derived *>(this);
        this->spacing = s;
        setNeedsRender();
        return static_cast<Derived *>(this);
    }

    Derived *setSeparatorSize(const Size &s)
    {
        if (this->separatorSize.w == s.w && this->separatorSize.h == s.h)
            return static_cast<Derived *>(this);
        this->separatorSize = s;
        setNeedsRender();
        return static_cast<Derived *>(this);
    }

    Derived *setBackgroundColor(uint32_t color)
    {
        if (this->backgroundColor == color)
            return static_cast<Derived *>(this);
        this->backgroundColor = color;
        setNeedsRender();
        return static_cast<Derived *>(this);
    }

    Derived *setPadding(int32_t p)
    {
        if (this->padding == p)
            return static_cast<Derived *>(this);
        this->padding = p;
        setNeedsRender();
        return static_cast<Derived *>(this);
    }

    void resetChildren()
    {
        if (children.empty())
            return;

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

    virtual void doRenderChild(Component &child, bool forceRender = false)
    {
        child.render(forceRender);
    }

    void doRender() override
    {
        auto position = this->getPosition();
        auto size = this->getSize();
        int32_t currentY = position.y + this->padding;
        int32_t currentX = position.x + this->padding;

        bool viewNeedsFullRender = !this->getIsRendered();

        // Calculate minimum size from children before normalizing
        // This ensures nested Views with auto-size calculate their minimums first
        if (this->hasAutoWidth() || this->hasAutoHeight())
        {
            Size minSize = this->calculateMinimumSize();
            if (this->hasAutoWidth())
                this->setWidth(minSize.w);
            if (this->hasAutoHeight())
                this->setHeight(minSize.h);
        }

        // Only clear background if THIS view needs rendering, not just children
        if (viewNeedsFullRender)
        {
            Serial.println("Rendering View: " + this->getName() + " (" + this->getId() + ")");
            Serial.println("Size: " + String(size.w) + "x" + String(size.h) + ", Position: (" + String(position.x) + "," + String(position.y) + ")");
            M5.Display.fillRect(position.x, position.y, size.w, size.h, this->backgroundColor);
        }

        this->normalizeAllChildrenSizes();
        for (int i = 0; i < children.size(); i++)
        {
            auto &child = children[i];
            if (child == nullptr || !child->isVisible())
                continue;

            auto childSize = child.get()->getSize();
            if (child->getAlignment() == Alignment::CENTER)
            {
                if (direction == LayoutDirection::Vertical)
                {
                    int32_t centeredX = position.x + padding + (size.w - padding * 2 - childSize.w) / 2;
                    child->setPosition({centeredX, currentY});
                }
                else
                {
                    int32_t centeredY = position.y + padding + (size.h - padding * 2 - childSize.h) / 2;
                    child->setPosition({currentX, centeredY});
                }
            } else if (child->getAlignment() == Alignment::RIGHT)
            {
                if (direction == LayoutDirection::Vertical)
                {
                    int32_t rightAlignedX = position.x + size.w - padding - childSize.w;
                    child->setPosition({rightAlignedX, currentY});
                }
                else
                {
                    int32_t rightAlignedY = position.y + size.h - padding - childSize.h;
                    child->setPosition({currentX, rightAlignedY});
                }
            }
            else
            {
                child->setPosition({currentX, currentY});
            }

            // If parent needs full render, force render all children. Otherwise only render those that need it
            if (viewNeedsFullRender)
            {
                this->doRenderChild(*child, true); // Force render since background was cleared
            }
            else if (child->needsRenderCheck())
            {
                this->doRenderChild(*child);
            }

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

    Size calculateMinimumSize() const override
    {
        if (children.empty())
            return this->getSize();

        int32_t totalMainAxisSize = 0;
        int32_t maxCrossAxisSize = 0;
        int32_t visibleChildCount = 0;

        for (const auto &child : children)
        {
            if (!child || !child->isVisible())
                continue;

            // Recursive call - works for both View and Component children
            // For nested Views with auto-size, this will calculate their size from their children
            Size childMin = child->calculateMinimumSize();
            visibleChildCount++;

            if (this->direction == LayoutDirection::Vertical)
            {
                totalMainAxisSize += childMin.h;
                if (childMin.w > maxCrossAxisSize)
                    maxCrossAxisSize = childMin.w;
            }
            else
            {
                totalMainAxisSize += childMin.w;
                if (childMin.h > maxCrossAxisSize)
                    maxCrossAxisSize = childMin.h;
            }
        }

        int32_t effectiveSpacing = this->getEffectiveSpacing();
        int32_t spacingTotal = visibleChildCount > 1 ? effectiveSpacing * (visibleChildCount - 1) : 0;

        Size minSize = this->getSize();

        if (this->direction == LayoutDirection::Vertical)
        {
            if (this->hasAutoHeight())
                minSize.h = totalMainAxisSize + spacingTotal + (this->padding * 2);
            if (this->hasAutoWidth())
                minSize.w = maxCrossAxisSize + (this->padding * 2);
        }
        else
        {
            if (this->hasAutoWidth())
                minSize.w = totalMainAxisSize + spacingTotal + (this->padding * 2);
            if (this->hasAutoHeight())
                minSize.h = maxCrossAxisSize + (this->padding * 2);
        }

        return minSize;
    }

protected:
    void normalizeAllChildrenSizes()
    {
        if (this->direction == LayoutDirection::Vertical)
        {
            int32_t totalFixedHeight = 0;
            int32_t totalAutoMinHeight = 0;
            int32_t numAutoHeightChildren = 0;
            for (auto &child : children)
            {
                if (child == nullptr || !child->isVisible())
                    continue;
                if (child->hasAutoHeight())
                {
                    numAutoHeightChildren++;
                    Size childMin = child->calculateMinimumSize();
                    totalAutoMinHeight += childMin.h;
                }
                else
                {
                    totalFixedHeight += child->getSize().h;
                }
            }
            int32_t effectiveSpacing = this->getEffectiveSpacing();
            int32_t availableHeight = this->getSize().h - (this->padding * 2) - (effectiveSpacing * (children.size() - 1)) - totalFixedHeight - totalAutoMinHeight;
            int32_t extraPerAutoChild = numAutoHeightChildren > 0 ? max((int32_t)0, availableHeight) / numAutoHeightChildren : 0;

            for (auto &child : children)
            {
                if (child == nullptr || !child->isVisible())
                    continue;
                int32_t childHeight;
                if (child->hasAutoHeight())
                {
                    Size childMin = child->calculateMinimumSize();
                    childHeight = childMin.h + extraPerAutoChild;
                }
                else
                {
                    childHeight = child->getSize().h;
                }
                auto childWidth = child->hasAutoWidth() ? this->getSize().w - (this->padding * 2) : child->getSize().w;
                child->setSize({childWidth, childHeight});
            }
        }
        else
        {
            int32_t totalFixedWidth = 0;
            int32_t totalAutoMinWidth = 0;
            int32_t numAutoWidthChildren = 0;
            for (auto &child : children)
            {
                if (child == nullptr || !child->isVisible())
                    continue;
                if (child->hasAutoWidth())
                {
                    numAutoWidthChildren++;
                    Size childMin = child->calculateMinimumSize();
                    totalAutoMinWidth += childMin.w;
                }
                else
                {
                    totalFixedWidth += child->getSize().w;
                }
            }
            int32_t effectiveSpacing = this->getEffectiveSpacing();
            int32_t availableWidth = this->getSize().w - (this->padding * 2) - (effectiveSpacing * (children.size() - 1)) - totalFixedWidth - totalAutoMinWidth;
            int32_t extraPerAutoChild = numAutoWidthChildren > 0 ? max((int32_t)0, availableWidth) / numAutoWidthChildren : 0;

            for (auto &child : children)
            {
                if (child == nullptr || !child->isVisible())
                    continue;
                int32_t childWidth;
                if (child->hasAutoWidth())
                {
                    Size childMin = child->calculateMinimumSize();
                    childWidth = childMin.w + extraPerAutoChild;
                }
                else
                {
                    childWidth = child->getSize().w;
                }
                auto childHeight = child->hasAutoHeight() ? this->getSize().h - (this->padding * 2) : child->getSize().h;
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