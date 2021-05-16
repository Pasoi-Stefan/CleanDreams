#include <iostream>

using namespace std;

class WashingProgram {
private:
    //  Name of washing program
    double speed;               //  Spin speed
    double temperature;         //  Temperature of water used (in degrees Celsius)
    int time;                //  Duration of current washing program (in minutes)
    double detergent;           //  Quantity of detergent used (in grams)

public:

    WashingProgram() {}

    WashingProgram(double speed, double temperature, int time,
                   double detergent) : speed(speed), temperature(temperature),
                                       time(time), detergent(detergent) {}

    double getSpeed() const;

    double getTemperature() const;

    int getTime() const;

    double getDetergent() const;

    bool operator==(const WashingProgram &rhs) const;

    bool operator!=(const WashingProgram &rhs) const;

    WashingProgram& operator=(const WashingProgram &rhs);
};

