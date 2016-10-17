// by Johan Hoeksma. 

WebSocketsServer webSocket = WebSocketsServer(81);
int cur = 0;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.print("Websocket disconnect ");
            Serial.println(num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.print("Websocket connect ");
                Serial.println(num);
                webSocket.sendTXT(num, "Connected");
                cur = num;
            }
            break;
        case WStype_TEXT:
            break;
        case WStype_BIN:
            hexdump(payload, lenght);
            break;
    }
}

void runWs() {
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

void loopWs() {
    webSocket.loop();  
}

void sendWs(String s) {
    webSocket.sendTXT(cur, s); 
}
