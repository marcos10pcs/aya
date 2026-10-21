#pragma once

#include "Item.hpp"
#include "Replicator.hpp"
#include "Utility/ProgramMemoryChecker.hpp"


namespace Aya
{
namespace Network
{

class Replicator::HashItem : public Item
{
    PmcHashContainer hashes;
    unsigned long long fuzzyToken;
    unsigned long long apiToken;
    unsigned long long prevApiToken;

public:
    HashItem(Replicator* replicator, const PmcHashContainer* const hashes, unsigned long long fuzzyToken, unsigned long long apiToken,
        unsigned long long prevApiToken);

    /*implement*/ virtual bool write(RakNet::BitStream& bitStream);
};

} // namespace Network
} // namespace Aya
