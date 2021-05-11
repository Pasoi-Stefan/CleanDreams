#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include "WashingMachine.h"

using namespace Pistache;

// Class with server endpoint
class ServerEndpoint {
private:
    // Defining the httpEndpoint and a router
    shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;

    // Set instance of washing machine
    WashingMachine washingMachine;

    void setupRoutes();

    // Configure a setting
    void setSettings(const Rest::Request& request, Http::ResponseWriter response);

    // Get all settings
    void getSettings(const Rest::Request& request, Http::ResponseWriter response);

    // Schedule a washing program (NOT IMPLEMENTED)
    void scheduleProgram(const Rest::Request& request, Http::ResponseWriter response);

public:
    // Constructor
    explicit ServerEndpoint(Address addr)
            : httpEndpoint(make_shared<Http::Endpoint>(addr)) { }

    // Initialization of the server. Additional options can be provided here
    void init() {
        // https://github.com/pistacheio/pistache/issues/441#issuecomment-730102957
        // Seems to solve the issue with "Address already in use"

        // Set options before initialising endpoint
        auto opts = Http::Endpoint::options()
                .flags(Tcp::Options::ReuseAddr);

        httpEndpoint->init(opts);

        // Server routes are loaded up
        setupRoutes();
    }

    // Start server
    void start() {
        httpEndpoint->setHandler(router.handler());
    }

    // Wait for signal for server to shut down
    void waitForSignal() {
        httpEndpoint->serve();
    }
};