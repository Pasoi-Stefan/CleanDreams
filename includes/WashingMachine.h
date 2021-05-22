#include <iostream>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <regex>
#include <vector>

#include "WashingProgram.h"

using namespace std;
using namespace nlohmann;

// Class with settings for washing machine
class WashingMachine {
private:
    string status;                                                   //  Current status of washing machine
    double detergentCache;                                           //  Total quantity of detergent available for washing programs
    double impurity;                                                 //  Percent of water impurity (give warning when impurity exceeds limit)
    bool waterSupplyAvailable;                                       //  If water supply is available or not
    double elapsedTimeOnPause;                                       //  Elapsed time of washing program when paused
    WashingProgram currentProgram;                                   //  Washing program currently being processed
    vector<vector<string>> clothes;                                  //  Clothes currently loaded. Each clothing accessory has color and material it is made from
    time_t scheduledTime;                                            //  Date and time when washing program is started
    time_t pauseStartTime;                                           //  Date and time when washing program is paused
    map<string, WashingProgram*> customWashingPrograms;              //  Custom programs declared by user and saved in memory

public:
    static vector<string> clothingFabrics;                           //  All possible clothing materials a washing machine can receive
    static vector<string> machineStatus;                             //  All possible statuses of a washing machine
    static map<string, WashingProgram*> standardWashingPrograms;     //  Standard washing programs available to the user

    // Constructor
    explicit WashingMachine() {
        // Machine is initially turned off
        status = machineStatus[0];

        // Set starting environment
        waterSupplyAvailable = true;
        impurity = 0;
        detergentCache = 0;

        // Set elapsedTimeOnPause to 0
        elapsedTimeOnPause = 0.0;
        scheduledTime = 0;

        // Set a dummy value for current washing program
        currentProgram = *(new WashingProgram(0,0,0,0));
        // customWashingPrograms["Medium"] = new WashingProgram(0,0,0,0);

        // Set a dummy value for scheduled time
    }

    void setCurrentProgram(const WashingProgram &currentProgram);

    void setClothes(const vector<vector<string>> list);

    string getRecommendedWashingProgram();

    // Change value for one of the settings
    int set(const string& name, const string& value);

    // get status
    string getStatus() {
        return status;
    }

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

    // Check if custom program has valid data
    bool customProgramIsValid(WashingProgram washingProgram);

    WashingProgram* getCustomWashingProgram(string programName) const;

    int findColorInClothes(string color);

    int findFabricInClothes(string fabric);

    bool fabricInList(string fabric);

    string insertClothesMessage(json settingsValues);

    json getEnvironment();

    string setEnvironmentMessage(json environment);

    string scheduleProgramMessage(json settingsValues);

    string setSettingsMessage(json settingsValues);
};