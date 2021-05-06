run: cmake-build-debug/cleandreams_washing_machine
	./cmake-build-debug/cleandreams_washing_machine

cmake-build-debug/cleandreams_washing_machine: cleandreams_washing_machine.cpp
	cmake -B cmake-build-debug
	make -C cmake-build-debug

install:
	# https://github.com/pistacheio/pistache#ubuntu-ppa-unstable - how to install Pistache
	sudo add-apt-repository ppa:pistache+team/unstable
	sudo apt update
	sudo apt install libpistache-dev

	# https://github.com/nlohmann/json/issues/1703#issuecomment-687867182 - how to install nlohmann_json
	sudo apt install nlohmann-json3-dev

	# install CMake
	sudo apt install cmake