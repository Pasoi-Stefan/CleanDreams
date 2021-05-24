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

bool WashingProgram::isValid() const {
    return !(detergent < MIN_DETERGENT ||
    detergent > MAX_DETERGENT ||
    speed < MIN_SPEED ||
    speed > MAX_SPEED ||
    temperature > MAX_TEMPERATURE ||
    temperature < MIN_TEMPERATURE ||
    time < MIN_TIME ||
    time > MAX_TIME);
}

void WashingProgram::print(ostream& out) {
    out << speed << ' ' << temperature << ' ';
    out << time << ' ' << detergent << '\n';
}

void WashingProgram::read(istream& in) {
    in >> speed >> temperature >> time >> detergent;
}






