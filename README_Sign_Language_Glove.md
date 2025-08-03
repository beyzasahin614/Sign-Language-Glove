# Sign Language Glove 🤟

A wearable smart glove that recognizes predefined sign language gestures using flex sensors and a gyroscope, and transmits the interpreted message via Bluetooth.

---

## 📌 Overview

This project aims to help individuals with hearing or speech impairments to communicate using a wearable device that converts certain hand gestures into text-based messages. The glove is built using **flex sensors**, an **MPU6050 gyroscope**, and an **Arduino Nano**, and it communicates through **Bluetooth (HC-05)**.

---

## 🛠️ Hardware Components

- **Arduino Nano**
- **3× Flex Sensors** (connected to A2, A6, A7)
- **MPU6050 Gyroscope/Accelerometer** (via I2C)
- **HC-05 Bluetooth Module** (D7 → RX, D8 → TX using `SoftwareSerial`)
- **Resistors, jumper wires, breadboard**
- **Battery Pack (optional for portability)**

---

## 💻 Software & Libraries

- **Arduino IDE**
- `EEPROM.h`
- `Wire.h`
- `Adafruit_MPU6050.h`
- `Adafruit_Sensor.h`
- `SoftwareSerial.h`

---

## ⚙️ How It Works

1. **Calibration Phase**:  
   On first boot (or if no EEPROM calibration data is found), the glove calibrates flex sensor values and stores min/max to EEPROM.

2. **Sensor Data Reading**:  
   - **Flex Sensors** measure finger bends (thumb, pointer, middle).
   - **MPU6050** provides x/y/z acceleration values for wrist orientation.

3. **Gesture Recognition**:  
   The glove maps sensor thresholds and orientation to predefined gestures like:
   - `Thumbs Up`
   - `I Love You`
   - `Thank You`
   - `Time`
   - `Victory`
   - `Sorry`
   - `One`, etc.

4. **Communication**:  
   Recognized gestures are sent over Bluetooth to a paired device using the HC-05 module.

---

## 🧠 Example Gestures (Mapped in Code)

| Gesture      | Conditions (Simplified)                                |
|--------------|--------------------------------------------------------|
| Thumbs Up    | thumb=1000, pointer=0, middle=0                        |
| Correct      | thumb=0, pointer=0, middle≥800                         |
| I Love You   | thumb≥800, pointer≥800, middle=0                      |
| Yes          | ax ≥ -2, all fingers=0                                 |
| Time         | ay < 2, pointer=400–800, others=0                     |
| Victory      | az > 0, pointer≥800, middle≥800, thumb≤400            |
| Thank You    | all fingers ≥600, ay=4–10, az=-10 to 1                 |
| Sorry        | ax = -11 to -5, all fingers=0                          |
| One          | pointer=1000, others=0                                 |

---

## 🚀 Setup Instructions

### 1. Circuit Connections
- Flex sensors: A2, A6, A7 (analog)
- MPU6050: I2C → A4 (SDA), A5 (SCL)
- HC-05: RX to D7, TX to D8 (use voltage divider if needed)

### 2. Upload the Code
- Open `Sign_Language_Glove.ino` in Arduino IDE.
- Select **Arduino Nano** board.
- Install required libraries (via Library Manager).
- Upload the sketch to the board.

### 3. Bluetooth Pairing
- Pair HC-05 with your phone or PC (default password usually `1234` or `0000`).
- Open a serial terminal to receive messages.

---

## 📈 Calibration

If no calibration data is found in EEPROM, the glove enters calibration mode:

- Keep the hand flat and relaxed for 5 seconds.
- Flex sensor min/max values are stored.
- You will see a message: `Calibration saved!`

---

## 🧩 Future Improvements

- Add more flex sensors for full ASL alphabet
- Real-time dynamic gesture recognition using ML
- Integrate speech output (Text-to-Speech)
- Develop a mobile companion app

---

## 👩‍💻 Author

**Beyza Şahin**  
📧 beyzasahin6104@gmail.com  
🔗 [GitHub](https://github.com/beyzasahin614)  
🔗 [LinkedIn](https://www.linkedin.com/in/beyza-şahin-25b10b1ba)

---

> *This project combines wearable tech, embedded systems, and assistive AI to improve accessibility and inclusion.*
