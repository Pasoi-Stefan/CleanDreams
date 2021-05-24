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

    mosquitto_subscribe(mosq, NULL, "#", 0);
}

void MqttSubscriber::onMessage(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    string topic = msg->topic, raw_message = (char*) msg->payload;
    cout << "New message, Topic: [" << topic << "]:" << raw_message << '\n';

    try {
        json settingsValues = json::parse(raw_message);
        string result;

        if (topic == "settings") {
            result = washingMachine.setSettingsMessage(settingsValues);
        }
        else if (topic == "program") {
            result = washingMachine.scheduleProgramMessage(settingsValues);
        }
        else if (topic == "environment") {
            result = washingMachine.setEnvironmentMessage(settingsValues);
        }
        else if (topic == "clothes") {
            result = washingMachine.insertClothesMessage(settingsValues);
        }

        cout << result << '\n';
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