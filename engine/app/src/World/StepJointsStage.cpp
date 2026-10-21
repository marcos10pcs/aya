


#include "World/StepJointsStage.hpp"
#include "World/HumanoidStage.hpp"
#include "World/Primitive.hpp"
#include "World/Assembly.hpp"
#include "World/MotorJoint.hpp"
#include "Utility/Profiling.hpp"
#include "Utility/Region2.hpp"


namespace Aya
{

#pragma warning(push)
#pragma warning(disable : 4355) // 'this' : used in base member initializer list
StepJointsStage::StepJointsStage(IStage* upstream, World* world)
    : IWorldStage(upstream, new HumanoidStage(this, world), world)
    , profilingJointUpdate(new Profiling::CodeProfiler("Joint Update"))
{
}
#pragma warning(pop)

StepJointsStage::~StepJointsStage()
{
    RBXASSERT(worldStepJoints.empty());
}


void StepJointsStage::addJoint(Joint* j)
{
    if (j->canStepWorld())
    {
        worldStepJoints.push_back(*j);
    }
}

void StepJointsStage::removeJoint(Joint* j)
{
    if (j->canStepWorld())
    {
        worldStepJoints.erase(worldStepJoints.iterator_to(*j));
    }
    else
    {
        RBXASSERT(!j->StepJointsStageHook::is_linked());
    }
}

///////////////////////////////////////////////////////////////////////////////


void StepJointsStage::onSimulateAssemblyAdded(Assembly* a)
{
    a->putInStage(this);

    rbx_static_cast<HumanoidStage*>(getDownstreamWS())->onAssemblyAdded(a);
}


void StepJointsStage::onSimulateAssemblyRemoving(Assembly* a)
{
    rbx_static_cast<HumanoidStage*>(getDownstreamWS())->onAssemblyRemoving(a);

    a->removeFromStage(this);
}


void StepJointsStage::onEdgeAdded(Edge* e)
{
    e->putInStage(this);

    if (Joint::isJoint(e))
    {
        Joint* j = rbx_static_cast<Joint*>(e);
        addJoint(j);
    }

    if (!Joint::isKinematicJoint(e))
    {
        getDownstreamWS()->onEdgeAdded(e);
    }
}

void StepJointsStage::onEdgeRemoving(Edge* e)
{
    if (!Joint::isKinematicJoint(e))
    {
        getDownstreamWS()->onEdgeRemoving(e);
    }

    if (Joint::isJoint(e))
    {
        Joint* j = rbx_static_cast<Joint*>(e);
        removeJoint(j);
    }

    e->removeFromStage(this);
}




void StepJointsStage::jointsStepWorld()
{
    Aya::Profiling::Mark mark(*profilingJointUpdate, false);

    for (Joints::iterator it = worldStepJoints.begin(); it != worldStepJoints.end(); ++it)
    {
        RBXASSERT(it->canStepWorld());
        it->stepWorld();
    }
}



} // namespace Aya
