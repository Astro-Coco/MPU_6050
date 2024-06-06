#include <QMC5883LCompass.h>

QMC5883LCompass compass;

void setup() {
  Serial.begin(115200);
  compass.init();

  compass.setMagneticDeclination(13,46);
  compass.setMode(0x01,0x0C, 0x00, 0xC0);
  compass.setSmoothing(10, true);
}

void loop() {

	float x, y, z, a, b;
	char myArray[3];
	
	compass.read();
  
	x = compass.getX();
	y = compass.getY();
	z = compass.getZ();
	
	a = map(compass.getAzimuth(),-180,180,0,360);
	
	b = compass.getBearing(a);

	compass.getDirection(myArray, a);
  
  /*
	Serial.print("X: ");
	Serial.print(x);

	Serial.print(" Y: ");
	Serial.print(y);

	Serial.print(" Z: ");
	Serial.print(z);

	Serial.print(" Azimuth: ");
	Serial.print(a);

	Serial.print(" Bearing: ");
	Serial.print(b);

	Serial.print(" Direction: ");
	Serial.print(myArray[0]);
	Serial.print(myArray[1]);
	Serial.print(myArray[2]);

	Serial.println();
  */

  Serial.print(x,6); Serial.print(",");
  Serial.print(y,6); Serial.print(",");
  Serial.print(z,6); Serial.println(",");
  delay(200); //5Hz

}