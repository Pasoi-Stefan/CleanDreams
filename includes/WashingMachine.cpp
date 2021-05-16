#include "WashingMachine.h"

vector<string> WashingMachine::clothingMaterials = {"Silk", "Wool", "Cotton", "Leather", "Linen", "Velvet", "Hemp"};
vector<string> WashingMachine::machineStatus = {"Standby", "Pending", "Running" , "Paused", "Finished"};
map<string, WashingProgram*> WashingMachine::standardWashingPrograms = {
    {"Fast", new WashingProgram(700, 40, 30, 100)},
    {"Normal", new WashingProgram(500, 50, 60, 150)},
    {"Long", new WashingProgram(600, 60, 120, 200)}
};


void WashingMachine::setCurrentProgram(const WashingProgram &currentProgram) {
    WashingMachine::currentProgram = currentProgram;
}


int WashingMachine::set(const string& name, const string& value) {
    try {
        if (name == "status") {
            status = value;
            return 1;
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
    if (status == WashingMachine::machineStatus[0] || status == WashingMachine::machineStatus[4])
        return optional<int>();

    // Get current date and time (number of seconds since 1st Jan 1970)
    time_t currentTime = time(nullptr);

    // Take the duration of the washing program and convert from minutes to seconds
    int durationOfProgram = currentProgram.getTime() * 60;

    // Update Pending status
    if (status == WashingMachine::machineStatus[1]) {
        // If machine is in the washing interval, then set status to Running
        if(scheduledTime <= currentTime && currentTime <= scheduledTime + durationOfProgram)
            status = WashingMachine::machineStatus[2];

        // If machine is past the washing interval, then set status to Finished
        if(currentTime > scheduledTime + durationOfProgram)
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
        int timeRemaining = (scheduledTime + durationOfProgram) - currentTime;
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




