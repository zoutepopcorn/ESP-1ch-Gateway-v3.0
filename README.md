#Single Channel LoRaWAN Gateway

Version 3.0, September 27, 2016
Author: M. Westenberg (mw12554@hotmail.com)
Copyright: M. Westenberg (mw12554@hotmail.com)

This repository contains a proof-of-concept implementation of a single
channel LoRaWAN gateway. It has been tested on the Wemos D1 Mini, using a 
HopeRF RFM95W transceiver.  The nodes tested are:
- TeensyLC with HopeRF RFM95 radio
- Arduino Pro-Mini (default Armega328 model, 8MHz 3.3V)

###Note

There seems to be a lot of variation in Arduino Pro-Mini devices. I have at least 4 different types. 
Most work, and some do not work with the 1ch Gateway mostly due to timing issues so it seems.
The standard 8MHz type with the large chrystal on board seems to be working best.

The code is for testing and development purposes only, and is not meant 
for production usage yet. 

Version 1 was originally based on code base of Single Channel gateway for RaspberryPI
which was developed by Thomas Telkamp. Code was ported and extended to run on ESP 8266 
mcu and provide RTC, Webserver and DNS services.
Version 2.0 adds several enhancements and part have been completely redesigned.
Changes include two-way traffic. The code is also slit over multiple source files
which makes editing easier.
Version 3.0 includes WiFi Master support which makes it easy to use the 1-ch gateway for demo purposes where you need to quickly connect the gateway to a WiFi accesspoint (or your mobiel phone).It also contains limited SPIFF filesystem support which enables the ESP to store its data in a more persistent way.

Maintained by Maarten Westenberg (mw12554@hotmail.com)

##Features

- Supports ABP nodes (TeensyLC and Arduino Pro-mini)
- Supports OTAA functions on TeensyLC and Arduino Pro-Mini (not all of them) for SF7 and SF8.
- Supports SF7, SF8. SF7 is tested for downstream communication
- Listens on configurable frequency and spreading factor
- Send status updates to server (keepalive)
- PULL_DATA messages to server
- It can forward messages to two servers at the same time (and read from them as well)
- DNS support for server lookup
- NTP Support for time sync with internet time servers
- Webserver support (default port 8080)
- WiFiManager support
- Limited SPIFF (filesystem) support for persistent data storage
- .h header file for configuration

Not (yet) supported:

- SF7BW250 modulation
- FSK modulation
- SF9-SF12 downlink messaging not tested yet

##Dependencies

The software is dependent on several pieces of software, the Ardiuino IDE for ESP8266 
being the most important. Several other libraries are also used by this program, make sure you install those libraries with the IDE:

- gBase64 library, The gBase library is actually a base64 library made 
	by Adam Rudd (url=https://github.com/adamvr/arduino-base64). I changed the name because I had
	another base64 library installed on my system and they did not coexist well.
- Time library (http://playground.arduino.cc/code/time)
- Arduino JSON; Needed to decode downstream messages
- SimpleTimer; ot yet used, but reserved for interrupt and timing
- WiFiManager
- ArduinoJson
- Streaming

For simplicity, the libraries are also found in this gitshub repository in the libraries directory. 
However, these libraries are not part of the single-channel Gateway software.

##Connections

See http://things4u.github.io in the hardware section for building
and connection instructions

##Configuration

###Connect to WiFI with WiFiManager

The easiest way to configure the Gateway on WiFi is by using the WiFimanager function. This function works out of the box. WiFiManager will put the gateway in accesspoint mode so that you can connect to it as a WiFi accesspoint. 

The standard access point name used by the gateway is "ESP8266 Gway" and its password is "ttnAutoPw". After binding to the access point with your mobile phone or computer, go to htp://192.168.4.1 in a browser and tell the gateway to which WiFi network you want it to connect, and specify the password.

The gateway will then reset and bind to the given network. If all goes well you are now set and the ESP8266 will remember the network that it must connect to.



Note: All settings and the more advanced features can be set by editing the configuration file (see the next section). 

### Editing ESP-sc-gway.h

All user configurable settings are put in the ESP-sc-gway.h file as much as possible.
The most important things to configure to your own environment are:

- static char *wpa[WPASIZE][2] contains the array of known WiFi access points the Gateway will connect to.
Make sure that the dimensions of the array are correctly defined in the WPASIZE settings. 
Note: When the WiFiManager software is enabled (it is by default) there must at least be one entry in the wpa file, wpa[0] is used for storing WiFiManager information.
- Only the sx1276 (and HopeRF 95) radio modules are supported at this time. The sx1272 code should be 
working without much work, but as I do not have one of these modules available I cannot test this.
- This software allows to connect to 2 servers at the same time (as most gateways do BTW). 
Make sure that you set:

 \#define _THINGPORT 1701							// Your UDP server should listen to this port  
 \#define _THINGSERVER "your_server.com"			// Server URL of the LoRa udp.js server program  

- Set the identity parameters for your gateway:  
// Gateway Ident definitions, please set location, email and description.  

\#define _DESCRIPTION "ESP Gateway"  
\#define _EMAIL "your.email@provider.com"  
\#define _PLATFORM "ESP8266"  
\#define _LAT 52.00  
\#define _LON 5.00  
\#define _ALT 0  


###Lora Radio Defaults:  

- LoRa:   SF7 at 868.1 Mhz
- Server:  
  \#define _TTNSERVER "router.eu.thethings.network"  
  \#define _TTNPORT 1700  
  These two settings are mandatory and should point to the standard servers of TTN
  40.114.249.243, port 1700 
  
Edit .h file (ESP-sc-gway.h) to change configuration (look for: "Configure these values!").

###Webserver

The built-in webserver can be used to display status and debugging information. It can be accessed with the following URL: http://<gatewayIP>:8080 The webserver shows various configuration settings as well as providing functions to set debug mode and reset statistics and the WiFiManager accesspoint.

##Notes

The Gateay timestamps are according to the LoRa specification: 
- Receive_Delay1 1s
- Receive Delay2	2s (starting after Receive_Delay1)
- Join_Accept_Delay1 5s
- Join_Accept_Delay2 6s


##License

The source files of the gateway sketch in this repository is made available under the Eclipse
Public License v1.0. The libraries included in this repository are included for convenience only and all have their own license.
