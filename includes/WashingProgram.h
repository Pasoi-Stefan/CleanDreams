#include <iostream>

using namespace std;

class WashingProgram {
private:
    int id;                     //  Unique identifier for washing program
    string name;                //  Name of washing program
    double speed;               //  Spin speed
    double temperature;         //  Temparature of water used (in degrees Celsius)
    double time;                //  Duration of current washing program (in minutes)
    double detergent;           //  Quantity of detergent used (in grams)

public:

    WashingProgram() {}

    WashingProgram(int id, const string &name, double speed, double temperature, double time,
                   double detergent) : id(id), name(name), speed(speed), temperature(temperature),
                                       time(time), detergent(detergent) {}

    int getId() const;

    const string &getName() const;

    double getSpeed() const;

    double getTemperature() const;

    double getTime() const;

    double getDetergent() const;

    bool operator==(const WashingProgram &rhs) const;

    bool operator!=(const WashingProgram &rhs) const;

    WashingProgram& operator=(const WashingProgram &rhs);
};

