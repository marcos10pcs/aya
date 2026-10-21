
#include "GUI/EquationDisplay.hpp"
#include "Utility/IMetric.hpp"
#include "aya/Profiler.hpp"

namespace Aya
{

///////////////////////////////////////////////////////////////////////////
//
// EquationDisplay
//

EquationDisplay::EquationDisplay(const std::string& title, const std::string& eqText)
    : TextDisplay(title, title)
    , equation(eqText)
{
}


EquationDisplay::EquationDisplay(const std::string& title, const std::string& label, const std::string& eqText)
    : TextDisplay(title, label)
    , equation(eqText)
{
}


std::string EquationDisplay::getLabel() const
{
    const Instance* root = Instance::getRootAncestor(this);
    const IMetric* metric = dynamic_cast<const IMetric*>(root);
    if (metric)
    {
        RBXASSERT(metric);
        std::string answer = metric->getMetric(equation);
        return label + " " + answer;
    }
    else
    {
        return label;
    }
}

void EquationDisplay::render2d(Adorn* adorn)
{
    if (isVisible())
    {
        Rect myRect = this->getMyRect(adorn->getCanvas());
        Vector2 s = myRect.size();
        Vector2 pos = myRect.center();
        switch (align)
        {
        default:
        case Text::XALIGN_LEFT:
            pos.x = myRect.low.x + (0.1f * s.x);
            break;

        case Text::XALIGN_RIGHT:
            pos.x = myRect.high.x - (0.1f * s.x);
            break;

        case Text::XALIGN_CENTER: // pos.x == center
            break;
        }

        int red = this->fontColor.r * 255;
        int green = this->fontColor.g * 255;
        int blue = this->fontColor.b * 255;

        uint32_t color = (red << 16) | (green << 8) | blue;
#ifndef __APPLE__
        ExternalMicroProfileDrawText(pos.x, pos.y, color, getLabel().c_str(), getLabel().length());
#endif
    }
}


} // namespace Aya