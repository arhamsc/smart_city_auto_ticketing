// #include <Arduino.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WifiManager.h>
#include <WifiClient.h>
#include <ArduinoJSON.h>
#include <ESP8266HTTPClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const int button = D5;
const int led1 = 2;
const char *serverIP = "http://192.168.1.2:3000";

SoftwareSerial ESPMod(D2, D3);
ESP8266WebServer webServer(80);

String httpGETRequest(const char *);
String httpPOSTRequestUserID(const char *, String);

const byte DNS_PORT = 53;
DNSServer dnsServer;

String trimCommand(String command)
{
  command.replace('#', ' ');
  command.trim();
  return command;
}

String responseHTML = ""
                      "<!DOCTYPE html><html><head><title>This is a Ticketing System</title></head><body>"
                      "<h1>Welcome to Smart City Ticketing System</h1></body></html>";

void handleRoot()
{
  ESPMod.println("##ledOn##");
  // ESPMod.write("\n");
  // digitalWrite(LED, 1);
  webServer.send(200, "text/plain", "hello from esp8266!\r\n");
  delay(1000);
  ESPMod.write("##ledOff##");
  ESPMod.write("\n");
  ESPMod.flush();
}

void handleWriteToCard()
{
  if (webServer.method() == HTTP_POST)
  {
    digitalWrite(led1, LOW);
    String response = "";
    DynamicJsonDocument requestBody(1024);
    for (uint8_t i = 0; i < webServer.args(); i++)
    {
      const String key = webServer.argName(i);
      const String value = webServer.arg(i);
      requestBody[key] = value;
      // message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
    }
    serializeJson(requestBody, response);
    // serializeJsonPretty(requestBody, Serial);
    // JsonObject data = requestBody.to<JsonObject>();

    if (!requestBody.containsKey("email"))
    {
      webServer.send(404, "application/json", "{message:'User Id not found'}");
      return;
    }
    ESPMod.write("##writeUserID##");
    ESPMod.write("\n");
    ESPMod.println(response);
    serializeJson(requestBody, response);
    while (trimCommand(ESPMod.readStringUntil('\n')).equals("done"))
      ; // do nothing until writing over
    webServer.send(200, "application/json", response);
    delay(1000);
    digitalWrite(led1, HIGH);
  }
  else
  {
    webServer.send(44, "text/plain", "Method not allowed");
  }
}

void handleGetDataFromAPI()
{
  DynamicJsonDocument res(1024);
  //* Change this in case localhost shifts
  String responseData = httpGETRequest("http://192.168.1.2:3000/users");
  // String responseData = httpGETRequest("http://172.20.10.4:3000/users");
  deserializeJson(res, responseData);

  Serial.println(res["name"].as<const char *>());
}

void handleGetUIDFromAPI()
{

  if (webServer.method() == HTTP_GET)
  {
    DynamicJsonDocument res(1024);
    ESPMod.println("##getUid##");
    String response = "";
    String uid;
    while (!(uid = trimCommand(ESPMod.readStringUntil('\n')).equals("done")))
    {
      // String uid = ESPMod.readStringUntil('\n');
      res["uid"] = uid;
      serializeJson(res, response);
    }
    webServer.send(200, "application/json", response);
  }
}

void handlePostDataToAPI(String user_id)
{
  DynamicJsonDocument res(1024);
  //* Change this URL
  String responseData = httpPOSTRequestUserID("http://192.168.1.2:3000/ticket/board", user_id);
  // String responseData = httpPOSTRequestUserID("http://172.20.10.4:3000/ticket/board", user_id);
  deserializeJson(res, responseData);

  Serial.println(res["message"].as<const char *>());
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  ESPMod.begin(9600);
  pinMode(D2, INPUT);
  pinMode(D3, OUTPUT);
  pinMode(led1, OUTPUT);    // Built in LED
  digitalWrite(led1, HIGH); // Inverted
  pinMode(button, INPUT);

  WiFiManager wifiManager;

  wifiManager.autoConnect("AutoConnectAP");

  Serial.println(WiFi.localIP());

  webServer.on("/", handleRoot);
  webServer.on("/write-to-card", handleWriteToCard);
  webServer.on("/uid", handleGetUIDFromAPI);

  webServer.onNotFound([]()
                       { webServer.send(200, "text/html", responseHTML); });
  webServer.begin();
}

void loop()
{
  webServer.handleClient();

  if (digitalRead(button) == HIGH)
  {
    Serial.println("Button Pressed");
    Serial.println(WiFi.localIP());
    // handleGetDataFromAPI();
    // handlePostDataToAPI();
    delay(1000);
  }

  if (ESPMod.available())
  {
    while (ESPMod.available() > 0)
    {
      String command = ESPMod.readStringUntil('\n');
      command = trimCommand(command);
      if (command.equals("readCard"))
      {
        String email = ESPMod.readStringUntil('\n');
        Serial.println(email);
        handlePostDataToAPI(email);
      }
    }
  }
  delay(30);
}

String httpGETRequest(const char *serverName)
{
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);

  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  return payload;
}

String httpPOSTRequestUserID(const char *serverName, String user_id)
{
  WiFiClient client;
  HTTPClient http;

  http.begin(client, serverName);

  http.addHeader("Content-Type", "application/json");
  const size_t capacity = JSON_OBJECT_SIZE(3) + JSON_ARRAY_SIZE(2) + 60;
  DynamicJsonDocument doc(capacity);

  doc["user_id"] = user_id;
  doc["bus_number"] = "KA04MF4146";
  doc["boarding"] = "kammanhalli";

  String requestBody;
  serializeJson(doc, requestBody);
  int httpResponseCode = http.POST(requestBody);

  String payload = "{}";

  if (httpResponseCode > 0)
  {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else
  {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  return payload;
}