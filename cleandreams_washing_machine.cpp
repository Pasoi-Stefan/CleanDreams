#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>

#include <exception>

using namespace std;
using namespace Pistache;

// Class with settings for washing machine
class WashingMachine {
private:
    // The machine's status can be 0 (off) or 1 (on)
    int8_t status;

public:
    explicit WashingMachine() {
        // Machine is initially turned off
        status = 0;
    }

    // Change value for one of the settings
    int set(const string& name, const string& value) {
        try {
            if (name == "status") {
                status = static_cast<int8_t>(stoi(value));
                return 1;
            }

            return 0;
        }
        catch (exception& e) {
            cout << e.what() << '\n';
            return 0;
        }
    }

    // Get settings
    string get(const string& name) {
        if (name == "status")
            return to_string(status);

        return "";
    }
};

// Class with server endpoint
class ServerEndpoint {
private:
    // Defining the httpEndpoint and a router
    shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;

    // Set instance of washing machine
    WashingMachine washingMachine;

public:
    explicit ServerEndpoint(Address addr)
        : httpEndpoint(make_shared<Http::Endpoint>(addr)) { }

private:
    void setupRoutes() {
        // Defining various endpoints
        // Generally say that when http://localhost:9080/ready is called, the handleReady function should be called
        Rest::Routes::Post(router, "/settings/:settingName/:value",
                           Rest::Routes::bind(&ServerEndpoint::setSetting, this));
        Rest::Routes::Get(router, "/settings/:settingName/",
                          Rest::Routes::bind(&ServerEndpoint::getSetting, this));
    }

    // You don't know what the parameter content that you receive is, but you should
    // try to cast it to some data structure (ex: request.param(":settingName").as<std::string>())

    // Configure a setting
    void setSetting(const Rest::Request& request, Http::ResponseWriter response) {
        auto settingName = request.param(":settingName").as<std::string>();
        string val = "";

        if (request.hasParam(":value"))
            val = request.param(":value").as<string>();

        // Set new value to the specified setting
        int setResponse = washingMachine.set(settingName, val);

        // Sending some confirmation or error response
        if (setResponse == 1)
            response.send(Http::Code::Ok, settingName + " was set to " + val);
        else
            response.send(Http::Code::Not_Found, settingName + " was not found and or '" + val + "' was not a valid value ");
    }

    // Get a setting
    void getSetting(const Rest::Request& request, Http::ResponseWriter response) {
        auto settingName = request.param(":settingName").as<string>();
        string valueSetting = washingMachine.get(settingName);

        if (valueSetting != "") {
            // In this response I also add a couple of headers, describing the server that sent this response
            // and the way the content is formatted
            response.headers()
                    .add<Http::Header::Server>("pistache/0.1")
                    .add<Http::Header::ContentType>(MIME(Text, Plain));

            response.send(Http::Code::Ok, settingName + " is " + valueSetting);
        }
        else
            response.send(Http::Code::Not_Found, settingName + " was not found");
    }

public:
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

int main(int argc, char *argv[]) {
    // https://github.com/pistacheio/pistache/issues/242#issuecomment-494441391
    // It appears Pistache is natively multi-threaded

    // Set a port on which your server to communicate
    Port port(9080);

    Address addr(Ipv4::any(), port);

    // Instance of the class that defines what the server can do
    ServerEndpoint stats(addr);

    // Initialize and start the server
    stats.init();
    stats.start();

    cout << "Server started" << '\n';
    cout << "Host: " << addr.host() << '\n';
    cout << "Port: " << addr.port() << '\n';

    // Wait for user to stop the server
    stats.waitForSignal();
}