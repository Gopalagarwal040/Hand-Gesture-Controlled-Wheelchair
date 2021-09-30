/* Code For Hand Gesture Controlled WheelChair
 * Software used - Arduino IDE
 * Hardware Used - Arduino UNO
 * Created By    - Gopal Agarwal
 */

/*  PINOUT of Transmitter
  *     MPU6050 SCl -> Pin A5
  *     MPU6050 SDA -> Pin A4
  *     Flex Sensor -> Pin A0
  *     nrf MOSI -> Pin 11
  *     nrf MISO -> Pin 12
  *     nrf SCLK -> Pin 13
  */
 
/**********  Add Required Libraries  **********/
#include <Wire.h>         // Wire Library for I2C Communication
#include <SPI.h>          // Library for SPI Communication
#include <nRF24L01.h>     // Library for nrfl2401
#include <RF24.h>         // Library for nrfl2401

/**********  Global Variables  **********/
const int FLEX_PIN = A0;
const float Vcc    = 5.0;
int WheelSpeed, Vadc_out, V_out_map;
int Raw_Accl_X, Raw_Accl_Y, Raw_Accl_Z;
float Ax, Ay, Az, V_out;

/**********  Create an RF24 object  **********/
RF24 radio(9, 8);  // CE Connected to Pin9, CSN Connected to Pin8

/**********  nrfl2401 Module Address  *******/
const byte address[6] = "00001";

/**********  Setup Function  **********/
void setup() 
{
  radio.begin();                      // Begin nrfl2401 Commuication
  radio.stopListening();              // Stop listening for incoming messages, and switch to transmit mode.
  radio.openWritingPipe(address);     // Set the nrfl2401 address
  radio.setDataRate(RF24_250KBPS);    // Set Datarate of nrf module to 250Kbps

  pinMode(FLEX_PIN, INPUT_PULLUP);   // FLex sensor input taken at pin A0
  
  Serial.begin(9600);                // Begin Serial Communication
  Wire.begin();                      // Begin I2C Communication
  setUpMPU();                        // Configure the MPU6050 Accelerometer
}

/***  Infinite Loop Function  ***/
void loop() 
{
  Vadc_out   = analogRead(FLEX_PIN);              // Read Flex Sensor Analog Value
  V_out      = Vadc_out * (Vcc/1023.0);           // Voltage converted in range of 0-5V
  V_out_map  = V_out * 100;                       // Voltage converted into int from float
  WheelSpeed = map(V_out_map, 235, 335, 0, 255);  // Map Resistance Values in range of Arduino PWM Values
  readAndProcessAccelData();                      // Read Data from MPU6050 Accelerometer


  if((Ax>=0.4 && Ax<=1)  && (Ay<=-0.4 && Ay>=-1))           //Make Forward Right Decision
   {
    String text = "FR";
    radio.write(&text, sizeof(text));
    radio.write(&WheelSpeed, sizeof(WheelSpeed));
   }
  else if ((Ax>=0.4 && Ax<=1) && (Ay>=0.4 && Ay<=1))        //Make Forward Left Decision
   {
    String text = "FL";
    radio.write(&text, sizeof(text));
    radio.write(&WheelSpeed, sizeof(WheelSpeed));
   }
  else if ((Ax<=-0.4 && Ax>=-1) && (Ay<=-0.4 && Ay>=-1))    //Make Backward Right Decision
   {
    String text = "BR";
    radio.write(&text, sizeof(text));
    radio.write(&WheelSpeed, sizeof(WheelSpeed));
   }
  else if((Ax<=-0.4 && Ax>=-1) && (Ay>=0.4 && Ay<=1))       //Make Backward Left Decision
   {
    String text = "BL";
    radio.write(&text, sizeof(text));
    radio.write(&WheelSpeed, sizeof(WheelSpeed));
   }
  else if(Ax>=0.4 && Ax<=1)                               //Make Forward Decision
   {
    String text = "F";
    radio.write(&text, sizeof(text));
    radio.write(&WheelSpeed, sizeof(WheelSpeed));
   }
  else if(Ax<=-0.4 && Ax>=-1)                             //Make Backward Decision
   {
    String text = "B";
    radio.write(&text, sizeof(text));
    radio.write(&WheelSpeed, sizeof(WheelSpeed));
   }
  else                                                    //Make Stop Decision
   {
    String text = "STOP";
    radio.write(&text, sizeof(text));
   }
}

void setUpMPU() 
{
  // power management1
    Wire.beginTransmission(0b1101000);          // Start the communication by using MPU6050 I2C Adrress = 0x68
    Wire.write(0x6B);                           // Access the power management register1
    Wire.write(0b00001000);                     // Set sleep = 0, reset = 0, temperature sensor = 1, clk source for MPU6050 = internal 8MHz crystal oscillator
    Wire.endTransmission();                     // End the Transmission

  // configure accelerometer
    Wire.beginTransmission(0b1101000);          // Start the communication by using MPU6050 I2C Adrress = 0x68
    Wire.write(0x1C);                           // Access the accelerometer configuration register
    Wire.write(0b00000000);                     // +-2g range is selected
    Wire.endTransmission();                     // End the Transmission

   // configure Digital Low Pass Filter
    Wire.beginTransmission(0b1101000);          // Start the communication by using MPU6050 I2C Adrress = 0x68
    Wire.write(0x1A);                           // Access the Digital Low Pass Filter configuration register                       
    Wire.write(0b00000101);                     // 10Hz Cutoff frequency selected
    Wire.endTransmission();                     // End the Transmission
}

void readAndProcessAccelData() 
{
  Wire.beginTransmission(0b1101000);            // Start the communication by using MPU6050 I2C Adrress = 0x68
  Wire.write(0x3B);                             // Access Accelerometer Measurement register            
  Wire.endTransmission();                       // End the Transmission
  Wire.requestFrom(0b1101000,6);                // Request to read 6 register from 0x3B 
  while(Wire.available() < 6);                  // Read till register 0x40
  Raw_Accl_X = Wire.read()<<8|Wire.read();      // X axis Acceleration 
  Ax = Raw_Accl_X/16384.0;                      // Converting value into -1g to +1g
  Raw_Accl_Y = Wire.read()<<8|Wire.read();      // y axis Acceleration  
  Ay = Raw_Accl_Y/16384.0;                      // Converting value into -1g to +1g
  Raw_Accl_Z = Wire.read()<<8|Wire.read();      // z axis Acceleration 
  Az = Raw_Accl_Z/16384.0;                      // Converting value into -1g to +1g
}
