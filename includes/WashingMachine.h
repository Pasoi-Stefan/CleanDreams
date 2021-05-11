#include <iostream>
#include <nlohmann/json.hpp>
#include <iomanip>

#include "WashingProgram.h"

using namespace std;
using namespace nlohmann;

// Class with settings for washing machine
class WashingMachine {
private:
    static vector<string> clothingMaterials;                    //  All possible clothing materials a washing machine can receive
    static vector<string> machineStatus;                        //  All possible statuses of a washing machine
    static vector<WashingProgram*> standardWashingPrograms;     //  Standard washing programs available to the user

    string status;                                              //  Current status of washing machine
    double detergentCache;                                      //  Total quantity of detergent available for washing programs
    double impurity;                                            //  Percent of water impurity (give warning when impurity exceeds limit)
    double elapsedTimeOnPause;                                  //  Elapsed time of washing program when paused
    WashingProgram currentProgram;                              //  Washing program currently being processed
    pair<double, double> temperatureInterval;                   //  Current temperature interval of supplied water
    pair<string, string> clothes;                               //  Clothes currently loaded. Each clothing accessory has color and material it is made from
    time_t scheduledTime;                                       //  Date and time when washing program is started

public:
    // Constructor
    explicit WashingMachine() {
        // Code below is temporary

        // https://en.cppreference.com/w/cpp/chrono/c/localtime
        // Get current date and time and write them to console with a certain format
        time_t t = time(nullptr);
        cout << t << '\n';
        cout << "Current date and time: " << put_time(localtime(&t), "%d.%m.%Y %H:%M") << '\n';


        // Take a string containing date and time and convert to a struct
        struct tm tm;
        string filler = ":00";
        istringstream iss("13.02.2018 10:26" + filler);
        iss >> get_time(&tm, "%d.%m.%Y %H:%M:%S");
        // https://stackoverflow.com/a/50995391/15445526 - Solves the problem with one hour difference
        tm.tm_isdst = -1;
        time_t time = mktime(&tm);

        // Add an hour to time
        time += 3600;
        cout << put_time(localtime(&time), "%d.%m.%Y %H:%M") << '\n';

        // Machine is initially turned off
        status = machineStatus[0];
    }

    // Change value for one of the settings
    int set(const string& name, const string& value);

    // Get settings
    string get();
};