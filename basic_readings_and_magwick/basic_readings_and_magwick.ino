#include <Boards.h>
#include <Firmata.h>
#include <FirmataConstants.h>
#include <FirmataDefines.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <Fusion.h>
#include <stdbool.h>
#include <stdio.h>

Adafruit_MPU6050 mpu;

// Define FusionAHRS instance
FusionAhrs ahrs;

//ACCELEROMETER
float AccX, AccY, AccZ = 0.;


//GYROSCOPE
float GyroX, GyroY, GyroZ = 0.;
float angleX, angleY, angleZ = 0.;
float roll, pitch, yaw;
float total_gyro_x, total_gyro_y, total_gyro_z = 0.;

long compteur = 0;
//nombre de données prises
int iterations_gyro_calibration = 20000;


//Input mesured offsets here, note that these will be set to 0 if calibrate is set to true
float offset_gyroX = 0.010678; 
float offset_gyroY = 0.006555;
float offset_gyroZ = -0.021964;

bool calibrate = false;
bool show_offsets = true; //set to true if you want to program to end by showing offsets, else will calibrate and continue operation

float delais;
unsigned long last_angle_time;

void check_mpu_connection(){
  // Try to initialize
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
}

const FusionAhrsSettings settings = {
            .convention = FusionConventionNwu,
            .gain = 0.3f,
            .gyroscopeRange = 250.0f, /* replace this with actual gyroscope range in degrees/s */
            .accelerationRejection = 20.0f,
            .magneticRejection = 10.0f,
            .recoveryTriggerPeriod = 5 * 100, /* 5 seconds */ //j'ai mit un truc random
    };



void setup(void) {
  Serial.begin(115200);

  check_mpu_connection();

  set_correct_ranges_and_frequency();

  
  FusionAhrsInitialise(&ahrs);
  
  FusionAhrsSetSettings(&ahrs, &settings);
  

  if (calibrate){
    offset_gyroX = 0.;
    offset_gyroY = 0.;
    offset_gyroZ = 0.;
  }

  delay(4000);
}

void loop() {
  

  get_sensor_raw_data();

  gyro_offset_calibration();

  gyro_integration();

  
  print_data();
}

void gyro_integration(){
  unsigned long current_time = millis();
  unsigned long elapsed_milliseconds = current_time - last_angle_time;
  unsigned long elapsed_seconds = elapsed_milliseconds / 1000; // Convert milliseconds to seconds

  // Remainder in milliseconds
  unsigned long remainder_milliseconds = elapsed_milliseconds % 1000;

  // Convert remainder to fractional seconds with higher precision
  unsigned long precision_seconds = (remainder_milliseconds * 1000) / 1000;

  // Combine whole seconds and precision seconds
  float elapsed_time = elapsed_seconds + (float)precision_seconds / 1000.0;



  angleX += 57.29577951308232*GyroX*elapsed_time;
  angleY += 57.29577951308232*GyroY*elapsed_time;
  angleZ += 57.29577951308232*GyroZ*elapsed_time; //computed angles in degrees
  last_angle_time = millis();

  // Update the FusionAHRS with gyroscope and accelerometer data
  const FusionVector gyroscope = {57.29577951308232*GyroX, 57.29577951308232*GyroY, 57.29577951308232*GyroZ}; // Gyroscope data in degrees/s
  const FusionVector accelerometer = {AccX/9.81, AccY/9.81, AccZ/9.81}; // Accelerometer data in g
  FusionAhrsUpdateNoMagnetometer(&ahrs, gyroscope, accelerometer, elapsed_time);

  // Get the orientation angles
  const FusionEuler euler = FusionQuaternionToEuler(FusionAhrsGetQuaternion(&ahrs));

  // Assign the angles to variables
  roll = euler.angle.roll;
  pitch = euler.angle.pitch;
  yaw = euler.angle.yaw;
}

void print_data(){
  /* Print out the values */
  
  //Put the wanted prints in the if (1 block) and check formatting
  if (1){
    Serial.print("angleX:");
    Serial.print(angleX);
    Serial.print(",");
    Serial.print("angleY:");
    Serial.print(angleY);
    Serial.print(",");
    Serial.print("angleZ:");
    Serial.print(angleZ);

    Serial.print(",");
    Serial.print("roll:");
    Serial.print(roll);
    Serial.print(",");
    Serial.print("pitch:");
    Serial.print(pitch);
    Serial.print(",");
    Serial.print("yaw:");
    Serial.print(yaw);

    
    } else {

      Serial.print(",");
    Serial.print("GyroX:");
    Serial.print(GyroX);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(GyroY);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(GyroZ);


      Serial.print("AccX:");
      Serial.print(AccX);
      Serial.print(",");
      Serial.print("AccY:");
      Serial.print(AccY);
      Serial.print(",");
      Serial.print("AccZ:");
      Serial.print(AccZ);

      

      Serial.print(",");
 
  }

  Serial.println("");
}


void get_sensor_raw_data(){
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  AccX = a.acceleration.x;
  AccY = a.acceleration.y;
  AccZ = a.acceleration.z;

  GyroX = g.gyro.x - offset_gyroX;
  GyroY = g.gyro.y - offset_gyroY;
  GyroZ= g.gyro.z - offset_gyroZ;
}


void gyro_offset_calibration(){
  if ((compteur < iterations_gyro_calibration) && (calibrate)){
    total_gyro_x += GyroX;
    total_gyro_y += GyroY;
    total_gyro_z += GyroZ; // corrected accumulation
    compteur += 1;
  } else if ((compteur == iterations_gyro_calibration) && (calibrate)) {
    offset_gyroX = total_gyro_x / iterations_gyro_calibration;
    offset_gyroY = total_gyro_y / iterations_gyro_calibration;
    offset_gyroZ = total_gyro_z / iterations_gyro_calibration;
    compteur += 1;

    if (show_offsets){
      Serial.print("Offsets, x, y, z : ");
      Serial.print(offset_gyroX,6);
      Serial.print(", ");
      Serial.print(offset_gyroY,6);
      Serial.print(", ");
      Serial.print(offset_gyroZ,6);
      while (1){
        ;
      }
    }
  }
} 

void set_correct_ranges_and_frequency(){
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_260_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
}





