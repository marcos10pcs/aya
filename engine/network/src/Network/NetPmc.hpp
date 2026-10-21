#pragma once
#include <stdint.h>
#include <vector>
#include "boost/thread/mutex.hpp"

#ifdef _MSC_VER
#define FORCEINLINE __forceinline
#else
#define FORCEINLINE inline
#endif

namespace Aya
{
namespace Network
{
class Replicator;
}
} // namespace Aya

namespace Aya
{
namespace Security
{

struct NetPmcChallenge
{
    // these will be encrypted on the game client
    uint32_t base;
    uint32_t size;
    uint32_t seed;
    // This will be encrypted in a different way.
    uint64_t result;

    NetPmcChallenge& operator^=(const NetPmcChallenge& rhs)
    {
        this->base ^= rhs.base;
        this->size ^= rhs.size;
        this->seed ^= rhs.seed;
        this->result ^= rhs.result;
        return *this;
    }
};

#if defined(_WIN32) || defined(__linux) || defined(__APPLE__)
const size_t kNumChallenges = 128;
extern const volatile NetPmcChallenge kChallenges[kNumChallenges];

#ifndef AYA_STUDIO

uint32_t netPmcHashCheck(const NetPmcChallenge& challenge);

#endif

void salsa20(uint8_t* message, uint64_t mlen, uint8_t key[32], uint64_t nonce);
// This will appear in the release patcher and in RCC, but not in studio or the final client.
FORCEINLINE std::vector<NetPmcChallenge> generateNetPmcKeys()
{
    std::vector<NetPmcChallenge> keys;
    keys.resize(kNumChallenges);
    uint8_t key[32];
    for (size_t i = 0; i < 32; ++i)
    {
        key[i] = (uint8_t)((i + 1) * (i ^ 0x55));
    }
    Aya::Security::salsa20(reinterpret_cast<uint8_t*>(keys.data()), sizeof(NetPmcChallenge) * kNumChallenges, key, 2015);
    return keys;
}

#ifdef AYA_SERVER
extern std::vector<NetPmcChallenge> netPmcKeys;

class NetPmcServer
{
    std::vector<uint8_t> challenges;
    uint8_t challengeIdx;

    std::vector<uint8_t> challengesInFlight;
    boost::mutex flightMutex;

    bool isGameCreator;
    bool gameHasManyPlayers;
    bool gameHasManyParts;
    bool userHasActivity;

    size_t challengesSent;
    size_t challengesRecv;

public:
    NetPmcServer();

    // the intent it to make it much harder to detect this mechanism by making it
    // only work in places that are actually games.
    bool canSendChallenge(const Aya::Network::Replicator* rep); /*non-const*/

    bool tooManyPending() const;

    uint8_t getRandomChallenge();

    bool sendChallenge(uint8_t idx);

    bool removeFromList(uint8_t idx);

    bool checkResult(uint8_t idx, uint32_t response, uint64_t correct) const;

    unsigned int generateDebugInfo(const Aya::Network::Replicator* rep, uint32_t& sent, uint32_t& recv, uint32_t& pending) const;
};
#endif

#endif

} // namespace Security
} // namespace Aya
