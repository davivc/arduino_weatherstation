#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>

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

const char* ssid = "MALOCA";
const char* password = "jardimsecreto";

unsigned long lastPostTime = 0;
bool showPressure = false;

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
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledYellow, OUTPUT);
    ledR_On();
    ledY_On();
    ledG_On();

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
        lcd.print("Connecting to WiFi...");
        Serial.println("Connecting to WiFi...");
    }

    delay(500);
    // ledY_On();
    // ledG_On();

    clearLCDLine(1);
    lcd.setCursor(0, 1);
    lcd.print("Connected to WiFi");
    Serial.println("Connected to WiFi");

    delay(3000);
    // ledG_Off();
}

void loop() {

    digitalWrite(ledGreen, HIGH);
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    unsigned long currentTime = millis();
    if (currentTime - lastPostTime > 30000 or lastPostTime == 0) {
        // Get weather API
        String message = "Nublado com chuvas! ";

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
  
        lastPostTime = currentTime;

        // Display the BME280 data on the display
        displayData(temperature, humidity, pressure, showPressure, message);

        // Post BME280 data to the endpoint
        // postData(temperature, humidity, pressure);
        
        // Regularly consume an endpoint to check if it's going to rain
        // checkRain();

        showPressure = !showPressure;     
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
        clearLCDLine(1);
        lcd.setCursor(0, 1);
        lcd.print(p);
        lcd.setCursor(7, 1);
        lcd.print("hPa");
        Serial.println(p);
        // delay(5000); // Adjust delay as needed
    }
    else {
        int len = phrase.length();
        int displayWidth = 16; // The number of columns in the LCD

        for (int offset = 0; offset < 10; offset++) { // Infinite loop
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

void clearLCDLine(int line) {
    // Clear the line
    lcd.setCursor(0, line); // Set cursor to the beginning of the first line
    for(int i = 0; i < lcdCols; i++) {
        lcd.print(" "); // Overwrite the line with spaces
    }
}

void ledG_On() {
  digitalWrite(ledGreen, HIGH);
}

void ledG_Off() {
  digitalWrite(ledGreen, LOW);
}

void ledR_On() {
  digitalWrite(ledRed, HIGH);
}

void ledR_Off() {
  digitalWrite(ledRed, LOW);
}

void ledY_On() {
  digitalWrite(ledYellow, HIGH);
}

void ledY_Off() {
  digitalWrite(ledYellow, LOW);
}

