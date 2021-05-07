#include "WashingMachine.h"

int WashingMachine::set(const string& name, const string& value) {
    try {
        if (name == "status") {
            status = static_cast<int8_t>(stoi(value));
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
    json responseBody = {
            {"status", status}
    };

    return responseBody.dump();
}
