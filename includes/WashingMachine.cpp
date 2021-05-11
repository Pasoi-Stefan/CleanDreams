#include "WashingMachine.h"

vector<string> WashingMachine::clothingMaterials = {"Silk", "Wool", "Cotton", "Leather", "Linen", "Velvet", "Hemp"};
vector<string> WashingMachine::machineStatus = {"NotStarted", "Paused", "Running" , "Finished"};
vector<WashingProgram*> WashingMachine::standardWashingPrograms = {
    new WashingProgram(1, "Fast", 700, 40, 30, 100),
    new WashingProgram(2, "Normal", 500, 50, 60, 150),
    new WashingProgram(3, "Long", 600, 60, 120, 200)
};

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
