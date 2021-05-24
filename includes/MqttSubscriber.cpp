#include "MqttSubscriber.h"

WashingMachine MqttSubscriber::washingMachine;

MqttSubscriber::MqttSubscriber(string clientName, int obj) {
    mosquitto_lib_init();
    this->id = obj;
    mosq = mosquitto_new(clientName.c_str(), true, &id);
}

MqttSubscriber::~MqttSubscriber() {
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

void MqttSubscriber::onConnect(struct mosquitto *mosq, void *obj, int connection) {
    printf("ID: %d\n", *(int*) obj);
    if (connection != 0) {
        fprintf(stderr, "Error with result code: %d\n", connection);
        exit(-1);
    }

    mosquitto_subscribe(mosq, NULL, "settings", 0);
    mosquitto_subscribe(mosq, NULL, "program", 0);
    mosquitto_subscribe(mosq, NULL, "environment", 0);
    mosquitto_subscribe(mosq, NULL, "clothes", 0);
    mosquitto_subscribe(mosq, NULL, "status-request", 0);
    mosquitto_subscribe(mosq, NULL, "environment-request", 0);
    mosquitto_subscribe(mosq, NULL, "recommandations-request", 0);
    mosquitto_subscribe(mosq, NULL, "custom-program", 0);
}

void MqttSubscriber::onMessage(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    string topic = msg->topic, raw_message = (char*) msg->payload;
    cout << "New message, Topic: [" << topic << "]" << '\n';

    try {
        json settingsValues = json::parse(raw_message);
        string result;

        if (topic == "settings") {
            result = washingMachine.setSettingsMessage(settingsValues);
            mosquitto_publish(mosq, NULL, (topic + "-output").c_str(), result.length(), result.c_str(), 0, false);
        }
        else if (topic == "program") {
            result = washingMachine.scheduleProgramMessage(settingsValues);
            mosquitto_publish(mosq, NULL, (topic + "-output").c_str(), result.length(), result.c_str(), 0, false);
        }
        else if (topic == "environment") {
            result = washingMachine.setEnvironmentMessage(settingsValues);
            mosquitto_publish(mosq, NULL, (topic + "-output").c_str(), result.length(), result.c_str(), 0, false);
        }
        else if (topic == "clothes") {
            result = washingMachine.insertClothesMessage(settingsValues);
            mosquitto_publish(mosq, NULL, (topic + "-output").c_str(), result.length(), result.c_str(), 0, false);
        }
        else if (topic == "custom-program") {
            result = washingMachine.setCustomWashingProgram(settingsValues);
            mosquitto_publish(mosq, NULL, "program-output", result.length(), result.c_str(), 0, false);
        }
        else if (topic == "status-request") {
            result = washingMachine.getScheduleAndProgram();
            mosquitto_publish(mosq, NULL, "status-output", result.length(), result.c_str(), 0, true);
        }
        else if (topic == "environment-request") {
            result = washingMachine.getEnvironment();
            mosquitto_publish(mosq, NULL, "environment-output", result.length(), result.c_str(), 0, true);
        }
        else if (topic == "recommandations-request") {
            result = washingMachine.getRecommendedWashingProgram();
            mosquitto_publish(mosq, NULL, "recommandations-output", result.length(), result.c_str(), 0, false);
        }
    }
    catch (json::parse_error& e) {
        cout << e.what() << '\n';
    }
}

int MqttSubscriber::connectLoop() {
    mosquitto_connect_callback_set(mosq, onConnect);
    mosquitto_message_callback_set(mosq, onMessage);
    connection = mosquitto_connect(mosq, "localhost", 1883, 10);
    if (connection)
    {
        fprintf(stderr, "Could not connect to Broker with return code %d\n", connection);
        return -1;
    }

    mosquitto_loop_start(mosq);
    printf("Press Enter to quit...\n");
    getchar();

    mosquitto_loop_stop(mosq, true);
    mosquitto_disconnect(mosq);

    return 0;
}