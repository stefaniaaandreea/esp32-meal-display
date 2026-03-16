# ESP32 Meal API Display

This project uses an ESP32 microcontroller to connect to a WiFi network, retrieve recipe data from an online API and display information on a multiplexed 2-digit 7-segment display.

## Features

* WiFi connection using ESP32
* HTTP GET request to retrieve data from an API
* JSON parsing using ArduinoJson
* Serial monitor output for debugging
* Multiplexed control of a 7-segment display

## Hardware

* ESP32 development board
* 2-digit 7-segment display
* Breadboard and resistors

## Software

Developed using the Arduino IDE.

Libraries used:

* WiFi.h
* WiFiClient.h
* HTTPClient.h
* ArduinoJson

## Project Description

The ESP32 connects to a WiFi network and sends an HTTP request to an API that returns recipe information in JSON format.

The program extracts fields such as:

* Meal ID
* Meal name
* Meal category

The extracted information is displayed on a multiplexed 7-segment display while additional information is printed on the serial monitor.

## Notes

WiFi credentials were removed from the public version of the code for security reasons.

## Author

Electronics and Telecommunications Student
National University of Science and Technology POLITEHNICA Bucharest
