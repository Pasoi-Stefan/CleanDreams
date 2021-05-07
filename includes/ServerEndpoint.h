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

public:
    // Constructor
    explicit ServerEndpoint(Address addr)
            : httpEndpoint(make_shared<Http::Endpoint>(addr)) { }

    // Initialization of the server. Additional options can be provided here
    void init();

    // Start server
    void start();

    // Wait for signal for server to shut down
    void waitForSignal();
};