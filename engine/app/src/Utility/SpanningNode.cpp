


#include "Utility/SpanningNode.hpp"
#include "Utility/SpanningEdge.hpp"


namespace Aya
{


// Having a parent edge == in the spanning tree

void SpanningNode::setEdgeToParent(SpanningEdge* edge)
{
    edgeToParent = edge;
}

bool SpanningNode::lessThan(const IndexedTree* other) const
{
    RBXASSERT(edgeToParent);
    const SpanningNode* otherNode = rbx_static_cast<const SpanningNode*>(other);
    RBXASSERT(otherNode->getConstEdgeToParent());
    return edgeToParent->isLighterThan(otherNode->getConstEdgeToParent());
}


} // namespace Aya
