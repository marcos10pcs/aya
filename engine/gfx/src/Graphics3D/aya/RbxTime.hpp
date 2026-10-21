#ifndef RBX_TIME_H
#define RBX_TIME_H
#include "Graphics3D/G3DGameUnits.hpp"

namespace Aya
{
class RbxTime
{
public:
    static G3D::RealTime getTick();

private:
    static G3D::RealTime m_startTime;
};
} // namespace Aya

#endif