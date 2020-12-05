#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
//Einbindung von Font Bibliotheken
#include <Fonts/FreeSans9pt7b.h> //Font für Logo
#include <Fonts/FreeSansBold9pt7b.h> //Font für Logo

#include <SPI.h>
#include <SD.h>
#include <WiFi.h>

#include <HTTPClient.h>
#include <Arduino_JSON.h>

//Definition der verwendeten Pins
#define TFT_CS 15 // Chip select line for TFT display
#define TFT_RST 4 // Reset line for TFT
#define TFT_DC 2 // Data/command line for TFT

#define SD_CS 16 // Chip select line for SD card

const char* ssid = "SFR-7e70";
const char* password = "QGL93LWLU65T";

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "91b651a6cfe609c9745b222d8bf25ccc";
// Example:
//String openWeatherMapApiKey = "bd939aa3d23ff33d3c8f5dd1dd435";

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
//tft instance
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

//Funktion zum Anzeigen eines Textes am Display in einer bestimmten Farbe
void displayText(char *text , uint16_t color) {
tft.setTextColor(color);
tft.setTextWrap(true); //automatischer Zeilenumbruch wird aktiviert
tft.print(text);
}

//Funktion zum Anzeigen eines Textes an einem beliebigen Punkt (x,y) auf
//dem Display. Die Farbe kann gewählt werden
void displayText(uint16_t x, uint16_t y, char *text , uint16_t color) {
tft.setCursor(x, y);
displayText(text,color);
}

//Funktion zum Anzeigen des Logos ganz oben am Display
void displayLogo() {
//Hintergrund füllen
tft.fillRect(0,0,127,30,ST7735_YELLOW);
//Rahmen zeichnen
tft.drawRect(1,1,125,28,ST7735_BLUE);
//Font für das Wort HOME fett
tft.setFont(&FreeSansBold9pt7b);
//Cursor positionieren
tft.setCursor(7,20);
//Text in schwarz ausgeben
tft.setTextColor(ST7735_BLACK);
tft.print("BABY");
//Font für das Wort Control nicht fett
tft.setFont(&FreeSans9pt7b);
//Text in rot ausgeben
tft.setTextColor(ST7735_RED);
tft.print("Box");
//Font auf default zurücksetzen
tft.setFont(NULL);
}

void setup() {
Serial.begin(115200);

WiFi.begin(ssid, password);
Serial.println("Connecting");
while(WiFi.status() != WL_CONNECTED) {
  delay(500);
  Serial.print(".");
}
Serial.println("");
Serial.print("Connected to WiFi network with IP Address: ");
Serial.println(WiFi.localIP());
Serial.println("Timer set to 10 seconds (timerDelay variable), it will take 10 seconds before publishing the first reading.");

//tft initialisieren und schwarzer Hintergrund
tft.initR(INITR_BLACKTAB);
tft.fillScreen(ST7735_BLACK);
//Logo anzeigen
displayLogo();
//SD-Karte initialisieren und Ergebnis anzeigen
if (!SD.begin(SD_CS)) {
displayText(20,60,"No SD-Card",ST7735_YELLOW);
} else {
displayText(20,60,"SD-Card OK",ST7735_GREEN);
}

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
      return;
    }
  
    Serial.print("JSON object = ");
    Serial.println(myObject);
    Serial.print("Temperature: ");
    displayText(20,60,"Température: ",ST7735_YELLOW);
    Serial.println(myObject["main"]["temp"]);
    Serial.print("Pressure: ");
    Serial.println(myObject["main"]["pressure"]);
    Serial.print("Humidity: ");
    Serial.println(myObject["main"]["humidity"]);
    Serial.print("Wind Speed: ");
    Serial.println(myObject["wind"]["speed"]);
  }
  else {
    Serial.println("WiFi Disconnected");
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
