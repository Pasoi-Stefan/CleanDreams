#pragma once

#include <string>
#include <mosquitto.h>

#include "WashingMachine.h"

class MqttSubscriber
{
private:
    int connection, id;
    struct mosquitto *mosq;
    
    static WashingMachine washingMachine;
    static void onConnect(struct mosquitto *mosq, void *obj, int connection);
    static void onMessage(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);

public:
    MqttSubscriber(string, int);
    ~MqttSubscriber();
    int connectLoop();
};