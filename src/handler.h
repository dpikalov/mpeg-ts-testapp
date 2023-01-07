#pragma once

#include "tsduck.h"

class Handler: public ts::PluginEventHandlerInterface
{
public:
    Handler(ts::Report& report);

    void handlePluginEvent(const ts::PluginEventContext& context) override;

private:
    void handlePacket(const ts::TSPacket& packet);

    void printStatus();

private:
    ts::Report& _report;

    struct TPIDContext {
      // last known PCR
      uint64_t pcr = 0;
      // last known TS packet number
      uint64_t packetNumber = 0;
      // last known PES stream id
      uint8_t pesStreamId = 0;
      // bitrate calculated from PCR above
      double bitrate = 0.0;
    };

    std::map<ts::PID, TPIDContext> _pidCtxMap; 

    // counts the total number of packets
    uint64_t _allPacketsCounter = 0;

    // counts the total number of packets containing Video
    uint64_t _vidPacketsCounter = 0;

    // counts the number of packets containing PCR
    uint64_t _pcrPacketsCounter = 0;
};
