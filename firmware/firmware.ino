#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
//#include <WiFiClient.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// LED pins
int redPin = D6;
int greenPin = D7;
int bluePin = D8;

// Initialize BME280
Adafruit_BME280 bme; // I2C

// Initialize the LCD. The address (0x27) and size (16, 2) may vary for your display
int lcdCols = 16;
int lcdRows = 2;
LiquidCrystal_I2C lcd(0x27, lcdCols, lcdRows);

const char* ssid = "ai_dinamica";
const char* password = "ai_dinamica";

unsigned long lastPostTime = 0;

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.println("Starting Arduino...");
  Serial.begin(9600);
  while (!Serial) {
      delay(10);
  }

  // Set RGB LED pins as output
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // 1. Initialize display
  lightOn("yellow");
  Serial.println("Initializing display...");
  lcd.init(); // Initialize the LCD
  lcd.backlight(); // Turn on the backlight
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello, there!");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(1000);

  // 2. Check BME280 sensor
  if (!bme.begin(0x76)) {
      clearLCDLine(1);
      lcd.print("Could not find BME280 sensor, check wiring!");
      Serial.println("Could not find BME280 sensor, check wiring!");
      lightOn("red");
      while (1);
  }

  clearLCDLine(1);
  lcd.setCursor(0, 1);
  lcd.print("WiFi Setup");
  delay(500);

  // 2. Connect to WiFi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
      lightOn("red");
      delay(500);
      clearLCDLine(1);
      lcd.setCursor(0, 1);
      loopMessage("Connecting to WiFi... ", 10);
      Serial.println("Connecting to WiFi...");
  }


  clearLCDLine(1);
  lcd.setCursor(0, 1);
  lcd.print("Connected!");
  Serial.print("Connected to WiFi: ");
  Serial.println(WiFi.localIP());
  lightOn("green");

  delay(5000);
  lightOn("blue");
}

void loop() {

  unsigned long currentTime = millis();
  if (currentTime - lastPostTime > 10000 or lastPostTime == 0) {
    lastPostTime = currentTime;

    // Check measurements
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    // Display the BME280 data on the display
    displayData(temperature, humidity, pressure);

    // Post BME280 data to the endpoint
    postData(temperature, humidity, pressure);
  }
}

void displayData(float t, float h, float p) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(t);
  lcd.setCursor(5, 0);
  lcd.print("C");

  lcd.setCursor(10, 0);
  lcd.print(h);
  lcd.setCursor(15, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print(p);
  lcd.setCursor(7, 1);
  lcd.print("hPa");
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

    // Delay to control the speed of scrolling
    delay(500); // Adjust delay as needed

    // Clear the part of the LCD where the text is displayed to prevent ghosting
    clearLCDLine(1);
  }
}

void postData(float temperature, float humidity, float pressure) {
    Serial.print("Data Sent! ");
    HTTPClient http;
    http.begin(client, "http://192.168.43.137:5000/bme280");
    http.addHeader("Content-Type", "application/json");
    String postData = String("{\"temperature\":") + temperature + ",\"humidity\":" + humidity + ",\"pressure\":" + pressure + "}";
    int httpCode = http.POST(postData);
    Serial.println(httpCode);
    http.end();
}

// void getData() {
//     HTTPClient http;
//     http.begin(client, "http://192.168.43.137:5000/bme280");
//     int httpCode = http.GET();
//     Serial.println(httpCode);
//     Serial.println(http.getString());
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

void lightOn(String color) {
  if (color == "red") {
    setColor(255, 0, 0);
  }
  else if (color == "green") {
    setColor(0, 255, 0);
  }
  else if (color == "blue") {
    setColor(0, 0, 255);
  }
  else if (color == "yellow") {
    setColor(255, 235, 0);
  }
}

void lightOff() {
  setColor(0, 0, 0);
}

// Function to set color
void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

