#include "Utility/MemoryStats.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <unistd.h>

namespace Aya
{
namespace MemoryStats
{

static bool readMemInfoValue(const char* key, unsigned long long& out)
{
    FILE* file = fopen("/proc/meminfo", "r");
    if (!file)
        return false;

    bool found = false;
    char line[256];
    size_t keyLength = strlen(key);

    while (fgets(line, sizeof(line), file))
    {
        if (strncmp(line, key, keyLength) != 0)
            continue;

        const char* value = line + keyLength;
        while (*value == ' ' || *value == '\t' || *value == ':')
            value++;

        out = strtoull(value, NULL, 10);
        found = true;
        break;
    }

    fclose(file);
    return found;
}

memsize_t usedMemoryBytes()
{
    // Resident set size of the current process, in bytes.
    FILE* file = fopen("/proc/self/statm", "r");
    if (!file)
        return 0;

    unsigned long long totalPages = 0;
    unsigned long long residentPages = 0;
    int matched = fscanf(file, "%llu %llu", &totalPages, &residentPages);
    fclose(file);

    if (matched != 2)
        return 0;

    long pageSize = sysconf(_SC_PAGESIZE);
    if (pageSize <= 0)
        pageSize = 4096;

    return static_cast<memsize_t>(residentPages) * static_cast<memsize_t>(pageSize);
}

memsize_t freeMemoryBytes()
{
    unsigned long long available = 0;
    if (readMemInfoValue("MemAvailable", available))
        return static_cast<memsize_t>(available) * 1024;

    unsigned long long free = 0;
    if (readMemInfoValue("MemFree", free))
        return static_cast<memsize_t>(free) * 1024;

    return 0;
}

memsize_t totalMemoryBytes()
{
    unsigned long long total = 0;
    if (readMemInfoValue("MemTotal", total))
        return static_cast<memsize_t>(total) * 1024;

    return 0;
}

} // namespace MemoryStats
} // namespace Aya
