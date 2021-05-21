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
        {"tetergent", recommendedWashingProgram.getDetergent()}
    };

    return responseBody.dump();
}


int WashingMachine::set(const string& name, const string& value) {
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


string WashingMachine::get() {
    // https://github.com/nlohmann/json#examples
    // First example
    cout << status << '\n';
    json responseBody = {
            {"status", status}
    };

    return responseBody.dump();
}


json WashingMachine::getEnvironment() {
    // https://github.com/nlohmann/json#examples
    // First example
    cout << status << '\n';
    json responseBody = {
            {"status", status},
            {"waterSupplyAvailable", waterSupplyAvailable},
            {"impurity", impurity},
            {"detergentCache", detergentCache}
    };

    return responseBody;
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
    return true;
}

WashingProgram* WashingMachine::getCustomWashingProgram(string programName) const{
    if(not WashingMachine::customWashingPrograms.count(programName)) {
        return nullptr;
    }
    return WashingMachine::customWashingPrograms.at(programName);
}




