#include "ServerEndpoint.h"

void ServerEndpoint::setupRoutes() {
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
void ServerEndpoint::setSettings(const Rest::Request& request, Http::ResponseWriter response) {
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
void ServerEndpoint::getSettings(const Rest::Request& request, Http::ResponseWriter response) {
    string settings = washingMachine.get();

    response.headers()
            .add<Http::Header::Server>("pistache/0.1")
            .add<Http::Header::ContentType>(MIME(Application, Json));

    response.send(Http::Code::Ok, settings);
}


// Initialization of the server. Additional options can be provided here
void ServerEndpoint::init() {
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
void ServerEndpoint::start() {
    httpEndpoint->setHandler(router.handler());
}

// Wait for signal for server to shut down
void ServerEndpoint::waitForSignal() {
    httpEndpoint->serve();
}
