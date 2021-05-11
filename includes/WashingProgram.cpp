#include "WashingProgram.h"

int WashingProgram::getId() const {
    return id;
}


const string& WashingProgram::getName() const {
    return name;
}


double WashingProgram::getSpeed() const {
    return speed;
}


double WashingProgram::getTemperature() const {
    return temperature;
}


double WashingProgram::getTime() const {
    return time;
}


double WashingProgram::getDetergent() const {
    return detergent;
}


bool WashingProgram::operator==(const WashingProgram &rhs) const {
    return id == rhs.id &&
           name == rhs.name &&
           speed == rhs.speed &&
           temperature == rhs.temperature &&
           time == rhs.time &&
           detergent == rhs.detergent;
}


bool WashingProgram::operator!=(const WashingProgram &rhs) const {
    return !(rhs == *this);
}

WashingProgram& WashingProgram::operator=(const WashingProgram &rhs) {
    id = rhs.id;
    name = rhs.name;
    speed = rhs.speed;
    temperature = rhs.temperature;
    time = rhs.time;
    detergent = rhs.detergent;
    return *this;
}






