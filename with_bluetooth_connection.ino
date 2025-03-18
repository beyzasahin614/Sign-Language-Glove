#include <EEPROM.h> // EEPROM library
#include <Wire.h>   // I2C for communication
#include <Adafruit_MPU6050.h> // Adafruit MPU6050 library
#include <Adafruit_Sensor.h>  // Base sensor class
#include <SoftwareSerial.h>   // For Bluetooth communication

const int flex1 = A2;
const int flex2 = A7;
const int flex3 = A6;

// EEPROM addresses
const int minAddress = 0;
const int maxAddress = 4;

// Calibration values
int flexMin = 1023;
int flexMax = 0;

// MPU6050 object
Adafruit_MPU6050 mpu;

// Sensor data
float ax, ay, az;
float gx, gy, gz;

// Bluetooth configuration (HC-05 connected to D7 (RX), D8 (TX))
SoftwareSerial bluetooth(7, 8);

void setup() {
  // Start serial communication
  Serial.begin(9600);
  bluetooth.begin(9600); // Initialize Bluetooth communication
  
  delay(1500);

  // Read calibration values from EEPROM
  flexMin = EEPROM.read(minAddress) | (EEPROM.read(minAddress + 1) << 8);
  flexMax = EEPROM.read(maxAddress) | (EEPROM.read(maxAddress + 1) << 8);

  if (flexMin == 0xFFFF || flexMax == 0xFFFF || flexMin >= flexMax) {
    bluetooth.println("No valid calibration found, restarting...");
    calibrateSensors();
  } else {
    bluetooth.print("Calibration loaded! Min: ");
    bluetooth.print(flexMin);
    bluetooth.print(", Max: ");
    bluetooth.println(flexMax);
  }

  if (!mpu.begin()) {
    bluetooth.println("Failed to find MPU6050 sensor.");
    while (1) delay(10);
  }

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  bluetooth.println("MPU6050 initialized");
}

void loop() {
  int rawThumb = analogRead(flex1);
  int rawPointer = analogRead(flex2);
  int rawMiddle = analogRead(flex3);

  int thumb = map(rawThumb, flexMin, flexMax, 0, 1000);
  int pointer = map(rawPointer, flexMin, flexMax, 0, 1000);
  int middle = map(rawMiddle, flexMin, flexMax, 0, 1000);

  thumb = constrain(thumb, 0, 1000);
  pointer = constrain(pointer, 0, 1000);
  middle = constrain(middle, 0, 1000);

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  String gesture = "";

  if (thumb == 1000 && pointer == 0 && middle == 0) {
    gesture = "Thumbs Up";
  } else if (thumb == 0 && pointer == 0 && middle >=800) {
    gesture = "Correct";
  } else if (thumb >= 800 && pointer >= 800 && middle == 0) {
    gesture = "I Love You";
  } else if (a.acceleration.x >= -2 && thumb == 0 && pointer == 0 && middle == 0) {
    gesture = "Yes";
  } else if (a.acceleration.y < 2 && thumb == 0 && (pointer >= 400 && pointer <= 800) && middle == 0) {
    gesture = "Time";
  } else if (a.acceleration.z > 0 && thumb <= 400 && pointer >= 800 && middle >= 800) {
    gesture = "Victory";
  } else if (a.acceleration.z < 0 && thumb == 0 && pointer >= 600 && (middle >= 400 && middle <= 800)) {
    Serial.println("See");
  } else if (a.acceleration.y < 10.00 && a.acceleration.y > 4.00 && a.acceleration.z < 1.00 && a.acceleration.z > -10.00 && thumb >= 600 && pointer >= 600 && middle >= 600) {
    gesture = "Thank You";
  } else if (a.acceleration.x < -5 && a.acceleration.x > -11 && thumb == 0 && pointer == 0 && middle == 0) {
    gesture = "Sorry";
  } else if ( thumb <=200 && pointer == 1000 && middle ==0) {
    gesture = "One";
  }
  if (gesture != "") {
    bluetooth.println("Gesture Detected: " + gesture);
  }

  delay(3000);
}


void calibrateSensors() {
  bluetooth.println("Flex sensor calibration starting...");
  delay(5000);

  flexMin = 1023;
  flexMax = 0;
  for (int i = 0; i < 100; i++) {
    int thumb = analogRead(flex1);
    int pointer = analogRead(flex2);
    int middle = analogRead(flex3);

    if (thumb < flexMin) flexMin = thumb;
    if (pointer < flexMin) flexMin = pointer;
    if (middle < flexMin) flexMin = middle;

    if (thumb > flexMax) flexMax = thumb;
    if (pointer > flexMax) flexMax = pointer;
    if (middle > flexMax) flexMax = middle;

    delay(50);
  }

  EEPROM.write(minAddress, flexMin & 0xFF);
  EEPROM.write(minAddress + 1, (flexMin >> 8) & 0xFF);
  EEPROM.write(maxAddress, flexMax & 0xFF);
  EEPROM.write(maxAddress + 1, (flexMax >> 8) & 0xFF);

  bluetooth.println("Calibration saved!");
}
