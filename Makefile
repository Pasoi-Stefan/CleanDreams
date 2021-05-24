run: cmake-build-debug/cleandreams_washing_machine
	./cmake-build-debug/cleandreams_washing_machine

run-mqtt: cmake-build-debug/cleandreams_washing_machine
	./cmake-build-debug/cleandreams_washing_machine --mqtt

cmake-build-debug/cleandreams_washing_machine: cleandreams_washing_machine.cpp includes/WashingMachine.cpp includes/WashingMachine.h includes/ServerEndpoint.cpp includes/ServerEndpoint.h includes/WashingProgram.cpp includes/WashingProgram.h includes/MqttSubscriber.cpp includes/MqttSubscriber.h
	cmake -B cmake-build-debug
	make -C cmake-build-debug

install:
	sudo apt update
	sudo apt install g++
	
	# https://www.jetbrains.com/help/clion/how-to-use-wsl-development-environment-in-product.html#wsl-general
	# install the same packages as listed in step 3 of 'Configure WSL' for CLion
	sudo apt install cmake gcc clang gdb build-essential

	# https://github.com/pistacheio/pistache#ubuntu-ppa-unstable - how to install Pistache
	sudo add-apt-repository ppa:pistache+team/unstable
	sudo apt update
	sudo apt install libpistache-dev

	# https://github.com/nlohmann/json/issues/1703#issuecomment-687867182 - how to install nlohmann_json
	sudo apt install nlohmann-json3-dev

	# https://mosquitto.org/download/ - install mosquitto
	sudo apt-add-repository ppa:mosquitto-dev/mosquitto-ppa
	sudo apt-get update
	sudo apt install mosquitto mosquitto-clients libmosquitto-dev