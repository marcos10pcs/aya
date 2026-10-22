#include "RbxPlatform.hpp"
#include "Utility/MachineIdUploader.hpp"

#include <cstring>
#include <string>

#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace Aya;

bool MachineIdUploader::fillMachineId(MachineId* out)
{
    struct ifaddrs* addresses = NULL;
    if (getifaddrs(&addresses) != 0)
        return false;

    bool success = false;

    for (struct ifaddrs* current = addresses; current != NULL; current = current->ifa_next)
    {
        if (current->ifa_addr == NULL)
            continue;

        if (current->ifa_addr->sa_family != AF_INET && current->ifa_addr->sa_family != AF_INET6)
            continue;

        if ((current->ifa_flags & IFF_LOOPBACK) != 0)
            continue;

        int socketFd = socket(AF_INET, SOCK_DGRAM, 0);
        if (socketFd < 0)
            continue;

        struct ifreq request;
        memset(&request, 0, sizeof(request));
        strncpy(request.ifr_name, current->ifa_name, IFNAMSIZ - 1);

        if (ioctl(socketFd, SIOCGIFHWADDR, &request) == 0)
        {
            const unsigned char* address = reinterpret_cast<const unsigned char*>(request.ifr_hwaddr.sa_data);

            out->macAddresses.resize(out->macAddresses.size() + 1);
            for (size_t i = 0; i < MacAddress::kBytesInMacAddress; ++i)
                out->macAddresses.back().address[i] = address[i];

            success = true;
        }

        close(socketFd);
    }

    freeifaddrs(addresses);
    return success;
}
