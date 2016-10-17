String printIP(IPAddress ipa) {
  String response;
  response+=(IPAddress)ipa[0]; response+=".";
  response+=(IPAddress)ipa[1]; response+=".";
  response+=(IPAddress)ipa[2]; response+=".";
  response+=(IPAddress)ipa[3];
  return (response);
}


bool getPage(char *page, IPAddress cont) {
    HTTPClient http;      
    Serial.println("get " + String(page));
    http.begin("http://dweet.io/dweet/for/" + String(page) + "?ip=" + printIP(cont)); //HTTP
    int httpCode = http.GET();
    if(httpCode > 0) {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        if(httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            Serial.println(payload);
        }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    return true;
}
