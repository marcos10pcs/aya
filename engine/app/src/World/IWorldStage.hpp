

#pragma once

#include "Kernel/IStage.hpp"
#include "aya/Debug.hpp"
#include "Utility/G3DCore.hpp"

namespace Aya
{

class World;
class Edge;
class Contact;
class Primitive;

class RBXBaseClass IWorldStage : public IStage
{
private:
    World* world;

public:
    typedef enum
    {
        NUM_CONTACTSTAGE_CONTACTS,
        NUM_STEPPING_CONTACTS,
        NUM_TOUCHING_CONTACTS,
        MAX_TREE_DEPTH
    } MetricType;

    IWorldStage(IStage* upstream, IStage* downstream, World* world)
        : IStage(upstream, downstream)
        , world(world)
    {
    }

    IWorldStage* getUpstreamWS()
    {
        return rbx_static_cast<IWorldStage*>(getUpstream());
    }
    IWorldStage* getDownstreamWS()
    {
        return rbx_static_cast<IWorldStage*>(getDownstream());
    }
    const IWorldStage* getDownstreamWS() const
    {
        return rbx_static_cast<const IWorldStage*>(getDownstream());
    }

    World* getWorld()
    {
        return world;
    }

    ////////////////////////////////////////////
    //
    // Calls to DOWNSTREAM stage
    virtual void onEdgeAdded(Edge* e);
    virtual void onEdgeRemoving(Edge* e);

    virtual int getMetric(MetricType metricType)
    {
        RBXASSERT(getDownstreamWS());
        return getDownstreamWS()->getMetric(metricType);
    }
};
} // namespace Aya