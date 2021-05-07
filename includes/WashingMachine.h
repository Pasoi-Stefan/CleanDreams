#include <iostream>
#include <nlohmann/json.hpp>

using namespace std;
using namespace nlohmann;

// Class with settings for washing machine
class WashingMachine {
private:
    // The machine's status can be 0 (off) or 1 (on)
    int8_t status;

public:
    // Constructor
    explicit WashingMachine() {
        // Machine is initially turned off
        status = 0;
    }

    // Change value for one of the settings
    int set(const string& name, const string& value);

    // Get settings
    string get();
};