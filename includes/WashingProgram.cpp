#include "WashingProgram.h"


double WashingProgram::getSpeed() const {
    return speed;
}


double WashingProgram::getTemperature() const {
    return temperature;
}


int WashingProgram::getTime() const {
    return time;
}


double WashingProgram::getDetergent() const {
    return detergent;
}


bool WashingProgram::operator==(const WashingProgram &rhs) const {
    return speed == rhs.speed &&
           temperature == rhs.temperature &&
           time == rhs.time &&
           detergent == rhs.detergent;
}


bool WashingProgram::operator!=(const WashingProgram &rhs) const {
    return !(rhs == *this);
}

WashingProgram& WashingProgram::operator=(const WashingProgram &rhs) {
    speed = rhs.speed;
    temperature = rhs.temperature;
    time = rhs.time;
    detergent = rhs.detergent;
    return *this;
}






