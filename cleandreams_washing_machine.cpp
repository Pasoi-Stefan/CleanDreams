#include <pistache/net.h>
#include <pistache/http.h>
#include <pistache/peer.h>
#include <pistache/router.h>
#include <pistache/endpoint.h>
#include <nlohmann/json.hpp>
#include <exception>

using namespace std;
using namespace Pistache;
using namespace nlohmann;

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
    string get() {
        // https://github.com/nlohmann/json#examples
        // First example
        json responseBody = {
                {"status", status}
        };

        return responseBody.dump();
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
        Rest::Routes::Post(router, "/settings",
                           Rest::Routes::bind(&ServerEndpoint::setSettings, this));
        Rest::Routes::Get(router, "/settings",
                          Rest::Routes::bind(&ServerEndpoint::getSettings, this));
    }

    // You don't know what the parameter content that you receive is, but you should
    // try to cast it to some data structure (ex: request.param(":settingName").as<std::string>())

    // Configure a setting
    void setSettings(const Rest::Request& request, Http::ResponseWriter response) {
        // https://nlohmann.github.io/json/features/parsing/parse_exceptions/
        // Parse the request body as json and catch parsing error
        try {
            json settingsValues = json::parse(request.body());

            // https://nlohmann.github.io/json/features/iterators/
            // The for loop for processing each (key, value) pair from a json
            for (auto& [key, val] : settingsValues.items()) {
                // If val is not a string in json, and exception is thrown
                int setResponse = washingMachine.set(key, val.dump());

                if(!setResponse) {
                    response.send(Http::Code::Bad_Request, key + " was not found and or '" + val.dump() + "' was not a valid value ");
                    return;
                }
            }
        }
        catch (json::parse_error& e) {
            response.send(Http::Code::Bad_Request, e.what());
            return;
        }

        response.send(Http::Code::Ok, "Settings updated successfully.");
    }

    // Get all settings
    void getSettings(const Rest::Request& request, Http::ResponseWriter response) {
        string settings = washingMachine.get();

        response.headers()
                .add<Http::Header::Server>("pistache/0.1")
                .add<Http::Header::ContentType>(MIME(Application, Json));

        response.send(Http::Code::Ok, settings);
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