#include <EEPROM.h> // EEPROM library
#include <Wire.h>   // I2C for communication
#include <Adafruit_MPU6050.h> // Adafruit MPU6050 library
#include <Adafruit_Sensor.h>  // Base sensor class

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

void setup() {
  // Start serial communication
  Serial.begin(9600);
  delay(1500);

  // Read calibration values from EEPROM
  flexMin = EEPROM.read(minAddress) | (EEPROM.read(minAddress + 1) << 8);
  flexMax = EEPROM.read(maxAddress) | (EEPROM.read(maxAddress + 1) << 8);

  // Check for valid calibration values
  if (flexMin == 0xFFFF || flexMax == 0xFFFF || flexMin >= flexMax) {
    Serial.println("No valid calibration found, restarting...");
    delay(5000); // Wait before recalibration
    calibrateSensors();
  } else {
    Serial.print("Calibration loaded from EEPROM! Min: ");
    Serial.print(flexMin);
    Serial.print(", Max: ");
    Serial.println(flexMax);
  }

  // Initialize MPU6050
  while (!Serial)
    delay(10); // for Leonardo/Micro only

  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 sensor.");
    while (1)
      delay(10);
  }

  // Set accelerometer range to ±2g
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);

  // Set gyroscope range to ±250 degrees per second
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);

  // Set filter bandwidth
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  Serial.println("MPU6050 initialized");
}

void loop() {
  // Read flex sensor data and map to a range
  int rawThumb = analogRead(flex1);
  int rawPointer = analogRead(flex2);
  int rawMiddle = analogRead(flex3);

  // Map values from raw data to a range (0-1000)
  int thumb = map(rawThumb, flexMin, flexMax, 0, 1000);
  int pointer = map(rawPointer, flexMin, flexMax, 0, 1000);
  int middle = map(rawMiddle, flexMin, flexMax, 0, 1000);

  // Constrain the values to the range [0, 1000]
  thumb = constrain(thumb, 0, 1000);
  pointer = constrain(pointer, 0, 1000);
  middle = constrain(middle, 0, 1000);

  // Read MPU6050 data
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Print MPU6050 data
  Serial.print("Accel X: ");
  Serial.print(a.acceleration.x);
  Serial.print("\tAccel Y: ");
  Serial.print(a.acceleration.y);
  Serial.print("\tAccel Z: ");
  Serial.println(a.acceleration.z);

  Serial.print("Gyro X: ");
  Serial.print(g.gyro.x);
  Serial.print("\tGyro Y: ");
  Serial.print(g.gyro.y);
  Serial.print("\tGyro Z: ");
  Serial.println(g.gyro.z);

  // Print flex sensor data
  Serial.print("Thumb: ");
  Serial.print(thumb);
  Serial.print("\tPointer: ");
  Serial.print(pointer);
  Serial.print("\tMiddle: ");
  Serial.println(middle);

  // Gesture detection using flex sensors and MPU6050
  if (thumb == 1000 && pointer == 0 && middle == 0) {
    Serial.println("Thumbs Up");
  }
  else if ((thumb <= 800 && thumb >= 600) && pointer == 0 && middle == 0) {
  Serial.println("call me");
  }
  else if (thumb >= 800 && pointer >= 800 && middle == 0) {
    Serial.println("I Love You");
  }
  else if ( a.acceleration.x >= -2 && thumb == 0 && pointer == 0 && middle == 0) {
    Serial.println("Yes");
  }
  else if (a.acceleration.y < 2 && thumb == 0 && (pointer >= 400 && pointer <= 800) && middle == 0) {   
    Serial.println("Time");
  }
  else if (a.acceleration.z > 0 && thumb <= 400 && pointer >= 800 && middle >= 800) {
  Serial.println("Victory");
  }
  else if (a.acceleration.z < 0 && thumb == 0 && pointer >= 600 && (middle >= 400 && middle <= 800)) {
    Serial.println("See");
  }
  else if (a.acceleration.y < 10.00 && a.acceleration.y > 4.00 && a.acceleration.z < 1.00 && a.acceleration.z > -10.00 && thumb >= 600 && pointer >= 600 && middle >= 600) {
    Serial.println("Thank You");
  }
  else if (a.acceleration.x < -5 && a.acceleration.x > -11 && thumb == 0 && pointer == 0 && middle == 0) {
    Serial.println("Sorry");
}

  else {
    Serial.println("...");
  }

  delay(3000); // Wait for 2 seconds
}

// Calibration function
void calibrateSensors() {
  Serial.println("Flex sensor calibration starting...");
  Serial.println("Bend the sensors to their minimum and maximum positions.");
  delay(5000);

  // Calibration loop
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

  Serial.print("New calibration completed! Min: ");
  Serial.print(flexMin);
  Serial.print(", Max: ");
  Serial.println(flexMax);

  // Save calibration to EEPROM
  EEPROM.write(minAddress, flexMin & 0xFF);
  EEPROM.write(minAddress + 1, (flexMin >> 8) & 0xFF);
  EEPROM.write(maxAddress, flexMax & 0xFF);
  EEPROM.write(maxAddress + 1, (flexMax >> 8) & 0xFF);

  Serial.println("Calibration saved to EEPROM!");
}
