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
}


void ServerEndpoint::setSettings(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
            .add<Http::Header::Server>("pistache/0.1")
            .add<Http::Header::ContentType>(MIME(Text, Plain));

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


void ServerEndpoint::scheduleProgram(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
            .add<Http::Header::Server>("pistache/0.1")
            .add<Http::Header::ContentType>(MIME(Text, Plain));

    try {
        json settingsValues = json::parse(request.body());

        // TODO (Marian) check if customProgram is valid before code below

        // Check if "scheduledTime" and "standardProgram" or "customProgram" (not both) are set
        if(settingsValues["scheduledTime"] == nullptr) {
            response.send(Http::Code::Bad_Request, "Schedule time is not set.");
            return;
        }

        if(settingsValues["standardProgram"] == nullptr && settingsValues["customProgram"] == nullptr) {
            response.send(Http::Code::Bad_Request, "No program is selected or created.");
            return;
        }

        if(settingsValues["standardProgram"] != nullptr && settingsValues["customProgram"] != nullptr) {
            response.send(Http::Code::Bad_Request, "Can only select predefined program or create a new program.");
            return;
        }

        WashingProgram customWashingProgram = *(new WashingProgram(0, 0, 0, 0));

        if (settingsValues["customProgram"] != nullptr) {

            if(settingsValues["customProgram"].is_string()) {
                string programName = settingsValues["customProgram"];
                
                if (washingMachine.getCustomWashingProgram(programName) != nullptr) {
                    customWashingProgram = *(washingMachine.getCustomWashingProgram(programName));
                } else {
                    response.send(Http::Code::Bad_Request, "Could not find custom program with the given name.");
                }

            } else if (settingsValues["customProgram"].is_object()) {
                if (settingsValues["customProgram"]["speed"] == nullptr ||
                    settingsValues["customProgram"]["temperature"] == nullptr ||
                    settingsValues["customProgram"]["time"] == nullptr ||
                    settingsValues["customProgram"]["detergent"] == nullptr ) {
                    response.send(Http::Code::Bad_Request, "At least one field of given custom program is missing or is spelled incorrectly");
                    return;
                
                } else {
                     WashingProgram customProgram(
                        settingsValues["customProgram"]["speed"],
                        settingsValues["customProgram"]["temperature"],
                        settingsValues["customProgram"]["time"],
                        settingsValues["customProgram"]["detergent"]
                    );

                    if (washingMachine.customProgramIsValid(customProgram))
                        customWashingProgram = customProgram;
                    else
                        // TODO (Bleo) change error message below so it contains the actual invalid parameters
                        response.send(Http::Code::Bad_Request, "Invalid parameters for custom washing program.");
                }
            } else {
                response.send(Http::Code::Bad_Request, "Custom program must be string or object.");
                return;
            }
        }

        // If schedule could not be set, use the code to write the reason for failure
        int code = washingMachine.setSchedule(settingsValues["scheduledTime"]);

        switch (code) {
            case -1: response.send(Http::Code::Bad_Request, "Current washing program is pending, still running or it is paused."); break;
            case -2: response.send(Http::Code::Bad_Request, "Scheduled time does not match the format \"dd.dd.dddd dd:dd\" where d is a digit."); break;
            case -3: response.send(Http::Code::Bad_Request, "Scheduled time has an invalid date or time."); break;
            case -4: response.send(Http::Code::Bad_Request, "Schedule must be set for a date in the future."); break;
        }

        // Set standard washing program, if key is given
        if(settingsValues["standardProgram"] != nullptr) {
            string programName = settingsValues["standardProgram"];

            // Check if the name of the washing program exists
            if(not WashingMachine::standardWashingPrograms.count(programName)) {
                response.send(Http::Code::Bad_Request, "Could not find standard program with the given name.");
                return;
            }

            washingMachine.setCurrentProgram(*WashingMachine::standardWashingPrograms[programName]);
        }

        if (customWashingProgram != *(new WashingProgram(0, 0, 0, 0))) {
            washingMachine.setCurrentProgram(customWashingProgram);
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

string ServerEndpoint::insertClothesMessage(json settingsValues) {
    if(settingsValues["clothesList"] == nullptr) {
        //response.send(Http::Code::Bad_Request, "Request must contain clothes list");
        return "Request must contain clothes list";
    } else if (!settingsValues["clothesList"].is_array()) {
        //response.send(Http::Code::Bad_Request, "Clothes list must be an array");
        return "Clothes list must be an array";
    }

    vector<vector<string>> list;

    for (json pair : settingsValues["clothesList"]) {
        if (!pair.is_array()) {
            //response.send(Http::Code::Bad_Request, "Elements inside clothes list must be arrays");
            return "Elements inside clothes list must be arrays";
        }

        if (pair.size() != 2) {
            //response.send(Http::Code::Bad_Request, "Arrays inside clothes list must contain only 2 elements");
            return "Arrays inside clothes list must contain exactly 2 elements";
        }

        json fabric = pair.at(0);
        json color = pair.at(1);

        if (!fabric.is_string() || !color.is_string()) {
            //response.send(Http::Code::Bad_Request, "Elements inside clothes list must contain only strings");
            return "Elements inside clothes list must contain only strings";
        }

        if (!washingMachine.fabricInList(fabric)) {
            //response.send(Http::Code::Bad_Request, "Fabric must be one of the following: Silk, Wool, Cotton, Leather, Velvet, Synthetic");
            return "Fabric must be one of the following: Silk, Wool, Cotton, Leather, Velvet, Synthetic";
        }

        vector<string> props;
        props.push_back(fabric);
        props.push_back(color);
        list.push_back(props);
    }

    if (washingMachine.getStatus() != WashingMachine::machineStatus[1] && washingMachine.getStatus() != WashingMachine::machineStatus[2] && washingMachine.getStatus() != WashingMachine::machineStatus[3]) {
        washingMachine.setClothes(list);
    }
    else {
        return "Can't change clothes while machine is working ):";
    }
    
    return "Clothes were successfully inserted.";
}

void ServerEndpoint::insertClothes(const Rest::Request& request, Http::ResponseWriter response) {
    response.headers()
        .add<Http::Header::Server>("pistache/0.1")
        .add<Http::Header::ContentType>(MIME(Text, Plain));

    try {
        json settingsValues = json::parse(request.body());

        string result = ServerEndpoint::insertClothesMessage(settingsValues);
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