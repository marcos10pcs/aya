#pragma once

#include "Utility/SystemAddress.hpp"

#include "Utility/G3DCore.hpp"

#include "RakNet/RakNetTypes.hpp"

#ifdef _WIN32
#if defined(_NOOPT) || defined(_DEBUG) || defined(RBX_TEST_BUILD)
#define NETWORK_PROFILER
#define NETWORK_DEBUG
#endif
#endif

namespace Aya
{

namespace SpatialRegion
{
class Id;
}

namespace Network
{

const Aya::SystemAddress RakNetToRbxAddress(const RakNet::SystemAddress& raknetAddress);
std::string RakNetAddressToString(const RakNet::SystemAddress& raknetAddress, bool writePort = true, char portDelineator = '|');

} // namespace Network
} // namespace Aya
