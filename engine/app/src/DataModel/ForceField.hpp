

#pragma once

#include "Tree/Instance.hpp"
#include "Utility/RunStateOwner.hpp"
#include "GfxBase/IAdornable.hpp"
#include "GfxBase/Adorn.hpp"
#include "AppDraw/DrawAdorn.hpp"
#include "DataModel/Effect.hpp"

namespace Aya
{

class PartInstance;

static const float largeSize = 1.1f;

extern const char* const sForceField;
class ForceField
    : public DescribedCreatable<ForceField, Instance, sForceField>
    , public IAdornable
    , public Effect
{
private:
    // typedef DescribedCreatable<ForceField, Instance, sForceField> Super;

    Time startTime;
    int cycle;
    int invertCycle;
    shared_ptr<Instance> torso;

    // Instance
    /*override*/ bool askSetParent(const Instance* instance) const;

    // IAdornable
    /*override*/ bool shouldRender2d() const
    {
        return true;
    }
    /*override*/ bool shouldRender3dAdorn() const
    {
        return true;
    }
    /*override*/ bool shouldRender3d() const
    {
        return true;
    }
    /*override*/ bool shouldRender3dSortedAdorn() const
    {
        return true;
    }
    /*override*/ void render3dAdorn(Adorn* adorn);

public:
    ForceField();
    virtual ~ForceField() {}

    static bool partInForceField(PartInstance* part);
    static int cycles()
    {
        return 60;
    }
};
} // namespace Aya