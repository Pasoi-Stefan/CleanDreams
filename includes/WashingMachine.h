#include <iostream>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <regex>

#include "WashingProgram.h"

using namespace std;
using namespace nlohmann;

// Class with settings for washing machine
class WashingMachine {
private:
    string status;                                                   //  Current status of washing machine
    double detergentCache;                                           //  Total quantity of detergent available for washing programs
    double impurity;                                                 //  Percent of water impurity (give warning when impurity exceeds limit)
    double elapsedTimeOnPause;                                       //  Elapsed time of washing program when paused
    WashingProgram currentProgram;                                   //  Washing program currently being processed
    pair<double, double> temperatureInterval;                        //  Current temperature interval of supplied water
    pair<string, string> clothes;                                    //  Clothes currently loaded. Each clothing accessory has color and material it is made from
    time_t scheduledTime;                                            //  Date and time when washing program is started

public:
    static vector<string> clothingMaterials;                         //  All possible clothing materials a washing machine can receive
    static vector<string> machineStatus;                             //  All possible statuses of a washing machine
    static map<string, WashingProgram*> standardWashingPrograms;     //  Standard washing programs available to the user

    // Constructor
    explicit WashingMachine() {
        // Machine is initially turned off
        status = machineStatus[0];

        // Set a dummy value for current washing program
        currentProgram = *(new WashingProgram(0,0,0,0));

        // Set a dummy value for scheduled time
        scheduledTime = 0;
    }

    void setCurrentProgram(const WashingProgram &currentProgram);

    // Change value for one of the settings
    int set(const string& name, const string& value);

    // Get settings
    string get();

    // Check if date and time are valid in struct
    bool dateAndTimeIsValid(struct tm tm);

    // Update the status of the machine
    optional<int> updateStatus();

    // Schedule washing program
    int setSchedule(string schedule);

    // Get a string representing a json containing the scheduled time and the washing program
    string getScheduleAndProgram();
};