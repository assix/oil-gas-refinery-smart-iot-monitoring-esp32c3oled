#include <U8g2lib.h>
#include <Wire.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <esp_system.h>

// --- Configuration ---
const char* ssid = "WIFI_SSID_NAME";
const char* password = "WIFI_PASSWORD";
const float CRITICAL_TEMP = 90.0;

// Global Control: Set to true to bypass the tutorial automatically
bool skipBootManual = false; 

#define RGB_PIN 8
#define BOOT_BUTTON 9 // Left Button (Cycle/Skip)
#define NUM_LEDS 1

struct Refinery {
    String name;
    float lat;
    float lon;
    float fallbackTemp;
};

/* * DATA STRUCTURE EXPLANATION:
 * { "Display Name", Latitude, Longitude, Fallback Temp }
 */
Refinery stations[] = {
    {"STATION 405", 24.1246, 52.7199, 65.0}, 
    {"STATION 406", 24.1100, 52.7300, 66.0}, 
    {"STATION 407", 24.1300, 52.7000, 67.0}
};

int currentStationIdx = 0;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE, 6, 5);

// Perfect Screen Offsets
const int xOffset = 30;
const int yOffset = 14; 

Adafruit_NeoPixel pixels(NUM_LEDS, RGB_PIN, NEO_GRB + NEO_KHZ800);
float currentTemp = 0.0;
bool isAlarm = false;
bool lastFetchSuccessful = false;

const char* bootMsg[] = {
    "Welcome", "System Booting", "---",
    "Monitoring", "Oil & Gas", "Refineries", "---",
    "Capturing", "Live Data", "---",
    "Hold LEFT to", "SKIP TUTORIAL", "---",
    "RIGHT Button:", "Hard Reboot", "---",
    "LEFT Button:", "Cycle Station", "---",
    "Enjoy!", "----------------"
};

void setup() {
    Serial.begin(115200);
    pinMode(BOOT_BUTTON, INPUT_PULLUP);
    pixels.begin();
    u8g2.begin();
    u8g2.setContrast(255);
    
    // Check Global Override or Button Press to skip
    if (!skipBootManual) {
        rollingBootSequence();
    }
    
    // WiFi Sync Phase
    WiFi.begin(ssid, password);
    showStatus("WIFI SYNC...");
    
    int counter = 0;
    while (WiFi.status() != WL_CONNECTED && counter < 20) {
        delay(500);
        pixels.setPixelColor(0, pixels.Color(50, 50, 0)); // Yellow
        pixels.show();
        counter++;
    }
    
    updateData(); // Initial blocking fetch
}

void loop() {
    static bool lastBtnState = HIGH;
    bool currentBtnState = digitalRead(BOOT_BUTTON);

    // Left Button (GPIO 9) -> Manual Station Cycle
    if (currentBtnState == LOW && lastBtnState == HIGH) {
        currentStationIdx = (currentStationIdx + 1) % 3;
        updateData(); 
        delay(300); 
    }
    lastBtnState = currentBtnState;

    renderDisplay();
    handleAlarms();
}

void updateData() {
    lastFetchSuccessful = false;
    showStatus("FETCHING...");

    // Blocking retry logic to ensure connection
    for (int attempt = 0; attempt < 3; attempt++) {
        if (WiFi.status() == WL_CONNECTED) {
            pixels.setPixelColor(0, pixels.Color(0, 0, 150)); // Blue
            pixels.show();

            HTTPClient http;
            String url = "http://api.open-meteo.com/v1/forecast?latitude=" + String(stations[currentStationIdx].lat, 4) + 
                         "&longitude=" + String(stations[currentStationIdx].lon, 4) + "&current_weather=true";
            
            http.begin(url);
            int httpCode = http.GET();
            
            if (httpCode == 200) {
                StaticJsonDocument<768> doc;
                deserializeJson(doc, http.getString());
                currentTemp = doc["current_weather"]["temperature"];
                lastFetchSuccessful = true;
                http.end();
                break; 
            }
            http.end();
        }
        delay(1500); // Wait between retries
    }

    if (!lastFetchSuccessful) {
        currentTemp = stations[currentStationIdx].fallbackTemp;
    }
    isAlarm = (currentTemp >= CRITICAL_TEMP);
}

void renderDisplay() {
    u8g2.clearBuffer();
    
    // Line 1: Station Name
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.setCursor(xOffset, yOffset + 0); 
    u8g2.print(stations[currentStationIdx].name);

    // Line 2: Mode Status
    u8g2.setFont(u8g2_font_4x6_tr);
    u8g2.setCursor(xOffset, yOffset + 10);
    u8g2.print(lastFetchSuccessful ? "> LIVE DATA <" : "> DEMO MODE <");

    // Line 3: Temp Value
    u8g2.setFont(u8g2_font_profont12_tf); 
    u8g2.setCursor(xOffset, yOffset + 25);
    u8g2.print(currentTemp, 1); u8g2.print(" C");

    u8g2.sendBuffer();
}

void showStatus(String msg) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_5x7_tr);
    u8g2.setCursor(xOffset, yOffset + 15);
    u8g2.print(msg);
    u8g2.sendBuffer();
}

void rollingBootSequence() {
    int lineCount = 21;
    int lineHeight = 12; 
    int totalScrollHeight = (lineCount * lineHeight) + 60;

    for (int scrollPos = 0; scrollPos < totalScrollHeight; scrollPos++) {
        // Allow manual skip even if skipBootManual is false
        if (digitalRead(BOOT_BUTTON) == LOW) return; 

        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_5x7_tr);
        for (int i = 0; i < lineCount; i++) {
            int yPos = 30 + (i * lineHeight) - scrollPos;
            if (yPos > 0 && yPos < 45) {
                u8g2.setCursor(xOffset, yOffset + yPos);
                u8g2.print(bootMsg[i]);
            }
        }
        u8g2.sendBuffer();
        delay(75); 
    }
}

void handleAlarms() {
    if (isAlarm) {
        if (millis() % 400 < 200) pixels.setPixelColor(0, pixels.Color(200, 0, 0));
        else pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    } else {
        pixels.setPixelColor(0, lastFetchSuccessful ? pixels.Color(0, 10, 0) : pixels.Color(15, 5, 0));
    }
    pixels.show();
}