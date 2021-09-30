/* Code For Hand Gesture Controlled WheelChair
 * Software used - Arduino IDE
 * Hardware Used - Arduino UNO
 * Created By    - Gopal Agarwal
 */

 /*  PINOUT of Receiver
  *     nrf MOSI -> Pin 11
  *     nrf MISO -> Pin 12
  *     nrf SCLK -> Pin 13
  *     Motor Driver -> Pin 5, 4, 3, 2, 6, 10
  */
 
/**********  Add Required Libraries  **********/
#include <SPI.h>          // Library for SPI Communication
#include <nRF24L01.h>     // Library for nrFL401
#include <RF24.h>         // Library for nrFL401

/**********  Global Variables  **********/
int WheelSpeed;
String text;

/**********  Create an RF24 object  **********/
RF24 radio(9, 8);       // CE Connected to Pin9, CSN Connected to Pin8

/**********  nrFL401 Module Address  *******/
const byte address[6] = "00001";

/**********  Pin Connection  **********/
int FR  = 5;            // Forward Right Side Motor connected to Pin5
int FL  = 4;            // Forward Left Side Motor connected to Pin4
int BR  = 3;            // Backward Right Side Motor connected to Pin3
int BL  = 2;            // Backward Right Side Motor connected to Pin2
int ENA = 6;            // ENA of Motor Driver to control the speed of Wheels to PWM Pin6
int ENB = 10;           // ENB of Motor Driver to control the speed of Wheels to PWM Pin10

/**********  Setup Function  **********/
void setup() 
{
  // Digital Pins 5,4,3,2,6,10 set as output
  pinMode(FR, OUTPUT);
  pinMode(FL, OUTPUT);
  pinMode(BR, OUTPUT);
  pinMode(BL, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  
  radio.begin();                       // Begin nrFL401 Commuication
  radio.openReadingPipe(0, address);   // Talk To only one nrFL401 whose address is "00001"
  radio.startListening();              // Set nrFL401 as Receiver
  radio.setDataRate(RF24_250KBPS);     // Set Datarate of nrf module to 250Kbps

  Serial.begin(9600);                  // Begin Serial Communication
}

/***  Infinite Loop Function  ***/
void loop() 
{
  // Read the data if available in buffer of nrFL401 Receiver
    if (radio.available())
     {
       radio.read(&text, sizeof(text));
       radio.read(&WheelSpeed, sizeof(WheelSpeed));
     }

  // Set the Speed of the Wheels
    analogWrite(ENA, WheelSpeed);
    analogWrite(ENB, WheelSpeed);   

  // Take the Decision for wheelchair movement
    if(text == "FR")                //If Hand Moves in Forward Right Direction
      {
       digitalWrite(FL, HIGH);
       digitalWrite(FR, LOW);
       digitalWrite(BR, LOW);
       digitalWrite(BL, HIGH);       
      }
    else if(text == "FL")         //If Hand Moves in Forward Left Direction
      {
       digitalWrite(FL, LOW);
       digitalWrite(FR, HIGH);
       digitalWrite(BR, HIGH);
       digitalWrite(BL, LOW);        
      }
    else if(text == "BR")        //If Hand Moves in Bacward Right Direction
      {
       digitalWrite(FL, HIGH);
       digitalWrite(FR, LOW);
       digitalWrite(BR, LOW);
       digitalWrite(BL, HIGH);        
      }
    else if(text == "BL")       //If Hand Moves in Backward Left Direction
      {
       digitalWrite(FR, HIGH);
       digitalWrite(FL, LOW);
       digitalWrite(BR, HIGH);
       digitalWrite(BL, LOW);        
      }      
    else if(text == "F")        //If Hand Moves in Forward Direction
      {
       digitalWrite(FR, HIGH);
       digitalWrite(FL, HIGH);
       digitalWrite(BR, HIGH);
       digitalWrite(BL, HIGH);     
      }
    else if(text == "B")       //If Hand Moves in Backward Direction
      {
       digitalWrite(FR, HIGH);
       digitalWrite(FL, HIGH);
       digitalWrite(BR, HIGH);
       digitalWrite(BL, HIGH);       
      }                         
    else if(text == "STOP")     //If Hand Moves is not moving
      {
       digitalWrite(FR, LOW);
       digitalWrite(FL, LOW);
       digitalWrite(BR, LOW);
       digitalWrite(BL, LOW);        
      }  
}
