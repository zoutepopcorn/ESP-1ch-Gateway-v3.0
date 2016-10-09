//
// Copyright (c) 2016 Maarten Westenberg version for ESP8266
// Verison 3.2.0
// Date: 2016-10-08
//
// 	based on work done by Thomas Telkamp for Raspberry PI 1ch gateway
//	and many others.
//
// All rights reserved. This program and the accompanying materials
// are made available under the terms of the MIT License
// which accompanies this distribution, and is available at
// https://opensource.org/licenses/mit-license.php
//
// Author: Maarten Westenberg
//
// This file contains the webserver code for the ESP Single Channel Gateway.

// ================================================================================
// WEBSERVER FUNCTIONS 

#if A_SERVER==1


// ----------------------------------------------------------------------------
// Output the 4-byte IP address for easy printing
// ----------------------------------------------------------------------------
String printIP(IPAddress ipa) {
	String response;
	response+=(IPAddress)ipa[0]; response+=".";
	response+=(IPAddress)ipa[1]; response+=".";
	response+=(IPAddress)ipa[2]; response+=".";
	response+=(IPAddress)ipa[3];
	return (response);
}

// ----------------------------------------------------------------------------
// stringTime
// Only when RTC is present we print real time values
// t contains number of milli seconds since system started that the event happened.
// So a value of 100 wold mean that the event took place 1 minute and 40 seconds ago
// ----------------------------------------------------------------------------
String stringTime(unsigned long t) {
	String response;
	String Days[7]={"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

	if (t==0) { response = " -none- "; return(response); }
	
	// now() gives seconds since 1970
	time_t eventTime = now() - ((millis()-t)/1000);
	byte _hour   = hour(eventTime);
	byte _minute = minute(eventTime);
	byte _second = second(eventTime);
	
	response += Days[weekday(eventTime)-1]; response += " ";
	response += day(eventTime); response += "-";
	response += month(eventTime); response += "-";
	response += year(eventTime); response += " ";
	if (_hour < 10) response += "0";
	response += _hour; response +=":";
	if (_minute < 10) response += "0";
	response += _minute; response +=":";
	if (_second < 10) response += "0";
	response += _second;
	return (response);
}


// ----------------------------------------------------------------------------
// WIFI SERVER
//
// This funtion implements the WiFI Webserver (very simple one). The purpose
// of this server is to receive simple admin commands, and execute these
// results are sent back to the web client.
// Commands: DEBUG, ADDRESS, IP, CONFIG, GETTIME, SETTIME
// The webpage is completely built response and then printed on screen.
// ----------------------------------------------------------------------------
String WifiServer(char *cmd, char *arg) {

	String response;
	char *dup, *pch;

	yield();	
	if (debug >=2) { Serial.print(F("WifiServer new client")); }

	// These can be used as a single argument
	if (strcmp(cmd, "DEBUG")==0) {									// Set debug level 0-2
		debug=atoi(arg); response+=" debug="; response+=arg;
	}
	if (strcmp(cmd, "DELAY")==0) {									// Set debug level 0-2
		txDelay+=atoi(arg)*1000; response+=" delay="; response+=arg;
	}
	if (strcmp(cmd, "IP")==0) {										// List local IP address
		response+=" local IP="; 
		response+=(IPAddress) WiFi.localIP()[0]; response += ".";
		response+=(IPAddress) WiFi.localIP()[1]; response += ".";
		response+=(IPAddress) WiFi.localIP()[2]; response += ".";
		response+=(IPAddress) WiFi.localIP()[3];
	}

	if (strcmp(cmd, "GETTIME")==0) { response += "gettime tbd"; }	// Get the local time
	if (strcmp(cmd, "SETTIME")==0) { response += "settime tbd"; }	// Set the local time
	if (strcmp(cmd, "HELP")==0)    { response += "Display Help Topics"; }
	if (strcmp(cmd, "RESET")==0)   { 
		response += "Resetting Statistics"; 
		cp_nb_rx_rcv = 0;
		cp_nb_rx_ok = 0;
		cp_up_pkt_fwd = 0;
	}
	if (strcmp(cmd, "NEWSSID")==0) { 
		WiFiManager wifiManager;
		strcpy(wpa[0][0],""); 
		strcpy(wpa[0][1],"");
		WiFi.disconnect();
		wifiManager.autoConnect(AP_NAME, AP_PASSWD );
	}

	// Do work, fill the webpage
	delay(15);	
	response +="<!DOCTYPE HTML>";
	response +="<HTML><HEAD>";
	response +="<TITLE>ESP8266 1ch Gateway</TITLE>";
	response +="</HEAD>";
	response +="<BODY>";
		
	response +="<h1>ESP Gateway Config:</h1>";
	response +="Version: "; response+=VERSION;
	response +="<br>ESP is alive since "; response+=stringTime(1); 
	response +="<br>Current time is    "; response+=stringTime(millis()); 
	response +="<br>";
		
	response +="<h2>WiFi Config</h2>";
	response +="<table style=\"max_width: 100%; min-width: 40%; border: 1px solid black; border-collapse: collapse;\" class=\"config_table\">";
	response +="<tr>";
	response +="<th style=\"background-color: green; color: white;\">Parameter</th>";
	response +="<th style=\"background-color: green; color: white;\">Value</th>";
	response +="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">WiFi SSID</td><td style=\"border: 1px solid black;\">"; response+=WiFi.SSID(); response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">IP Address</td><td style=\"border: 1px solid black;\">"; response+=printIP((IPAddress)WiFi.localIP()); response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">IP Gateway</td><td style=\"border: 1px solid black;\">"; response+=printIP((IPAddress)WiFi.gatewayIP()); response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">NTP Server</td><td style=\"border: 1px solid black;\">"; response+=NTP_TIMESERVER; response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">LoRa Router</td><td style=\"border: 1px solid black;\">"; response+=_TTNSERVER; response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">LoRa Router IP</td><td style=\"border: 1px solid black;\">"; response+=printIP((IPAddress)ttnServer); response+="</tr>";

#ifdef _THINGSERVER
	response +="<tr><td style=\"border: 1px solid black;\">LoRa Router 2</td><td style=\"border: 1px solid black;\">"; response+=_THINGSERVER; 
		response += ":"; response += String(_THINGPORT); response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">LoRa Router 2 IP</td><td style=\"border: 1px solid black;\">"; response+=printIP((IPAddress)thingServer); response+="</tr>";
#endif
	response +="</table>";
	
	response +="<h2>System Status</h2>";
	response +="<table style=\"max_width: 100%; min-width: 40%; border: 1px solid black; border-collapse: collapse;\" class=\"config_table\">";
	response +="<tr>";
	response +="<th style=\"background-color: green; color: white;\">Parameter</th>";
	response +="<th style=\"background-color: green; color: white;\">Value</th>";
	response +="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Free heap</td><td style=\"border: 1px solid black;\">"; response+=ESP.getFreeHeap(); response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">ESP Chip ID</td><td style=\"border: 1px solid black;\">"; response+=ESP.getChipId(); response+="</tr>";
	response +="</table>";
		
	response +="<h2>LoRa Status</h2>";
	response +="<table style=\"max_width: 100%; min-width: 40%; border: 1px solid black; border-collapse: collapse;\" class=\"config_table\">";
	response +="<tr>";
	response +="<th style=\"background-color: green; color: white;\">Parameter</th>";
	response +="<th style=\"background-color: green; color: white;\">Value</th>";
	response +="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Frequency</td><td style=\"border: 1px solid black;\">"; response+=freq; response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Spreading Factor</td><td style=\"border: 1px solid black;\">"; response+=sf; response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Gateway ID</td><td style=\"border: 1px solid black;\">";	
        // The MAC array is always returned in lowercase
	for(int i = 0; i< 6; i++) {
	    if(i == 3) {
		response += "ffff"; 
	    }
	    if(MAC_array[i] <= 0x0F) {
		response += "0";         // ADD A ZERO BEFORE THE HEX
	    }
	    response += String(MAC_array[i],HEX);  
	}
	response+="</tr>";
	response +="</table>";
		
	response +="<h2>Statistics</h2>";
	delay(1);
	response +="<table style=\"max_width: 100%; min-width: 40%; border: 1px solid black; border-collapse: collapse;\" class=\"config_table\">";
	response +="<tr>";
	response +="<th style=\"background-color: green; color: white;\">Counter</th>";
	response +="<th style=\"background-color: green; color: white;\">Value</th>";
	response +="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Packages Received Total</td><td style=\"border: 1px solid black;\">"; response +=cp_nb_rx_rcv; response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Packages Received OK </td><td style=\"border: 1px solid black;\">"; response +=cp_nb_rx_ok; response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Packages Forwarded</td><td style=\"border: 1px solid black;\">"; response +=cp_up_pkt_fwd; response+="</tr>";
	response +="<tr><td>&nbsp</td><td> </tr>";
			
	response +="</table>";

	response +="<br>";
	response +="<h2>Settings</h2>";
	response +="<table style=\"max_width: 100%; min-width: 40%; border: 1px solid black; border-collapse: collapse;\" class=\"config_table\">";
	response +="<tr>";
	response +="<th style=\"background-color: green; color: white;\">Counter</th>";
	response +="<th style=\"background-color: green; color: white;\">Value</th>";
	response +="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">SF Setting</td><td style=\"border: 1px solid black;\">"; response += sf; response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Frequency</td><td style=\"border: 1px solid black;\">"; response += freq; response+="</tr>";
	response +="<tr><td style=\"border: 1px solid black;\">Timing Correction (uSec)</td><td style=\"border: 1px solid black;\">"; response += txDelay; response+="</tr>";
	response +="<tr><td>&nbsp</td><td> </tr>";
			
	response +="</table>";

	
	response +="Click <a href=\"/RESET\">here</a> to reset statistics<br><br>";
	
	response +="Click <a href=\"/NEWSSID\">here</a> to reset accesspoint<br>";

	response +="Set txDelay level: ";
	response +=" <a href=\"DELAY=1\">Higher</a><nbsp><nbsp><nbsp><nbsp>";
	response +=" <a href=\"DELAY=-1\">Lower</a>";
	response +=" <br><br>";
	
	response +="Debug level is: "; 
	response += debug; 
	response +=" set to: ";
	response +=" <a href=\"DEBUG=0\">0</a>";
	response +=" <a href=\"DEBUG=1\">1</a>";
	response +=" <a href=\"DEBUG=2\">2</a><br>";
		
	response +="Click <a href=\"/HELP\">here</a> to explain Help and REST options<br>";
	response +="</BODY></HTML>";

	delay(5);
	free(dup);									// free the memory used, before jumping to other page
	return (response);
}

#endif

