#include <iostream>
#include <fstream>

using namespace std;

#define MIN_SPEED 800
#define MAX_SPEED 2800
#define MIN_TEMPERATURE 20
#define MAX_TEMPERATURE 90
#define MIN_DETERGENT 50
#define MAX_DETERGENT 250
#define MIN_TIME 30
#define MAX_TIME 120


class WashingProgram {
private:
    //  Name of washing program
    double speed;               //  Spin speed
    double temperature;         //  Temperature of water used (in degrees Celsius)
    int time;                   //  Duration of current washing program (in minutes)
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

    bool isValid() const;

    void print(ostream& out);

    void read(istream& in);

    bool operator==(const WashingProgram &rhs) const;

    bool operator!=(const WashingProgram &rhs) const;

    WashingProgram& operator=(const WashingProgram &rhs);
};

