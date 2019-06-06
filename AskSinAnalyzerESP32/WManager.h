#ifndef WIFIFUNCTIONS_H_
#define WIFIFUNCTIONS_H_

Preferences preferences;

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress IP = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(IP);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void checkWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi disconnected. Reconnect initiated.");
    WiFi.reconnect();
  }
}

bool doWifiConnect() {
  preferences.begin("wifi", false);
  String _ssid =  preferences.getString("ssid", "none");           //NVS key ssid
  String _psk =  preferences.getString("password", "none");   //NVS key password
  preferences.end();


  //Serial.println("ssid; " + _ssid);
  //Serial.println(" psk; " + _psk);


  String _pskMask = "";
  for (uint8_t i = 0; i < _psk.length(); i++) {
    _pskMask += "*";
  }
  const char* ipStr = NetConfig.ip; byte ipBytes[4]; parseBytes(ipStr, '.', ipBytes, 4, 10);
  const char* netmaskStr = NetConfig.netmask; byte netmaskBytes[4]; parseBytes(netmaskStr, '.', netmaskBytes, 4, 10);
  const char* gwStr = NetConfig.gw; byte gwBytes[4]; parseBytes(gwStr, '.', gwBytes, 4, 10);

  if (!startWifiManager && _ssid != "none" && _psk != "none" ) {
    digitalWrite(AP_MODE_LED_PIN, LOW);
    if (String(NetConfig.ip) != "0.0.0.0") {
      WiFi.config(IPAddress(ipBytes[0], ipBytes[1], ipBytes[2], ipBytes[3]), IPAddress(gwBytes[0], gwBytes[1], gwBytes[2], gwBytes[3]), IPAddress(netmaskBytes[0], netmaskBytes[1], netmaskBytes[2], netmaskBytes[3]));
    }
#ifdef USE_DISPLAY
    drawStatusCircle(ILI9341_YELLOW);
#endif
    WiFi.begin(_ssid.c_str(), _psk.c_str());
    uint8_t connect_count = 0;
    Serial.println("Connecting to WiFi.");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
      connect_count++;
      if (connect_count > 60) return false;
    }
    connect_count = 0;
    Serial.println("\nConnected to the WiFi network");
    printWifiStatus();
    return true;
  } else {
    digitalWrite(AP_MODE_LED_PIN, HIGH);
    WiFiManager wifiManager;
    WiFiManagerParameter custom_ip("custom_ip", "IP-Adresse", (String(NetConfig.ip) != "0.0.0.0") ? NetConfig.ip : "", IPSIZE, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_netmask("custom_netmask", "Netzmaske", (String(NetConfig.netmask) != "0.0.0.0") ? NetConfig.netmask : "", IPSIZE, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_gw("custom_gw", "Gateway",  (String(NetConfig.gw) != "0.0.0.0") ? NetConfig.gw : "", IPSIZE, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_ccuip("ccu", "IP der CCU", HomeMaticConfig.ccuIP, IPSIZE, "pattern='((^|\\.)((25[0-5])|(2[0-4]\\d)|(1\\d\\d)|([1-9]?\\d))){4}$'");
    WiFiManagerParameter custom_svanalyzeinput("svanalyzeinput", "SV Analyze Input", HomeMaticConfig.SVAnalyzeInput, VARIABLESIZE, "pattern='[A-Za-z0-9_ -]+'");
    WiFiManagerParameter custom_svanalyzeoutput("svanalyzeoutput", "SV Analyze Output", HomeMaticConfig.SVAnalyzeOutput, VARIABLESIZE, "pattern='[A-Za-z0-9_ -]+'");

    wifiManager.setSaveConfigCallback(saveConfigCallback);

    //set static ip
    //wifiManager.setSTAStaticIPConfig(IPAddress(10, 0, 1, 99), IPAddress(10, 0, 1, 1), IPAddress(255, 255, 255, 0));
    WiFi.mode(WIFI_STA);

    wifiManager.addParameter(&custom_ccuip);
    wifiManager.addParameter(&custom_svanalyzeinput);
    wifiManager.addParameter(&custom_svanalyzeoutput);
    wifiManager.addParameter(&custom_ip);
    wifiManager.addParameter(&custom_netmask);
    wifiManager.addParameter(&custom_gw);

    if (!wifiManager.startConfigPortal("AskSinAnalyzer-AP")) {
      Serial.println("failed to connect and hit timeout");
      delay(3000);
      ESP.restart();
      delay(5000);
    }

    Serial.println("Wifi Connected");

    if (shouldSaveConfig) {
      preferences.begin("wifi", false); // Note: Namespace name is limited to 15 chars
      Serial.println("Writing new ssid");
      preferences.putString("ssid", WiFi.SSID());

      Serial.println("Writing new pass");
      preferences.putString("password", WiFi.psk());
      delay(300);
      preferences.end();


      if (String(custom_ip.getValue()).length() > 5) {
        Serial.println("Custom static IP Address is set!");
        strcpy(NetConfig.ip, custom_ip.getValue());
        strcpy(NetConfig.netmask, custom_netmask.getValue());
        strcpy(NetConfig.gw, custom_gw.getValue());

      } else {
        strcpy(NetConfig.ip,      "0.0.0.0");
        strcpy(NetConfig.netmask, "0.0.0.0");
        strcpy(NetConfig.gw,      "0.0.0.0");
      }
      strcpy(HomeMaticConfig.ccuIP, custom_ccuip.getValue());
      strcpy(HomeMaticConfig.SVAnalyzeInput, custom_svanalyzeinput.getValue());
      strcpy(HomeMaticConfig.SVAnalyzeOutput, custom_svanalyzeoutput.getValue());

      saveSystemConfig();
      digitalWrite(AP_MODE_LED_PIN, LOW);
    }
  }
}

#endif