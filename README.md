# CleanDreams

## Setup

You will need Ubuntu. If you are running the app for the first time, install the dependencies:

    $ make install

This will install g++, [pistache](https://github.com/pistacheio/pistache) (for the web server), [nlohmann-json](https://github.com/nlohmann/json) (for working with JSON files) and [mosquitto](https://github.com/eclipse/mosquitto) (for [MQTT](https://mqtt.org/)).

## Build and run
To run on HTTP:

    $ make

or

    $ make run

To run on MQTT, use:

    $ make run-mqtt

Note: you will need to have a broker already started with the default settings. You can do this by simply using the following command in another terminal:

    $ mosquitto

## Run some examples

### HTTP
For HTTP requests you can use specialised software such as [Postman](https://www.postman.com/). You can also use [curl](https://curl.se/). The following examples will use curl:

    $ curl -X POST -d @test_jsons/environment.json http://localhost:9080/environment
    $ curl -X POST -d @test_jsons/clothes.json http://localhost:9080/clothes
    $ curl -X POST -d @test_jsons/program.json http://localhost:9080/program
    $ curl -X POST -d @test_jsons/settings.json http://localhost:9080/settings
    $ curl -X POST -d @test_jsons/custom_program.json http://localhost:9080/addCustom
    $ curl -X GET http://localhost:9080/environment
    $ curl -X GET http://localhost:9080/clothes
    $ curl -X GET http://localhost:9080/program


### MQTT
When running in MQTT mode, the program will subscribe to the following topics:  
`settings, program, environment, clothes, custom-program, status-request, environment-request, recommandations-request`

The first 5 topics are used to change the values of the washing machine by sending JSON (strings that can be parsed as JSON) messages following the [specification.json](specification.json) in a similar way to the POST requests. The last 3 topics are used with an empty JSON-body message to request the desired data.

You can use `mosquitto_pub` to publish messages on a desired topic:

    $ mosquitto_pub -t environment -f test_jsons/environment.json 
    $ mosquitto_pub -t environment-request -m "{}"

The app will broadcast messages about the results on the `settings-output, program-output, environment-output,  clothes-output, status-output, recommandations-output` topics.

Using `mosquitto_sub` you can subscribe to the output topics:

    $ mosquitto_sub -t environment-output -t program-output 


## Team members
- **[Pâsoi Ștefan](https://github.com/Pasoi-Stefan)**
- **[Giugioiu Marian](https://github.com/MarianGiugioiu)**
- **[Simion Cristian](https://github.com/CristianSimion5)**
- **[Bleoțiu Cristian](https://github.com/bleotiu)**
- **[Rotaru Iulian](https://github.com/iulianrotaru)**
- **[Andreica Radu](https://github.com/AndreicaRadu)**