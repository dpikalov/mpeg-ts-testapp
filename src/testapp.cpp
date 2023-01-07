#include "tsduck.h"
#include "handler.h"

const ts::UString kInputStreamIpPort = u"239.0.0.1:1234";

// Enforce COM and network init on Windows, transparent elsewhere.
TS_MAIN(TestApp);

int TestApp(int argc, char* argv[])
{
    // Use an asynchronous logger to report errors, logs, debug, etc.
    ts::AsyncReport report;

    // The TS processing is performed into this object.
    ts::TSProcessor tsproc(report);

    Handler handler(report);
    tsproc.registerEventHandler(&handler, ts::PluginType::OUTPUT);

    // Build tsp options. Accept most default values, except a few ones.
    ts::TSProcessorArgs opt;
    opt.app_name = u"testapp";  // for error messages only.

    // Input plugin. Here, read an IP multicast stream.
    opt.input = {u"ip", { kInputStreamIpPort }};

    // Output plugin.
    opt.output = {u"memory", {}};

    // Start the TS processing.
    if (!tsproc.start(opt)) {
        return EXIT_FAILURE;
    }

    // And wait for TS processing termination.
    tsproc.waitForTermination();
    return EXIT_SUCCESS;
}
