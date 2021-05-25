#include "WashingMachine.h"

vector<string> WashingMachine::clothingFabrics = {"Silk", "Wool", "Cotton", "Leather", "Velvet", "Synthetic"};
vector<string> WashingMachine::machineStatus = {"Standby", "Pending", "Running" , "Paused", "Finished" , "Cancelled"};
map<string, WashingProgram*> WashingMachine::standardWashingPrograms = {
    {"Quick", new WashingProgram(1800, 40, 30, 80)},
    {"Normal", new WashingProgram(1600, 60, 60, 140)},
    {"Synthetic", new WashingProgram(1500, 40, 60, 130)},
    {"Cotton", new WashingProgram(1800, 60, 60, 130)},
    {"DelicateWool", new WashingProgram(1400, 40, 90, 110)},
    {"DelicateLeather", new WashingProgram(1300, 30, 70, 130)},
    {"Anti-allergy", new WashingProgram(1600, 90, 60, 100)},
    {"Easy iron and anti-crease", new WashingProgram(1400, 60, 90, 150)},
    {"Sport", new WashingProgram(1700, 90, 120, 200)}

};


void WashingMachine::readCustomPrograms() {
    std::ifstream customProgramsIn(CUSTOM_PROGRAMS_FILE);
    string programName;
    while (customProgramsIn >> programName) {
        if (programName.size() < 1) {
            continue;
        }
        double _speed;
        double _temperature;
        int _time;
        double _detergent;
        customProgramsIn >> _speed >> _temperature >> _time >> _detergent;
        WashingMachine::customWashingPrograms.insert(make_pair(programName, new WashingProgram(_speed, _temperature, _time, _detergent)));
    }
    customProgramsIn.close();
}

void WashingMachine::setCurrentProgram(const WashingProgram &currentProgram) {
    WashingMachine::currentProgram = currentProgram;
}

int WashingMachine::findColorInClothes(string color) {
    int cnt = 0;
    for (auto pair : WashingMachine::clothes) {
        if (pair.at(1) == color) {
            cnt++;
        }
    }
    return cnt;
}

int WashingMachine::findFabricInClothes(string fabric) {
    int cnt = 0;
    for (auto pair : WashingMachine::clothes) {
        if (pair.at(0) == fabric) {
            cnt++;
        }
    }
    return cnt;
}

bool WashingMachine::fabricInList(string fabric) {
    for (auto part : WashingMachine::clothingFabrics) {
        if (fabric == part) {
            return true;
        }
    }
    return false;
}

void WashingMachine::setClothes(const vector<vector<string>> list){
    WashingMachine::clothes = list;
}

string WashingMachine::getRecommendedWashingProgram() {
    string recommendedProgramName;
    WashingProgram recommendedWashingProgram;
    json responseBody;

    if (WashingMachine::clothes.empty()) {
        return "No clothes were introduced";
    }

    int length = WashingMachine::clothes.size();

    if (WashingMachine::findColorInClothes("#FFFFFF") > 0 && length != WashingMachine::findColorInClothes("#FFFFFF")) {
        return "There are white clothes combined with coloured ones. It is recommended to separate them.";
    }

    //Ratios for each fabric
    float syntheticPart = (float) WashingMachine::findFabricInClothes("Synthetic") / length;
    float cottonPart = (float) WashingMachine::findFabricInClothes("Cotton") / length;
    float leatherPart = (float) WashingMachine::findFabricInClothes("Leather") / length;
    float velvetPart = (float) WashingMachine::findFabricInClothes("Velvet") / length;
    float woolPart = (float) WashingMachine::findFabricInClothes("Wool") / length;
    float silkPart = (float) WashingMachine::findFabricInClothes("Silk") / length;

    //Find best washing program based on the ratios above
    if ((leatherPart > 0 || velvetPart > 0) && (woolPart + silkPart <= leatherPart + velvetPart)){
        recommendedProgramName = "DelicateLeather";
    } else if ((woolPart > 0 || silkPart > 0) && (woolPart + silkPart > leatherPart + velvetPart)){
        recommendedProgramName = "DelicateWool";
    } else if (syntheticPart > 0.5){
        recommendedProgramName = "Synthetic";
    } else if (cottonPart > 0.5) {
        recommendedProgramName = "Cotton";
    } else {
        recommendedProgramName = "Normal";
    }

    recommendedWashingProgram = *standardWashingPrograms.at(recommendedProgramName);

    //create response with name and program
    responseBody["name"] = recommendedProgramName;
    responseBody["program"] = {
        {"speed", recommendedWashingProgram.getSpeed()},
        {"temperature", recommendedWashingProgram.getTemperature()},
        {"duration", recommendedWashingProgram.getTime()},
        {"detergent", recommendedWashingProgram.getDetergent()}
    };

    return responseBody.dump();
}


int WashingMachine::set(const string& name, const string& value) {
    updateStatus();

    try {
        if (name == "status") {
            if (value == "pause") {
                // can pause a program only if it is in a state of Running
                if (status == machineStatus[2]) {
                    // set the state to Paused
                    status = machineStatus[3];

                    // get time when pause started
                    pauseStartTime = time(nullptr);

                    return 1;
                }
                return 0;
            }
            else if (value == "unpause") {
                // can unpause a program only if it is in a state of Paused
                if (!waterSupplyAvailable || impurity > 5) {
                    return 2;
                }
                if (status == machineStatus[3]) {
                    // set the state to Running
                    status = machineStatus[2];

                    // get current time
                    time_t curentTime = time(nullptr);

                    elapsedTimeOnPause += curentTime - pauseStartTime;
                    pauseStartTime = 0.0;
                    return 1;
                }
                return 0;
            }
            else if (value == "cancel") {
                // can cancel a program only if it is in a state of Pending, Running or Paused
                if (status == machineStatus[1] || status == machineStatus[2] || status == machineStatus[3]) {
                    // set the state to Cancelled
                    status = machineStatus[5];
                    
                    scheduledTime = 0.0;
                    currentProgram = *(new WashingProgram(0, 0, 0, 0));
                    return 1;
                }
                return 0;
            }

            return 0;
        }

        return 0;
    }
    catch (exception& e) {
        cout << e.what() << '\n';
        return 0;
    }
}

string WashingMachine::setSettingsMessage(json settingsValues) {
    // https://nlohmann.github.io/json/features/iterators/
    // The for loop for processing each (key, value) pair from a json
    for (auto& [key, val] : settingsValues.items()) {
        // If val is not a string in json, and exception is thrown
        int setResponse = set(key, val);

        if(setResponse == 0) {
            return key + " was not found and or '" + to_string(val) + "' was not a valid value ";
        }
        if(setResponse == 2) {
            return "Can't unpause washing program while water supply is missing or impurity is too high";
        }

    } return "Settings updated successfully.";
}

string WashingMachine::setEnvironmentMessage(json environment) {
    updateStatus();

    double newImpurity = 0.0;
    bool newWaterSupply = true;
    double newDetergentCache = 0.0;
    int parameters = 0;
    //get impurity and waterSupply out of request
    for (auto& [key, val] : environment.items()) {
        if(key == "waterSupplyAvailable"){
            parameters++;
            if (val.is_boolean()) {
                newWaterSupply = val;
            } else {
                return "Water supply must be a boolean";
            }
        }

        if(key == "impurity"){
            parameters++;
            if (val.is_number()){
                newImpurity = val;
            } else {
                return "Water impurity must be a double";
            }
        }

        if(key == "detergentCache"){
            parameters++;
            if (val.is_number()) {
                newDetergentCache = val;
            } else {
                return "Detergent cache must be double";
            }
        }
    }
    //if both parameters were provided
    if(parameters == 3) {
        //if running and one of two conditions not met
        detergentCache = newDetergentCache;
        waterSupplyAvailable = newWaterSupply;
        impurity = newImpurity;
        if ((newWaterSupply == false || newImpurity > MAX_IMPURITY) && status == WashingMachine::machineStatus[2]) {
            set("status", "pause");
        }

        if ((newWaterSupply == false || newImpurity > MAX_IMPURITY) && status == WashingMachine::machineStatus[1]) {
            set("status", "cancel");
        }

        //if paused and both conditions met
        if ((newWaterSupply == true && newImpurity <= MAX_IMPURITY) && status == WashingMachine::machineStatus[3]) {
            set("status", "unpause");
        }
        return "Environment updated successfully.";
    }
    //if one of the parameters was not provided
    return "Water supply, impurity or detergent not provided";
}

string WashingMachine::getEnvironment() {
    // https://github.com/nlohmann/json#examples
    // First example

    json responseBody = {
            {"status", status},
            {"waterSupplyAvailable", waterSupplyAvailable},
            {"impurity", impurity},
            {"detergentCache", detergentCache}
    };

    return responseBody.dump();
}


bool WashingMachine::dateAndTimeIsValid(struct tm tm) {
    // Get parameters from struct
    int day = tm.tm_mday;
    int month = tm.tm_mon + 1;
    int year = tm.tm_year + 1900;
    int hour = tm.tm_hour;
    int minute = tm.tm_min;
    int second = tm.tm_sec;

    // Validate time
    if (not (0 <= hour && hour <= 23))
        return false;
    if (not (0 <= minute && minute <= 59))
        return false;
    if (not (0 <= second && second <= 59))

        // Validate date
        // http://www.zedwood.com/article/cpp-checkdate-function-date-validation - Gregorian validator
        if (not (1582 <= year))
            return false;
    if (not (1 <= month && month <= 12))
        return false;
    if (not (1 <= day && day <= 31))
        return false;
    if (day == 31 && (month == 2 || month == 4 || month == 6 || month == 9 || month == 11))
        return false;
    if (day == 30 && month == 2)
        return false;
    if (month == 2 && day == 29 && year % 4 != 0)
        return false;
    if (month == 2 && day == 29 && year % 400 == 0)
        return true;
    if (month == 2 && day == 29 && year % 100 == 0)
        return false;
    if (month == 2 && day == 29 && year % 4 == 0)
        return true;

    return true;
}

optional<int> WashingMachine::updateStatus() {
    // statuses Standby and Finished are updated manually
    if (status == WashingMachine::machineStatus[0] || status == WashingMachine::machineStatus[4] || status == WashingMachine::machineStatus[5])
        return optional<int>();

    // Get current date and time (number of seconds since 1st Jan 1970)
    time_t currentTime = time(nullptr);

    // Take the duration of the washing program and convert from minutes to seconds
    int durationOfProgram = currentProgram.getTime() * 60;

    // Update Pending status
    if (status == WashingMachine::machineStatus[1]) {
        // If machine is in the washing interval, then set status to Running
        if(scheduledTime <= currentTime && currentTime <= scheduledTime + durationOfProgram + elapsedTimeOnPause)
            status = WashingMachine::machineStatus[2];

        // If machine is past the washing interval, then set status to Finished
        if(currentTime > scheduledTime + durationOfProgram + elapsedTimeOnPause)
            status = WashingMachine::machineStatus[4];
    }

    // Update Running status
    if (status == WashingMachine::machineStatus[2]) {
        // If machine is past the washing interval, then set status to Finished
        if(currentTime > scheduledTime + durationOfProgram)
            status = WashingMachine::machineStatus[4];
    }

    // If current status is Running, calculate and return time remaining from duration of washing program (in minutes)
    if (status == WashingMachine::machineStatus[2]) {
        int timeRemaining = (scheduledTime + durationOfProgram + elapsedTimeOnPause) - currentTime;
        return max(timeRemaining / 60, 0);
    }

    // If current status is Paused, calculate and return time remaining from duration of washing program (in minutes)
    if (status == WashingMachine::machineStatus[3]) {
        int timeRemaining = (scheduledTime + durationOfProgram + elapsedTimeOnPause) - pauseStartTime;
        return max(timeRemaining / 60, 0);
    }

    return optional<int>();
}

int WashingMachine::setSchedule(string schedule) {
    updateStatus();

    // If a washing program is pending, running or paused, the schedule cannot be set
    if (status == WashingMachine::machineStatus[1] || status == WashingMachine::machineStatus[2]
        || status == WashingMachine::machineStatus[3])
        return -1;

    // The given string should respect the following format: "dd.dd.dddd dd:dd" where d is a digit
    regex format("([0-9]{2})\\.([0-9]{2})\\.([0-9]{4}) ([0-9]{2}):([0-9]{2})");

    // Get matches from regex
    smatch matches;
    regex_match(schedule, matches, format);

    // If there are no matches, the schedule cannot be set
    if (matches.empty())
        return -2;

    // Use matches to initialise a tm object
    tm dateAndTime;

    // https://www.cplusplus.com/reference/ctime/tm/
    // tm stores the actual month and year differently
    dateAndTime.tm_mday = stoi(matches[1]);
    dateAndTime.tm_mon = stoi(matches[2]) - 1;
    dateAndTime.tm_year = stoi(matches[3]) - 1900;
    dateAndTime.tm_hour = stoi(matches[4]);
    dateAndTime.tm_min = stoi(matches[5]);
    dateAndTime.tm_sec = 0;

    // https://stackoverflow.com/a/50995391/15445526
    // Disable daylight saving time. Solves the one hour difference problem
    dateAndTime.tm_isdst = -1;

    // If the date is not valid, the schedule cannot be set
    if(not dateAndTimeIsValid(dateAndTime))
        return -3;

    // Get current date and time (number of seconds since 1st Jan 1970)
    time_t currentTime = time(nullptr);

    // If the washing program is scheduled for any date in the past, the schedule cannot be set
    if (not (currentTime < mktime(&dateAndTime)))
        return -4;

    // Set schedule
    scheduledTime = mktime(&dateAndTime);
    elapsedTimeOnPause = 0.0;

    // Update status to pending
    status = WashingMachine::machineStatus[1];

    return 0;
}


string WashingMachine::getScheduleAndProgram() {
    optional<int> programTimeLeft = updateStatus();

    // https://stackoverflow.com/a/28977975/15445526
    // Convert put_time() result to a string
    stringstream textStream;
    textStream << put_time(localtime(&scheduledTime), "%d.%m.%Y %H:%M");

    json responseBody;

    responseBody["status"] = status;

    // If the scheduled time or current program are not set, then give their keys the 'null' value
    if (scheduledTime == 0)
        responseBody["scheduledTime"] = nullptr;
    else
        responseBody["scheduledTime"] = textStream.str();

    if (currentProgram == *(new WashingProgram(0, 0, 0, 0)))
        responseBody["program"] = nullptr;
    else
        responseBody["program"] = {
                {"speed", currentProgram.getSpeed()},
                {"temperature", currentProgram.getTemperature()},
                {"duration", currentProgram.getTime()}
        };

    // If program is running, then give the key of remaining time its value, else give it the 'null' value
    if (not programTimeLeft.has_value())
        responseBody["programTimeLeft"] = nullptr;
    else
        responseBody["programTimeLeft"] = "> " + to_string(programTimeLeft.value()) + " min";

    return responseBody.dump();
}

// TODO (Bleo) check each parameter is in an interval
bool WashingMachine::customProgramIsValid(WashingProgram washingProgram) {
    return washingProgram.isValid();
}

WashingProgram* WashingMachine::getCustomWashingProgram(string programName) const{
    if(not WashingMachine::customWashingPrograms.count(programName)) {
        return nullptr;
    }

    return WashingMachine::customWashingPrograms.at(programName);
}

bool WashingMachine::isAlreadyExistent(string programName) const{
    return (WashingMachine::customWashingPrograms.count(programName) > 0);
}

string WashingMachine::insertClothesMessage(json settingsValues) {
    updateStatus();

    if(settingsValues["clothesList"] == nullptr) {
        return "Request must contain clothes list";
    } else if (!settingsValues["clothesList"].is_array()) {
        return "Clothes list must be an array";
    }

    vector<vector<string>> list;

    for (json pair : settingsValues["clothesList"]) {
        if (!pair.is_array()) {
            return "Elements inside clothes list must be arrays";
        }

        if (pair.size() != 2) {
            return "Arrays inside clothes list must contain exactly 2 elements";
        }

        json fabric = pair.at(0);
        json color = pair.at(1);

        if (!fabric.is_string() || !color.is_string()) {
            return "Elements inside clothes list must contain only strings";
        }

        if (!fabricInList(fabric)) {
            return "Fabric must be one of the following: Silk, Wool, Cotton, Leather, Velvet, Synthetic";
        }

        regex format("#[0-9A-F]{6}");

        // Get matches from regex
        smatch matches;

        string colorString = color;

        regex_match(colorString, matches, format);

        // If there are no matches, the schedule cannot be set
        if (matches.empty()){
             return "Color is incorrect";
        }

        vector<string> props;
        props.push_back(fabric);
        props.push_back(color);
        list.push_back(props);
    }

    if (status != WashingMachine::machineStatus[1] && status != WashingMachine::machineStatus[2] && status != WashingMachine::machineStatus[3]) {
        setClothes(list);
    }
    else {
        return "Can't change clothes while machine is working ):";
    }
    
    return "Clothes were successfully inserted.";
}

string WashingMachine::scheduleProgramMessage(json settingsValues) {
    if(!waterSupplyAvailable || impurity > 5) {
        return "Can't schedule washing program without water supply or with high levels of impurity.";
    }

    if(settingsValues["scheduledTime"] == nullptr) {
        return "Schedule time is not set.";
    }

    if(settingsValues["standardProgram"] == nullptr && settingsValues["customProgram"] == nullptr) {
        return "No program is selected or created.";
    }

    if(settingsValues["standardProgram"] != nullptr && settingsValues["customProgram"] != nullptr) {
        return "Can only select predefined program or create a new program.";
    }

    WashingProgram customWashingProgram = *(new WashingProgram(0, 0, 0, 0));

    if (settingsValues["customProgram"] != nullptr) {

        if(settingsValues["customProgram"].is_string()) {
            string programName = settingsValues["customProgram"];
            
            if (getCustomWashingProgram(programName) != nullptr) {
                customWashingProgram = *(getCustomWashingProgram(programName));
            } else {
                return "Could not find custom program with the given name.";
            }

        } else if (settingsValues["customProgram"].is_object()) {
            if (settingsValues["customProgram"]["speed"] == nullptr ||
                settingsValues["customProgram"]["temperature"] == nullptr ||
                settingsValues["customProgram"]["time"] == nullptr ||
                settingsValues["customProgram"]["detergent"] == nullptr ) {
                return "At least one field of given custom program is missing or is spelled incorrectly";
            
            } else {
                WashingProgram customProgram(
                    settingsValues["customProgram"]["speed"],
                    settingsValues["customProgram"]["temperature"],
                    settingsValues["customProgram"]["time"],
                    settingsValues["customProgram"]["detergent"]
                );

                if (customProgramIsValid(customProgram))
                    customWashingProgram = customProgram;
                else{
                    return "Invalid parameters for custom washing program.";
                }
            }
        } else {
            return "Custom program must be string or object.";
        }
    }

    // Set standard washing program, if key is given
    if(settingsValues["standardProgram"] != nullptr) {
        string programName = settingsValues["standardProgram"];

        // Check if the name of the washing program exists
        if(not WashingMachine::standardWashingPrograms.count(programName)) {
            return "Could not find standard program with the given name.";
        }

        customWashingProgram = (*WashingMachine::standardWashingPrograms[programName]);
    }

    if (customWashingProgram.getDetergent() > detergentCache) {
        return "There is not enough detergent in cache for this washing program.";
    }

    // If schedule could not be set, use the code to write the reason for failure
    int code = setSchedule(settingsValues["scheduledTime"]);

    switch (code) {
        case -1: {
            return "Current washing program is pending, still running or it is paused.";
        }
        case -2:{
            return "Scheduled time does not match the format \"dd.dd.dddd dd:dd\" where d is a digit.";
        }
        case -3:{
            return "Scheduled time has an invalid date or time.";
        }
        case -4:{
            return "Schedule must be set for a date in the future.";
        }
    }

    detergentCache -= customWashingProgram.getDetergent();
    setCurrentProgram(customWashingProgram);


    return "Program scheduled successfully.";
}

void WashingMachine::saveCustomProgram(string programName, WashingProgram washingProgram) {
    std::ofstream customProgramsFile(CUSTOM_PROGRAMS_FILE, ios::app);
    customProgramsFile << programName << '\n';
    washingProgram.print(customProgramsFile);
    customProgramsFile.close();
}

void WashingMachine::addCustomProgram(string programName, WashingProgram washingProgram) {
    WashingMachine::customWashingPrograms
    .insert(make_pair(programName, new WashingProgram(
            washingProgram.getSpeed(),
            washingProgram.getTemperature(),
            washingProgram.getTime(),
            washingProgram.getDetergent())));
    saveCustomProgram(programName, washingProgram);
}

string WashingMachine::setCustomWashingProgram(json programValues) {
    string programName = programValues["programName"];
    if (isAlreadyExistent(programName)) {
        return "Program already exists.";
    }

    if (programValues["speed"] == nullptr ||
        programValues["temperature"] == nullptr ||
        programValues["time"] == nullptr ||
        programValues["detergent"] == nullptr ) {
        return "At least one field of given custom program is missing or is spelled incorrectly";
    }

    WashingProgram customProgram(
            programValues["speed"],
            programValues["temperature"],
            programValues["time"],
            programValues["detergent"]
    );

    if (customProgramIsValid(customProgram)) {
        addCustomProgram(programName, customProgram);
        return "Custom program added successfully!";
    } else{
        return "Invalid parameters for custom washing program.";
    }

}





