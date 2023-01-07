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
    _allPacketsCounter++;

    auto pid = packet.getPID();
    auto& pidCtx =_pidCtxMap[ pid ];

    // Detect PES stream
    // https://en.wikipedia.org/wiki/Packetized_elementary_stream
    if (packet.getPESHeaderSize()) {
      pidCtx.pesStreamId = packet.getPayload()[3];
    }

    // Count video packets (stream-id range 0xE0-0xEF)
    if ((pidCtx.pesStreamId & 0xe0) == 0xe0) {
        _vidPacketsCounter++;
    }

    if ( packet.hasPCR() ) {
        _pcrPacketsCounter++;
    }

    if ( packet.hasPCR() ) { 
        // recalculate bitrate
        auto pcr = packet.getPCR();
        double deltaPCR = pcr - pidCtx.pcr;
        // The first 33 bits are based on a 90 kHz clock. The last 9 bits are
        // based on a 27 MHz clock. https://en.wikipedia.org/wiki/MPEG_transport_stream#PCR
        double deltaTime= deltaPCR / 27.e6;// 9.e4;
        double deltaPkts= _allPacketsCounter - pidCtx.packetNumber;
        double bitrate  = deltaPkts * ts::PKT_SIZE_BITS / deltaTime;

        pidCtx.pcr = pcr;
        pidCtx.packetNumber = _allPacketsCounter;
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

    _report.info(u"video packets: %d, pcr packets: %d", {
        _vidPacketsCounter, _pcrPacketsCounter
    });

    for (auto const& item : _pidCtxMap) {
        // don't show PIDs with unknown bitrate
        if (item.second.bitrate == 0.0)
            continue;
        _report.info(u"pid: 0x%x pesStreamId: 0x%x, bitrate: %.3f MBits/sec", {
            item.first, item.second.pesStreamId, item.second.bitrate / 1.e6
        });
    }
}
