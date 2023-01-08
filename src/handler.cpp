#include "handler.h"

using namespace std;

Handler::Handler(ts::Report& report) :
    _report(report)
{
}

void Handler::handlePluginEvent(const ts::PluginEventContext& context)
{
    ts::PluginEventData* data =
        dynamic_cast<ts::PluginEventData*>( context.pluginData() );

    if (data == nullptr) {
        return;
    }

    const ts::TSPacket* packets =
        reinterpret_cast<const ts::TSPacket*>( data->data() );

    const size_t packetsCount = data->size() / ts::PKT_SIZE;

    for (size_t i = 0; i < packetsCount; i++) {
        handlePacket( packets[i] );
    } 
}

void Handler::handlePacket(const ts::TSPacket& packet)
{
    auto pid = packet.getPID();
    auto& pidCtx = _pidContextMap[ pid ];

    pidCtx.packetsCounter++;

    // Detect PES stream
    // https://en.wikipedia.org/wiki/Packetized_elementary_stream
    if (packet.getPESHeaderSize()) {
      pidCtx.pesStreamId = packet.getPayload()[3];
    }

    // Count video packets (PES stream-id is in range 0xE0-0xEF)
    if ((pidCtx.pesStreamId & 0xe0) == 0xe0) {
        _vidPacketsCounter++;
    }

    if ( packet.hasPCR() ) {
        _pcrPacketsCounter++;
    }

    // Recalculate bitrate
    if ( packet.hasPCR() ) { 
        auto pcr = packet.getPCR();
        // TSDuck return PCR based on a 27MHz clock
        // https://github.com/tsduck/tsduck/blob/master/src/libtsduck/dtv/transport/tsTSPacket.cpp#L422
        double deltaTime = (pcr - pidCtx.pcr) / 27.e6;
        double deltaBits = pidCtx.packetsCounter * ts::PKT_SIZE_BITS;
        double bitrate   = deltaBits / deltaTime;

        pidCtx.pcr = pcr;
        pidCtx.packetsCounter = 0;
        pidCtx.bitrate = bitrate;
    }

    printStatus();
}

void Handler::printStatus() {
    static time_t lastPrintTime = 0;

    // print at most once per second
    if (lastPrintTime == time(0)) return; else lastPrintTime = time(0);

    // clear terminal
    _report.info(u"\x1B[2J\x1B[H");

    // get total bitrate
    double totalBitrate = 0;
    for (auto const& item : _pidContextMap) {
        totalBitrate += item.second.bitrate;
    }

    _report.info(u"Video packets: %d", { _vidPacketsCounter });
    _report.info(u"PCR packets  : %d", { _pcrPacketsCounter });
    _report.info(u"Total bitrate: %.3f", { totalBitrate / 1000 });

    for (auto const& item : _pidContextMap) {
        // don't print items with unknown bitrate
        if (item.second.bitrate != 0.0)
            _report.info(u"pid: %d pesStreamId: 0x%x, bitrate: %.3f kb/s", {
                item.first, item.second.pesStreamId, item.second.bitrate / 1000
        });
    }
}
