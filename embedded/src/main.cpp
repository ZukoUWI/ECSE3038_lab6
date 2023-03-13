#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>

//Stores Credentials
#include "env.h"

// replace with your API endpoint
const char* apiEndpoint = "https://zuko-lab6-api.onrender.com";

// Random temperature generator between 24 - 36
float getTemp(){
  return random(24.2,36.0);
}

//Initializing fan & led pins
#define fanPin = 22;
#define lightPin = 23;

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi network using the USERNAME and PASSWORD from the env.h file
  WiFi.begin(WIFI_USER, WIFI_PASS);
  Serial.println("Connecting to WiFi..");

  //Prints "." every 0.5 seconds until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Prints Connected with IP Address once connection is established
  Serial.println("");
  Serial.println("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){
    Serial.println("");
    HTTPClient http;
  
    // Adding api state routing to url to establish connection with server
    String url = String(apiEndpoint) + "/api/state";
    http.begin(url);
    http.addHeader("Content-type", "application/json");
    http.addHeader("Content-length", "23");


    //Setting document size for PUT
    StaticJsonDocument<1024> docput;
    String httpRequestData;
    docput["temperature"] = getTemp();

    serializeJson(docput, httpRequestData);

    //Http PUT request
    int httpResponseCode = http.PUT(httpRequestData);
    String http_response;

    //HTTP response codes if statements
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      Serial.print("HTTP Response from server: ");
      http_response = http.getString();
      Serial.println(http_response);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    http.end();  

    //Establish Connection with server for GET request  
    http.begin(url);
    httpResponseCode = http.GET();

    //HTTP response codes if statements
    if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        Serial.print("Response from server: ");
        http_response = http.getString();
        Serial.println(http_response);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
    }
  
    //Setting document size for GET
    StaticJsonDocument<1024> docget;
    DeserializationError error = deserializeJson(docget, http_response);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }
    
    bool temp = docget["fan"]; 
    bool light= docget["light"]; 

    digitalWrite(fanPin,temp);
    digitalWrite(lightPin,temp);
    
    //End connection to the server
    http.end();
  }
  //Otherwise print Wifi disconnected if connection terminates
  else {
    Serial.println("WiFi Disconnected");
  }
}
