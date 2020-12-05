#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// include WIFI, SD and SPI libraries
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>

#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define TFT_CS 15 // Chip select line for TFT display
#define TFT_RST 4 // Reset line for TFT
#define TFT_DC 2 // Data/command line for TFT

#define SD_CS 16 // Chip select line for SD card

//tft instance
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Wifi connection
const char* ssid = "SFR-7e70";
const char* password = "QGL93LWLU65T";

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "91b651a6cfe609c9745b222d8bf25ccc";

// Replace with your country code and city
String city = "Paris";
String countryCode = "FR";

// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;

String jsonBuffer;

void prepareText(char *text , uint16_t color) {
  tft.setTextColor(color);
  tft.setTextWrap(true); //automatischer Zeilenumbruch wird aktiviert
  tft.println(text);
}
void displayText(uint16_t x, uint16_t y, char *text , uint16_t color) {
  tft.setCursor(x, y);
  prepareText(text,color);
}

void setup() {
  Serial.begin(115200);

  // Init screen
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);
  tft.fillScreen(ST7735_BLACK);

  // Init wifi
  WiFi.begin(ssid, password);
  
  Serial.println("Connecting");
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  
  Serial.print("Connected to WiFi network with IP Address: ");
  displayText(20,0, "Connected to WiFi network with IP Address: ", ST7735_WHITE);
  Serial.println(WiFi.localIP());
  tft.println(WiFi.localIP());
  
  Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");
  displayText(0,30, "Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.", ST7735_WHITE);

}


void loop() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        displayText(20,60,"Parsing input failed!",ST7735_RED);
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);

      tft.fillScreen(ST7735_BLACK);
      displayText(30,0,"BABY BOX",ST7735_WHITE);
      
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      displayText(0,10,"Temperature: ",ST7735_YELLOW);
      tft.println(myObject["main"]["temp"]);
      
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      displayText(0,40,"Pression: ",ST7735_YELLOW);
      tft.println(myObject["main"]["pressure"]);
      
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      displayText(0,70,"Humidite: ",ST7735_YELLOW);
      tft.println(myObject["main"]["humidity"]);
      
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      displayText(0,100,"Vitesse vent: ",ST7735_YELLOW);
      tft.println(myObject["main"]["speed"]);
    }
    else {
      Serial.println("WiFi Disconnected");
      tft.fillScreen(ST7735_BLACK);
      displayText(10,10,"WiFi Disconnected",ST7735_RED);
    }
    lastTime = millis();
  }
  }
  
  String httpGETRequest(const char* serverName) {
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
  
  return payload;
 }
