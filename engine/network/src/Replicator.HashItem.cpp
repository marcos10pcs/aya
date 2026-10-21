#include "Replicator.HashItem.hpp"

#include "Item.hpp"
#include "Replicator.hpp"
#include "ReplicatorStats.hpp"

#include "FastLog.hpp"
#include "DataModel/DataModel.hpp"


#include "RakNet/BitStream.hpp"
#include "Utility/ProgramMemoryChecker.hpp"

#include "DataModel/HackDefines.hpp"

#include "Security/FuzzyTokens.hpp"
#include "Security/ApiSecurity.hpp"
#include "DataModel/HackDefines.hpp"




namespace Aya
{
namespace Network
{

Replicator::HashItem::HashItem(Replicator* replicator, const PmcHashContainer* const hashes, unsigned long long fuzzyToken,
    unsigned long long apiToken, unsigned long long prevApiToken)
    : Item(*replicator)
    , hashes(*hashes)
    , fuzzyToken(fuzzyToken)
    , apiToken(apiToken)
    , prevApiToken(prevApiToken)
{
#if defined(_WIN32) && !defined(RBX_PLATFORM_DURANGO)
    this->hashes.hash.push_back(Aya::Security::getIndirectly<LINE_RAND4>((void*)(&Aya::Security::rbxTextSize)));
    this->hashes.hash.push_back(Aya::Security::getIndirectly<LINE_RAND4>((void*)(&Aya::Security::rbxTextBase)));
#endif
}

// make sure to _copy_ data to the item and not re-read it from other memory.
bool Replicator::HashItem::write(RakNet::BitStream& bitStream)
{
    // There is a small amount of obscuring that is done to make this slightly harder to
    // analyze.  "nonce" = "number used once"
    writeItemType(bitStream, ItemTypeHash);
    unsigned char numItems;
    numItems = (unsigned char)hashes.hash.size();
    bitStream << numItems;
    unsigned int obscuringXorValue = hashes.nonce ^ hashes.hash[numItems - 1];
    bitStream << obscuringXorValue;
    for (unsigned char i = 0; i < numItems - 1; ++i)
    {
        obscuringXorValue ^= hashes.hash[i];
        bitStream << obscuringXorValue;
    }
    bitStream << hashes.hash[numItems - 1];
    bitStream << fuzzyToken;
    bitStream << apiToken;
    bitStream << prevApiToken;
    
    return true;
}

} // namespace Network
} // namespace Aya
