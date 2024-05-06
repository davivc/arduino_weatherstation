#include <Wire.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Pin Definitions
// #define LED_RED D6
// #define LED_YELLOW D8
// #define LED_GREEN D10
// LED pins
const int ledRed = D6;
const int ledYellow = D7;
const int ledGreen = D8;

// Initialize BME280
Adafruit_BME280 bme; // I2C

// Initialize the LCD. The address (0x27) and size (16, 2) may vary for your display
int lcdCols = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdCols, lcdRows);

const char* ssid = "ai_dinamica";
const char* password = "ai_dinamica";

unsigned long lastPostTime = 0;
unsigned long lastWeatherTime = 0;
bool showPressure = true;
String forecast = "";

void setup() {
    Serial.begin(9600);
    while (!Serial) {
        delay(10);
    }

    // 1. Initialize display
    lcd.init(); // Initialize the LCD
    lcd.backlight(); // Turn on the backlight
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hello, there!");
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    delay(1000);

    // ledY_Off();
    // ledG_Off();

    // 2. Check BME280 sensor
    if (!bme.begin(0x76)) {
        clearLCDLine(1);
        lcd.print("Could not find BME280 sensor, check wiring!");
        Serial.println("Could not find BME280 sensor, check wiring!");
        // ledR_On();
        while (1);
    }

    // ledR_Off();

    // 0. Start LEDS
    // pinMode(ledGreen, OUTPUT);
    // pinMode(ledRed, OUTPUT);
    // pinMode(ledYellow, OUTPUT);
    // ledR_On();
    // ledY_On();
    // ledG_On();

    clearLCDLine(1);
    lcd.setCursor(0, 1);
    lcd.print("WiFi Setup");
    // ledY_On();
    delay(500);

    // 2. Connect to WiFi
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        clearLCDLine(1);
        lcd.setCursor(0, 1);
        loopMessage("Connecting to WiFi... ", 10);
        Serial.println("Connecting to WiFi...");
    }

    delay(500);
    // ledY_On();
    // ledG_On();

    clearLCDLine(1);
    lcd.setCursor(0, 1);
    loopMessage("Connected to WiFi! ");
    Serial.println("Connected to WiFi");

    delay(2000);
    // ledG_Off();
}

void loop() {

  // digitalWrite(ledGreen, HIGH);

  unsigned long currentTime = millis();
  if (currentTime - lastPostTime > 10000 or lastPostTime == 0) {
    lastPostTime = currentTime;

    // Check measurements
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;
    
    // Get weather API
    updateWeatherForecast();

    // Print data to serial
    Serial.print("Temp: ");
    Serial.print(temperature);
    Serial.print("C");

    Serial.print(" | Hum: ");
    Serial.print(humidity);
    Serial.print("%");

    Serial.print(" | Pressure: ");
    Serial.print(pressure);
    Serial.println(" hPa");

  //   // Display the BME280 data on the display
  //   displayData(temperature, humidity, pressure, showPressure, forecast);

  //   // Post BME280 data to the endpoint
  //   // postData(temperature, humidity, pressure);
    
  //   // Regularly consume an endpoint to check if it's going to rain
  //   // checkRain();

  //   showPressure = !showPressure;     
  }
}

void displayData(float t, float h, float p, bool showP, String phrase) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(t);
  lcd.setCursor(5, 0);
  lcd.print("C");

  lcd.setCursor(10, 0);
  lcd.print(h);
  lcd.setCursor(15, 0);
  lcd.print("%");

  if (showP) {
    lcd.setCursor(0, 1);
    lcd.print(p);
    lcd.setCursor(7, 1);
    lcd.print("hPa");
    Serial.println(p);
    // delay(5000); // Adjust delay as needed
  }
  else {
    loopMessage(phrase, 20);    
  }
}

void loopMessage(String phrase, int loop) {
  int len = phrase.length();
  int displayWidth = 16; // The number of columns in the LCD

  for (int offset = 0; offset < 20; offset++) { // Infinite loop
    // Calculate the start and end indices for the substring
    int startIdx = offset % len;
    int endIdx = startIdx + displayWidth;
    String toDisplay;

    if (endIdx <= len) {
      // No wrap-around needed
      toDisplay = phrase.substring(startIdx, endIdx);
    } else {
      // Wrap around the end of the phrase to the beginning
      toDisplay = phrase.substring(startIdx, len) + phrase.substring(0, endIdx % len);
    }

    // Set the cursor position
    lcd.setCursor(0, 1); // Assuming you want to display it on the second row

    // Print the substring on LCD and Serial
    lcd.print(toDisplay);
    Serial.println(toDisplay);

    // Delay to control the speed of scrolling
    delay(500); // Adjust delay as needed

    // Clear the part of the LCD where the text is displayed to prevent ghosting
    clearLCDLine(1);
  }
}

// void postData(float temperature, float humidity, float pressure) {
//     HTTPClient http;
//     http.begin("YOUR_ENDPOINT");
//     http.addHeader("Content-Type", "application/json");
//     String postData = String("{\"temperature\":") + temperature + ",\"humidity\":" + humidity + ",\"pressure\":" + pressure + "}";
//     int httpCode = http.POST(postData);
//     http.end();
// }

// void checkRain() {
//     HTTPClient http;
//     http.begin("YOUR_RAIN_CHECK_ENDPOINT");
//     int httpCode = http.GET();
//     if (httpCode > 0) {
//         String payload = http.getString();
//         if (payload == "true") {
//         // Speak that the rain is coming
//         // ...
//         }
//     }
//     http.end();
// }

void updateWeatherForecast() {
  unsigned long currentTime = millis();
  Serial.println("updateWeatherForecast");
  if (currentTime - lastWeatherTime > 60000 or lastWeatherTime == 0) {
    lastWeatherTime = currentTime;
    Serial.print("checking");
    Serial.println("checking");
    HTTPClient http;
    WiFiClient client;
    http.begin(client, "https://api.openweathermap.org/data/2.5/weather?lat=-27.59667&lon=-48.54917&appid=f2762fddd7af542829a8c2883fb7a913");
    int httpCode = http.GET();
    if (httpCode > 0) {
      String payload = http.getString();
      if (payload == "true") {
        String payload = http.getString();
        Serial.println("Received payload:");
        Serial.println(payload);
        parseJson(payload);
      }
    } else {
      clearLCDLine(1);
      lcd.setCursor(0, 1);
      loopMessage("Error on HTTP request", 20);
      Serial.println("Error on HTTP request");
    }
    http.end();
  }
}

void parseJson(String payload) {
  StaticJsonDocument<1024> doc; // Adjust size according to your JSON payload
  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  // Extract values
  // Example: assuming your JSON has a structure like {"temperature": 22.5}
  float temperature = doc["temperature"];
  Serial.print("Temperature: ");
  Serial.println(temperature);
}

void clearLCDLine(int line) {
  // Clear the line
  lcd.setCursor(0, line); // Set cursor to the beginning of the first line
  for(int i = 0; i < lcdCols; i++) {
    lcd.print(" "); // Overwrite the line with spaces
  }
}

// void ledG_On() {
//   digitalWrite(ledGreen, HIGH);
// }

// void ledG_Off() {
//   digitalWrite(ledGreen, LOW);
// }

// void ledR_On() {
//   digitalWrite(ledRed, HIGH);
// }

// void ledR_Off() {
//   digitalWrite(ledRed, LOW);
// }

// void ledY_On() {
//   digitalWrite(ledYellow, HIGH);
// }

// void ledY_Off() {
//   digitalWrite(ledYellow, LOW);
// }

