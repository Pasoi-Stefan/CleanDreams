#include "ServerEndpoint.h"

void ServerEndpoint::setupRoutes() {
    // Defining various endpoints
    // Generally say that when http://localhost:9080/ready is called, the handleReady function should be called
    Rest::Routes::Post(router, "/settings",
                       Rest::Routes::bind(&ServerEndpoint::setSettings, this));
    Rest::Routes::Get(router, "/settings",
                      Rest::Routes::bind(&ServerEndpoint::getSettings, this));
}


void ServerEndpoint::setSettings(const Rest::Request& request, Http::ResponseWriter response) {
    // https://nlohmann.github.io/json/features/parsing/parse_exceptions/
    // Parse the request body as json and catch parsing error
    try {
        json settingsValues = json::parse(request.body());

        // https://nlohmann.github.io/json/features/iterators/
        // The for loop for processing each (key, value) pair from a json
        for (auto& [key, val] : settingsValues.items()) {
            // If val is not a string in json, and exception is thrown
            int setResponse = washingMachine.set(key, val);

            if(!setResponse) {
                response.send(Http::Code::Bad_Request, key + " was not found and or '" + to_string(val) + "' was not a valid value ");
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


void ServerEndpoint::getSettings(const Rest::Request& request, Http::ResponseWriter response) {
    string settings = washingMachine.get();

    response.headers()
            .add<Http::Header::Server>("pistache/0.1")
            .add<Http::Header::ContentType>(MIME(Application, Json));

    response.send(Http::Code::Ok, settings);
}