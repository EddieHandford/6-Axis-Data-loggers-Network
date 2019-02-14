#include <Arduino.h>
#include <Wire.h> 
#include <SD.h> 
#include <SPI.h>

// Define Addresses, Registers and User Settings
//Remeber to set divsion to correct value on MPU function if changing the sensor
#define MPU_ADDR 0x68
#define PWR_MGMT_1 0x6B
#define GYRO_CONFIG 0x1B
#define FS_SEL 1 // 0 250deg/s, 1 500deg/s, 2 1000deg/s, 3 2000deg/s
#define ACCEL_CONFIG 0x1C
#define AFS_SEL 2 // 0 2g, 1 4g, 2 8g, 3 16g

// extern TwoWire Wire1;  // A 1k pullup resistor to 5V must be used on sda1 and scl1   - - - - - only required for use of second i2c line

int chipSelect = 8; //chip select pin for the MicroSD Card Adapter
File SlaveC;  // name of file on sd, yes i wasn't feeling very inventive

float AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ, dataOut;
unsigned long millistime;

void write_byte(uint8_t, uint8_t, uint8_t);
void update_mpu(float*, float*, float*, float*, float*, float*, float*);

int FSRval = 0;
int z = LOW ;



void setup() {
  
// Wire1.begin();        // join i2c bus (address optional for master)
// Serial.begin(9600);  // start serial for output
//Wire1.setClock(400000);   //left as standard for the moment


//mpu========================================================================================
Wire.begin();
write_byte(MPU_ADDR, PWR_MGMT_1, 0); // Set power management to turn on. No sleep or extras.
write_byte(MPU_ADDR, GYRO_CONFIG, FS_SEL << 3); //Sets 3rd + 4th bit according to FS_SEL
write_byte(MPU_ADDR, ACCEL_CONFIG, AFS_SEL << 3); //Sets 3rd + 4th bit according to AFS_SEL
//mpu end =================================================================================

 //sd card ==============================================================================================
  pinMode(chipSelect, OUTPUT); // chip select pin must be set to OUTPUT mode
  if (!SD.begin(chipSelect)) { // Initialize SD card
    Serial.println("Could not initialize SD card."); // if return value is false, something went wrong.
  }
  if (SD.exists("SlaveC.txt")) { // if "file.txt" exists, fill will be deleted
    Serial.println("File exists.");
    if (SD.remove("SlaveC.txt") == true) {
      Serial.println("Successfully removed file.");
    } else {
      Serial.println("Could not removed file.");
    }
  }
   //sd card end ========================================================================================


//sd column headers
SlaveC = SD.open("SlaveC.txt", FILE_WRITE); // open "file.txt" to write data
 if (SlaveC) {
    
    SlaveC.print("Time is: , ");
    SlaveC.print("Acceleration X: ,");
    SlaveC.print("Y: ,"); 
    SlaveC.print("Z: ,");
    SlaveC.print ("Gyro X: ,");
    SlaveC.print("Gyro Y: ,");
    SlaveC.print("Gyro Z: ,");
    SlaveC.print("FSR:"); 
    SlaveC.println();
    SlaveC.close(); // close file 
  } 
//sd card write end ==============================================================================================================================================
 pinMode(9, INPUT);  // 10k resistor required as pull down to ground
}

void loop() {
while ( (digitalRead(9) == HIGH) || (z=HIGH)  )  {
      //program will not start till pin 2 is brought high
millistime = millis();
update_mpu(&AcX, &AcY, &AcZ, &Tmp, &GyX, &GyY, &GyZ);  // fsr not needed  - - - - - // fsr removed 10/01/19
//sd card write ==================================================================================================================================================
SlaveC = SD.open("SlaveC.txt", FILE_WRITE); // open "file.txt" to write data
 if (SlaveC) {
    
    SlaveC.print(millistime); SlaveC.print(",");
    SlaveC.print(AcX); SlaveC.print(",");
    SlaveC.print(AcY); SlaveC.print(",");
    SlaveC.print(AcZ); SlaveC.print(",");
    SlaveC.print(GyX); SlaveC.print(",");
    SlaveC.print(GyY); SlaveC.print(",");
    SlaveC.print(GyZ); SlaveC.print(",");
    SlaveC.print(FSRval);
    SlaveC.println();
    SlaveC.close(); // close file 
    
  } 
//sd card write end ==============================================================================================================================================
z = HIGH ; 

}

}


void write_byte(uint8_t address, uint8_t subAddress, uint8_t data) {
    Wire.beginTransmission(address);  // Initialize the Tx buffer
    Wire.write(subAddress);           // Put slave register address in Tx buffer
    Wire.write(data);                 // Put data in Tx buffer
    Wire.endTransmission();           // Send the Tx buffer
}


void update_mpu(float* pAcX, float* pAcY, float* pAcZ, float* pTmp, float* pGyX, float* pGyY, float* pGyZ) {
  
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR,14,true);  // request a total of 14 registers
  int16_t tAcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  int16_t tAcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  int16_t tAcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  int16_t tTmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  int16_t tGyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  int16_t tGyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  int16_t tGyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
   
  // Acceleration. Division dependant on AFS_SEL 0 16384.0, 1 8192.0, 2 4096.0, 3 2048.0
  *pAcX = (float) tAcX/4096.0;
  *pAcY = (float) tAcY/4096.0;
  *pAcZ = (float) tAcZ/4096.0;

  // Temperature. Calculation from the data sheet.
  *pTmp = (float) tTmp/340.0 + 36.53;

 //Division dependant on FS_SEL  0 131, 1 65.5 , 2 32.8 , 3 16.4
  *pGyX = (float) tGyX/65.5;
  *pGyY = (float) tGyY/65.5;
  *pGyZ = (float) tGyZ/65.5;

}
