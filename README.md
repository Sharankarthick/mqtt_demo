# mqtt_demo

This project is a complete tutorial package for controlling a DC motor's speed and direction remotely over the internet. It uses an Arduino, the MQTT protocol, and a simple web interface that can be run from any mobile phone or computer.

This repository contains all the necessary code, hardware diagrams, and a presentation for your reference!

## ⚠️ A Quick Word on Safety

A motor, even a small one, cannot be connected directly to an Arduino's I/O pins. Doing so will permanently damage your microcontroller due to two main reasons:

    High Inrush Current: Motors draw a large spike of current when they start, far exceeding the 40mA limit of an Arduino pin.

    Back-EMF Voltage Spikes: When power is cut, the motor's collapsing magnetic field creates a high-voltage spike that can destroy the pin.

It is recommended to use a dedicated L293D Motor Driver IC to handle these issues safely. This is the standard, minimal, and safe way to interface a motor with a microcontroller.

## Features

    💨 Real-time Control: Instantly adjust motor speed.

    🌐 Remote Access: Control the motor from anywhere in the world via a simple web link.

    📡 Efficient & Fast: Uses the lightweight MQTT protocol, perfect for IoT applications.

    🏗️ Two Architectures: Includes code for both a secure, backend-driven setup and a simpler, direct-connection method.

    📚 Complete Tutorial Kit: Comes with all the code, wiring diagrams, and slides needed to teach or learn.

## Project Architecture

This project can be set up in one of two ways, depending on your needs.

### Method A: Firebase-Mediated (Secure)

The web app communicates with a Firebase database. A Python script on a server (or your PC) listens for changes and securely relays commands to the MQTT broker.

Flow: Web App ➡️ Firebase ➡️ Python Script ➡️ HiveMQ Broker ➡️ Arduino

    Best for: Projects where security is important.

    Benefit: Your MQTT credentials are never exposed on the public internet.

### Method B: Direct MQTT (Simple)

The web app uses MQTT over WebSockets to talk directly to the broker. This is simpler but less secure.

Flow: Web App ➡️ HiveMQ Broker ➡️ Arduino

    Best for: Quick demos, and learning.

    Caveat: Requires embedding MQTT credentials in the public-facing HTML file.

## Hardware Requirements

    Microcontroller: Arduino UNO R4 WiFi (or any WiFi-enabled board like an ESP32).

    Motor: Small 5V-9V DC motor (like a DVD motor).

    Motor Driver: L293D Motor Driver IC.

    Power Supply: An external 5V-9V power source for the motor.

    Breadboard & Jumper Wires.

## Software & Services

    Arduino IDE

    Libraries: PubSubClient & ArduinoJson (install via Library Manager).

    Python 3 (only for Method A).

    Cloud Services:

        A HiveMQ Cloud account for the MQTT Broker.

        A Google Firebase project (only for Method A).

        A GitHub account for hosting the web interface.

## Setup Instructions

### 1. Hardware Assembly 

    If L293D is used then follow the below connections. Else connect motor's (+ve) to pin 9 and (-ve) to any GND.

    Place the L293D chip on the breadboard (note the divot indicating pin 1).

    Power Connections:

        Connect your external power supply's (+) to the L293D Pin 8 (VCC2).

        Connect the Arduino's 5V pin to the L293D Pin 16 (VCC1).

        Connect the Arduino's GND to your external supply's GND (-).

        Connect this common ground line to the L293D's Pins 4, 5, 12, and 13.

    Arduino Control Connections:

        Arduino Pin 9 (PWM) ➡️ L293D Pin 1 for speed control.

        Arduino Pin 5V ➡️ L293D Pin 2 (Input 1) for enabling.

    Motor Connection:

        Connect your motor's two terminals to L293D Pin 3 (Output 1) and Ground.

### 2. Arduino Code

    Open the sketch from the /Arduino_Code folder in the Arduino IDE.

    Update the WiFi and HiveMQ credentials at the top of the file.

    Upload the code to your Arduino. Open the Serial Monitor at 115200 baud to check the connection status.

### 3. Web Controller Setup

Choose the method you want to use and host the corresponding HTML file from the /WebApp_Controller folder on GitHub Pages.

    For Method A (Firebase):

        In your Firebase project, create a Realtime Database.

        In Project Settings, create a Web App (</>) and copy the firebaseConfig object.

        Paste this object into index_firebase.html.

        Run the Python script from the /Python_Bridge folder to link Firebase and MQTT.

    For Method B (Direct MQTT):

        Open mqttDemo.html.

        Update the HiveMQ brokerHost, brokerPort (use 8884 for WebSockets), brokerUser, and brokerPass.

## Usage

Once everything is running, open the public GitHub Pages URL for your web app. Moving the slider and pressing the buttons will control your motor instantly!
