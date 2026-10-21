#include "GL/DeviceGL.hpp"

namespace Aya
{
namespace Graphics
{

Device* Device::create(API api, void* windowHandle)
{

    if (api == API_OpenGL)
        return new DeviceGL(windowHandle);
    throw Aya::runtime_error("Unsupported API: %d", api);
}

} // namespace Graphics
} // namespace Aya
