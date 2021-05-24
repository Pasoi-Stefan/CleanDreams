#include <string>

#include "includes/ServerEndpoint.h"
#include "includes/MqttSubscriber.h"

int main(int argc, char *argv[]) {
    // https://github.com/pistacheio/pistache/issues/242#issuecomment-494441391
    // It appears Pistache is natively multi-threaded

    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "--mqtt") {
            MqttSubscriber mqtt("clean-dreams", 1);
            int err = mqtt.connectLoop();
            if (err != 0) {
                cout << "MQTT connection failed" << '\n';
            }
            return 0;
        }
        if (arg == "--http") {
            break;
        }
    }

    // Set a port on which your server to communicate
    Port port(9080);

    Address addr(Ipv4::any(), port);

    // Instance of the class that defines what the server can do
    ServerEndpoint stats(addr);

    // Initialize and start the server
    stats.init();
    stats.start();

    cout << "Server started" << '\n';
    cout << "Host: " << addr.host() << '\n';
    cout << "Port: " << addr.port() << '\n';

    // Wait for user to stop the server
    stats.waitForSignal();
}