cmake_minimum_required(VERSION 3.8)
project(iot_pi_connector)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main/code/cJSON.c
        main/code/cJSON.h
        main/code/cpustat.c
        main/code/iot.h
        main/code/iotmain.c
        main/code/jsonator.c
        main/code/jsonReader.c
        main/code/mac.c
        main/code/MQTTAsync.h
        main/code/MQTTClientPersistence.h
        main/code/mqttPublisher.c
        README.md)

add_executable(iot_pi_connector ${SOURCE_FILES})