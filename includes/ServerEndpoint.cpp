#include "ServerEndpoint.h"

void ServerEndpoint::setupRoutes() {
    // Defining various endpoints
    // Generally say that when http://localhost:9080/ready is called, the handleReady function should be called
    Rest::Routes::Post(router, "/settings",
                       Rest::Routes::bind(&ServerEndpoint::setSettings, this));
    Rest::Routes::Get(router, "/settings",
                      Rest::Routes::bind(&ServerEndpoint::getSettings, this));
    Rest::Routes::Post(router, "/program",
                      Rest::Routes::bind(&ServerEndpoint::scheduleProgram, this));
    Rest::Routes::Get(router, "/program",
                       Rest::Routes::bind(&ServerEndpoint::getStatus, this));
    Rest::Routes::Post(router, "/clothes",
                      Rest::Routes::bind(&ServerEndpoint::insertClothes, this));
    Rest::Routes::Get(router, "/clothes",
                       Rest::Routes::bind(&ServerEndpoint::giveRecommendations, this));
    Rest::Routes::Post(router, "/environment",
                       Rest::Routes::bind(&ServerEndpoint::setEnvironment, this));
    Rest::Routes::Get(router, "/environment",
                      Rest::Routes::bind(&ServerEndpoint::getEnvironment, this));
}


void ServerEndpoint::setSettings(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
            .add<Http::Header::Server>("pistache/0.1")
            .add<Http::Header::ContentType>(MIME(Text, Plain));

    // https://nlohmann.github.io/json/features/parsing/parse_exceptions/
    // Parse the request body as json and catch parsing error
    try {
        json settingsValues = json::parse(request.body());
        
        string result = washingMachine.setSettingsMessage(settingsValues);

        if (result != "Settings updated successfully.") {
            response.send(Http::Code::Bad_Request, result);
            return;
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


void ServerEndpoint::setEnvironment(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
            .add<Http::Header::Server>("pistache/0.1")
            .add<Http::Header::ContentType>(MIME(Text, Plain));

    try {
        json environment = json::parse(request.body());
        string result = washingMachine.setEnvironmentMessage(environment);
        if (result != "Environment updated successfully.") {
            response.send(Http::Code::Bad_Request, result);
            return;
        }
    }
    catch (json::parse_error& e) {
        response.send(Http::Code::Bad_Request, e.what());
        return;
    }

    response.send(Http::Code::Ok, "Environment updated successfully.");
}


void ServerEndpoint::getEnvironment(const Rest::Request& request, Http::ResponseWriter response) {
    json environment = washingMachine.getEnvironment();
    response.headers()
            .add<Http::Header::Server>("pistache/0.1")
            .add<Http::Header::ContentType>(MIME(Application, Json));

    string result = "";

    if(bool(environment["waterSupplyAvailable"])){
        result += "Water supply available;\n";
    } else {
        result += "Water supply not available;\n";
    }

    if(double(environment["impurity"]) > 5 ){
        result += "Water impurity too high;\n";
    } else {
        result += "Water impurity is low enough;\n";
    }

    result += "Detergent quantity: ";
    if (environment["detergentCache"] == 0.0) {
        result += "empty";
    } else {
        result += to_string(environment["detergentCache"]);
    }
    response.send(Http::Code::Ok, result);
}


void ServerEndpoint::scheduleProgram(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
            .add<Http::Header::Server>("pistache/0.1")
            .add<Http::Header::ContentType>(MIME(Text, Plain));

    try {
        json settingsValues = json::parse(request.body());

        string result = washingMachine.scheduleProgramMessage(settingsValues);

        if (result != "Program scheduled successfully.") {
            response.send(Http::Code::Bad_Request, result);
            return;
        }

    }
    catch (json::parse_error& e) {
        response.send(Http::Code::Bad_Request, e.what());
        return;
    }

    response.send(Http::Code::Ok, "Program scheduled successfully.");
}

void ServerEndpoint::getStatus(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
        .add<Http::Header::Server>("pistache/0.1")
        .add<Http::Header::ContentType>(MIME(Application, Json));

    string settings = washingMachine.getScheduleAndProgram();
    response.send(Http::Code::Ok, settings);
}

void ServerEndpoint::insertClothes(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
        .add<Http::Header::Server>("pistache/0.1")
        .add<Http::Header::ContentType>(MIME(Text, Plain));

    try {
        json settingsValues = json::parse(request.body());

        string result = washingMachine.insertClothesMessage(settingsValues);
        if (result != "Clothes were successfully inserted.") {
            response.send(Http::Code::Bad_Request, result);
            return;
        }

    } catch (json::parse_error& e) {
        response.send(Http::Code::Bad_Request, e.what());
        return;
    }

    response.send(Http::Code::Ok, "Clothes were successfully inserted.");
}

void ServerEndpoint::giveRecommendations(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
        .add<Http::Header::Server>("pistache/0.1")
        .add<Http::Header::ContentType>(MIME(Application, Json));

    string washingProgram = washingMachine.getRecommendedWashingProgram();

    response.send(Http::Code::Ok, washingProgram);
}