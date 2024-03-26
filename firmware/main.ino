#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Initialize BME280
#define BME_SDA 2
#define BME_SCL 3
Adafruit_BME280 bme; // I2C

// Sound Sensor Pin
#define SOUND_SENSOR_PIN A0

// Initialize the LCD. The address (0x27) and size (16, 2) may vary for your display
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

unsigned long lastPostTime = 0;

void setup() {
    Serial.begin(9600);
    delay(100);

    // 1. Check BME280 sensor
    if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        while (1);
    }

    // 2. Connect to WiFi
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    Serial.println("Connected to WiFi");

    // 3. Initialize sound sensor
    pinMode(SOUND_SENSOR_PIN, INPUT);


    // 4. Initialize display
    lcd.init();
    lcd.backlight();
}

void loop() {
    float temperature = bme.readTemperature();
    float humidity = bme.readHumidity();
    float pressure = bme.readPressure() / 100.0F;

    // Display the BME280 data on the display
    displayData(temperature, humidity, pressure);

    // If a clapping sound is detected, speak the current temperature, humidity, and pressure
    // ...

    unsigned long currentTime = millis();
    if (currentTime - lastPostTime > 60000) {
        lastPostTime = currentTime;

        // Post BME280 data to the endpoint
        postData(temperature, humidity, pressure);
        // Regularly consume an endpoint to check if it's going to rain
        checkRain();        
    }
}

void displayData(float temperature, float humidity, float pressure) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(temperature);
    lcd.print(" C");

    lcd.setCursor(0, 1);
    lcd.print("Hum: ");
    lcd.print(humidity);
    lcd.print("%");

    // Due to space constraints, you might need to cycle the pressure reading separately or display it based on an event
}

void postData(float temperature, float humidity, float pressure) {
    HTTPClient http;
    http.begin("YOUR_ENDPOINT");
    http.addHeader("Content-Type", "application/json");
    String postData = String("{\"temperature\":") + temperature + ",\"humidity\":" + humidity + ",\"pressure\":" + pressure + "}";
    int httpCode = http.POST(postData);
    http.end();
}

void checkRain() {
    HTTPClient http;
    http.begin("YOUR_RAIN_CHECK_ENDPOINT");
    int httpCode = http.GET();
    if (httpCode > 0) {
        String payload = http.getString();
        if (payload == "true") {
        // Speak that the rain is coming
        // ...
        }
    }
    http.end();
}