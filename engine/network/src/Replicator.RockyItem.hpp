#pragma once

#include "Item.hpp"
#include "Replicator.hpp"
#include "boost/cstdint.hpp"
#include "Network/NetPmc.hpp"
#include "Security/ApiSecurity.hpp"

namespace Aya
{
namespace Network
{

class ClientReplicator;

struct MccReport
{
    uint32_t memcheckRunTime;
    uint32_t memcheckDoneTime;
    uint32_t mccRunTime;
    uint32_t badAppRunTime;
    uint32_t localChecksEncoded;
};

enum RockySubtype
{
    RockeyMccReportClient = 0,
    RockeyNetPmcChallenge,
    RockeyNetPmcResponse,
    RockeyCallInfo
};

class Replicator::RockyItem : public Item
{
    MccReport report;

public:
    RockyItem(Replicator* replicator, MccReport&);

    /*implement*/ virtual bool write(RakNet::BitStream& bitStream);

    static shared_ptr<DeserializedItem> read(Replicator& replicator, RakNet::BitStream& bitStream);
};

// currently just PmcChallenges
class DeserializedRockyItem : public DeserializedItem
{
public:
    uint8_t idx;
    Aya::Security::NetPmcChallenge challenge;

    DeserializedRockyItem()
    {
        type = Item::ItemTypeRocky;
    }
    ~DeserializedRockyItem() {}

    /*implement*/ void process(Replicator& replicator);
};


#ifndef AYA_STUDIO
class Replicator::NetPmcResponseItem : public Item
{
    uint32_t response;
    uint64_t correct;
    uint8_t idx;

public:
    NetPmcResponseItem(Replicator* replicator, uint32_t response, uint64_t correct, uint8_t idx);

    /*implement*/ virtual bool write(RakNet::BitStream& bitStream);
};

class Replicator::RockyDbgItem : public Item
{
    std::vector<CallChainInfo> info;

public:
    RockyDbgItem(Replicator* replicator, const std::vector<CallChainInfo>& info);

    /*implement*/ virtual bool write(RakNet::BitStream& bitStream);
};
#endif

#ifdef AYA_SERVER
class Replicator::NetPmcChallengeItem : public Item
{
    uint8_t idx;

public:
    NetPmcChallengeItem(Replicator* replicator, uint8_t idx);

    /*implement*/ virtual bool write(RakNet::BitStream& bitStream);
};

#endif

} // namespace Network
} // namespace Aya
