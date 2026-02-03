# Smart IoT Oil & Gas Monitoring Node (ESP32-C3)

This repository contains a refinery-grade IoT edge node Proof of Concept (POC) designed for the Oil & Gas industry. 
It features geofenced thermal monitoring for critical refinery stations, a cinematic onboarding tutorial, and a robust fail-safe mechanism.

## 📸 Production Gallery

| Hardware Overview | Pinout & Interface |
| :---: | :---: |
| ![Hardware Overview](images/hardware_overview.png) | ![Pinout Diagram](images/pinout_diagram.png) |

| Live Monitoring State | Fail-Safe (Demo) State |
| :---: | :---: |
| ![Live Data](images/live_data_view.jpeg) | ![Demo Mode](images/demo_mode_view.jpeg) |

---

## 🛠 Project Overview
This node monitors ambient thermal conditions for specific refinery coordinates in the **Ruwais Industrial City**. It is designed to demonstrate **Tier-2 Redundancy**: if the local WiFi (`xeroxprinter`) is unavailable, the system automatically pivots from cloud-synchronized data to pre-defined industrial baselines.

### Key Features
* **Geofenced Data Acquisition:** Hardcoded coordinates for **Oil Refinery Stations 405, 406, and 407**.
* **Automatic Failover:** Real-time transition between **LIVE DATA** (API-driven) and **DEMO MODE** (Fallback-driven).
* **Industrial Safety Alerts:** Critical threshold set at **90°C** triggers a high-intensity **RED strobe** via GPIO 8.
* **Cinematic Onboarding:** A pixel-buffer rolling boot manual explaining system operations.

## 🏗 Hardware Details
* **MCU:** ESP32-C3 (RISC-V Architecture).
* **Display:** 0.42" SSD1306 OLED (72x40 visible window).
* **Indicator:** Onboard WS2812B RGB LED.
* **Buttons:**
    * **LEFT Button (BOOT/GPIO 9):** Skip tutorial / Cycle refinery stations.
    * **RIGHT Button (RST):** Hard hardware reboot.

## 🚀 Getting Started
1. **Clone the Repo:** `git clone https://github.com/assix/oil-gas-refinery-smart-iot-monitoring-esp32c3oled.git`
2. **Open in Arduino IDE:** Open the `src/RefineryGuardian/RefineryGuardian.ino` file.
3. **Install Dependencies:** `U8g2lib`, `ArduinoJson`, `Adafruit_NeoPixel`.
4. **Flash:** Target your ESP32-C3 Dev Module.
