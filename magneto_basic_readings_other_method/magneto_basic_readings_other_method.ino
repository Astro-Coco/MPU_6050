#include <Wire.h>
#include <MPU6050.h> // MPU6050 library
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h> // Magnetometer library

MPU6050 mpu; // Create an instance of the MPU6050 class
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345); // Create an instance of the magnetometer class

void setup() {
  Wire.begin();
  Serial.begin(9600);
  
  // Initialize MPU6050
  mpu.initialize();
  
  // Initialize magnetometer
  if(!mag.begin()) {
    Serial.println("Could not find a valid HMC5883L sensor, check wiring!");
    while(1);
  }
}

void loop() {
  // Read MPU6050 data
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);
  
  // Read magnetometer data
  sensors_event_t event;
  mag.getEvent(&event);

  // Print MPU6050 data
  Serial.print("Accel: ");
  Serial.print(ax);
  Serial.print(", ");
  Serial.print(ay);
  Serial.print(", ");
  Serial.println(az);
  
  // Print magnetometer data
  Serial.print("Magnetometer: ");
  Serial.print(event.magnetic.x);
  Serial.print(", ");
  Serial.print(event.magnetic.y);
  Serial.print(", ");
  Serial.println(event.magnetic.z);
  
  delay(500);
}