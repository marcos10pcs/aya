#include "Replicator.RockyItem.hpp"

#include "Item.hpp"

#include "Network/NetPmc.hpp"
#include "ClientReplicator.hpp"
#include "DataModel/DataModel.hpp"

#include <boost/bind.hpp>

namespace Aya
{
namespace Network
{

Replicator::RockyItem::RockyItem(Replicator* replicator, MccReport& report)
    : Item(*replicator)
    , report(report)
{
}

bool Replicator::RockyItem::write(RakNet::BitStream& bitStream)
{
    writeItemType(bitStream, ItemTypeRocky);
    bitStream << static_cast<unsigned char>(RockeyMccReportClient);
    bitStream << report.memcheckRunTime;
    bitStream << report.memcheckDoneTime;
    bitStream << report.mccRunTime;
    bitStream << report.badAppRunTime;
    bitStream << report.localChecksEncoded;
    return true;
}

#ifndef AYA_STUDIO
Replicator::NetPmcResponseItem::NetPmcResponseItem(Replicator* replicator, uint32_t response, uint64_t correct, uint8_t idx)
    : Item(*replicator)
    , response(response)
    , correct(correct)
    , idx(idx)
{
}

#if !defined(__linux) && !defined(__APPLE__)
bool Replicator::NetPmcResponseItem::write(RakNet::BitStream& bitStream)
{
    writeItemType(bitStream, ItemTypeRocky);
    bitStream << static_cast<unsigned char>(RockeyNetPmcResponse);
    bitStream << idx;
    bitStream << response;
    bitStream << correct;
    return true;
}
#else
bool Replicator::NetPmcResponseItem::write(RakNet::BitStream& bitStream)
{
    writeItemType(bitStream, ItemTypeRocky);
    bitStream << static_cast<unsigned char>(RockeyNetPmcResponse);
    bitStream << idx;
    bitStream << response;

    // Assuming correct is uint64_t and there's no direct overload
    // We need to serialize it manually
    // Check if RakNet::BitStream has a Write(uint64_t) function or similar
    bitStream.Write(correct);

    return true;
}
#endif

Replicator::RockyDbgItem::RockyDbgItem(Replicator* replicator, const std::vector<CallChainInfo>& info)
    : Item(*replicator)
    , info(info)
{
}

bool Replicator::RockyDbgItem::write(RakNet::BitStream& bitStream)
{
    writeItemType(bitStream, ItemTypeRocky);
    bitStream << static_cast<unsigned char>(RockeyCallInfo);
    bitStream << static_cast<uint8_t>(info.size());
    for (size_t i = 0; i < static_cast<uint8_t>(info.size()); ++i)
    {
        bitStream << info[i].handler;
        bitStream << info[i].ret;
    }
    return true;
}
#endif

#ifdef AYA_SERVER
Replicator::NetPmcChallengeItem::NetPmcChallengeItem(Replicator* replicator, uint8_t idx)
    : Item(*replicator)
    , idx(idx)
{
}

bool Replicator::NetPmcChallengeItem::write(RakNet::BitStream& bitStream)
{
    writeItemType(bitStream, ItemTypeRocky);
    bitStream << static_cast<unsigned char>(RockeyNetPmcChallenge);
    bitStream << idx;
    const Aya::Security::NetPmcChallenge& key = Aya::Security::netPmcKeys[idx];
    bitStream << key.base;
    bitStream << key.size;
    bitStream << key.seed;
#if !defined(__linux) && !defined(__APPLE__)
    bitStream << key.result;
#else
    bitStream << static_cast<const unsigned long long>(key.result);
#endif
    return true;
}
#endif


void DeserializedRockyItem::process(Replicator& replicator)
{
#if defined(_WIN32) && !defined(AYA_STUDIO)
    ClientReplicator* rep = rbx_static_cast<ClientReplicator*>(&replicator);
    DataModel::get(rep)->submitTask(boost::bind(&ClientReplicator::doNetPmcCheck, shared_from(rep), idx, challenge), DataModelJob::Write);
#endif
}

shared_ptr<DeserializedItem> Replicator::RockyItem::read(Replicator& replicator, RakNet::BitStream& bitStream)
{
    shared_ptr<DeserializedRockyItem> deserializedData(new DeserializedRockyItem());

    ClientReplicator* rep = rbx_static_cast<ClientReplicator*>(&replicator);
    rep->readRockyItem(bitStream, deserializedData->idx, deserializedData->challenge);

    return deserializedData;
}

} // namespace Network
} // namespace Aya