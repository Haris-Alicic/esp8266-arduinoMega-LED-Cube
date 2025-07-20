#include <Arduino.h>
#include <SPI.h>
#include "digitalWriteFast.h"

//PIN CONSTANTS
const uint8_t interrupt_pin = 46;
const uint8_t esp_active_pin = 47; 

//CUBE PINS
const uint8_t xPin[8][3] =
{
  {29, 37, 45},
  {28, 36, 44},
  {27, 35, 43},
  {26, 34, 42},
  {25, 33, 41},
  {24, 32, 40},
  {23, 31, 39},
  {22, 30, 38}
};

const uint8_t yPin[8]= {21, 20, 19, 18, 17, 16, 15, 14};

const uint8_t zPin[8]= {9, 8, 7, 6, 5, 4, 3, 2};

volatile uint8_t cubeState [64][3];
volatile uint8_t cube_position = 0;

//SPI VARABLES
volatile uint8_t reception_counter = 0;
volatile bool reception_done = 0;

//SET FRAMERATE
void setFramerate(float frames_per_second)
{
  long double float_output_compare = 0.0;
  uint64_t int_output_compare = 0;
  uint16_t prescaler = 0;
  uint8_t counter = 0;

  do
  {
    if(counter == 0) prescaler = 1;
    if(counter == 1) prescaler = 8;
    if(counter == 2) prescaler = 64;
    if(counter == 3) prescaler = 256;
    if(counter == 4) prescaler = 1024;
    float_output_compare = ((16000000.0 / (prescaler * 8.0 * 8.0 * frames_per_second)) - 1.0);
    int_output_compare = round(float_output_compare);
    counter++;
  }
  while(int_output_compare > 65535);

  cli();//stop interrupts
  TCCR5A = 0;// set entire TCCR1A register to 0
  TCCR5B = 0;// same for TCCR1B
  TCNT5  = 0;//initialize counter value to 0
  OCR5A = int_output_compare;
  TCCR5B |= (1 << WGM12);

  switch(prescaler)
  {
    case 1:
    TCCR5B |= (1 << CS10);  
    break;
    case 8:
    TCCR5B |= (1 << CS11);  
    break;
    case 64:
    TCCR5B |= (1 << CS11) | (1 << CS10); 
    break;
    case 256:
    TCCR5B |= (1 << CS12); 
    break;
    case 1024:
     TCCR5B |= (1 << CS10) | (1 << CS12);   
    break;
  }
  
  TIMSK5 |= (1 << OCIE4A);
  sei();//allow interrupts
}

void setup() {
  pinMode(interrupt_pin, OUTPUT);
  pinMode(esp_active_pin, INPUT);
  
  digitalWrite(interrupt_pin, LOW);

  //SPI
  SPCR |= bit(SPE);         /* Enable SPI */
  pinMode(MISO, OUTPUT);    /* Make MISO pin as OUTPUT */
  pinMode(SS, INPUT);
  SPI.attachInterrupt();    /* Attach SPI interrupt */
  
  setFramerate(150.0);

  Serial.begin (9600);


  interrupts();

  //CUBE STATE INITIALISATION

  for(uint8_t position = 0; position < 64; position++)
  {
    for(uint8_t color = 0; color < 3; color++)
    {
      cubeState[position][color] = 0;
    }
  }
 
  //CUBE PIN INITIALISATION

  pinModeFast(xPin[0][0], OUTPUT);
  digitalWriteFast(xPin[0][0], LOW);
  pinModeFast(xPin[0][1], OUTPUT);
  digitalWriteFast(xPin[0][1], LOW);
  pinModeFast(xPin[0][2], OUTPUT);
  digitalWriteFast(xPin[0][2], LOW);

  pinModeFast(xPin[1][0], OUTPUT);
  digitalWriteFast(xPin[1][0], LOW);
  pinModeFast(xPin[1][1], OUTPUT);
  digitalWriteFast(xPin[1][1], LOW);
  pinModeFast(xPin[1][2], OUTPUT);
  digitalWriteFast(xPin[1][2], LOW);

  pinModeFast(xPin[2][0], OUTPUT);
  digitalWriteFast(xPin[2][0], LOW);
  pinModeFast(xPin[2][1], OUTPUT);
  digitalWriteFast(xPin[2][1], LOW);
  pinModeFast(xPin[2][2], OUTPUT);
  digitalWriteFast(xPin[2][2], LOW);

  pinModeFast(xPin[3][0], OUTPUT);
  digitalWriteFast(xPin[3][0], LOW);
  pinModeFast(xPin[3][1], OUTPUT);
  digitalWriteFast(xPin[3][1], LOW);
  pinModeFast(xPin[3][2], OUTPUT);
  digitalWriteFast(xPin[3][2], LOW);

  pinModeFast(xPin[4][0], OUTPUT);
  digitalWriteFast(xPin[4][0], LOW);
  pinModeFast(xPin[4][1], OUTPUT);
  digitalWriteFast(xPin[4][1], LOW);
  pinModeFast(xPin[4][2], OUTPUT);
  digitalWriteFast(xPin[4][2], LOW);

  pinModeFast(xPin[5][0], OUTPUT);
  digitalWriteFast(xPin[5][0], LOW);
  pinModeFast(xPin[5][1], OUTPUT);
  digitalWriteFast(xPin[5][1], LOW);
  pinModeFast(xPin[5][2], OUTPUT);
  digitalWriteFast(xPin[5][2], LOW);

  pinModeFast(xPin[6][0], OUTPUT);
  digitalWriteFast(xPin[6][0], LOW);
  pinModeFast(xPin[6][1], OUTPUT);
  digitalWriteFast(xPin[6][1], LOW);
  pinModeFast(xPin[6][2], OUTPUT);
  digitalWriteFast(xPin[6][2], LOW);

  pinModeFast(xPin[7][0], OUTPUT);
  digitalWriteFast(xPin[7][0], LOW);
  pinModeFast(xPin[7][1], OUTPUT);
  digitalWriteFast(xPin[7][1], LOW);
  pinModeFast(xPin[7][2], OUTPUT);
  digitalWriteFast(xPin[7][2], LOW);

  pinModeFast(yPin[0], OUTPUT);
  digitalWriteFast(yPin[0], LOW);
  pinModeFast(yPin[1], OUTPUT);
  digitalWriteFast(yPin[1], LOW);
  pinModeFast(yPin[2], OUTPUT);
  digitalWriteFast(yPin[2], LOW);
  pinModeFast(yPin[3], OUTPUT);
  digitalWriteFast(yPin[3], LOW);
  pinModeFast(yPin[4], OUTPUT);
  digitalWriteFast(yPin[4], LOW);
  pinModeFast(yPin[5], OUTPUT);
  digitalWriteFast(yPin[5], LOW);
  pinModeFast(yPin[6], OUTPUT);
  digitalWriteFast(yPin[6], LOW);
  pinModeFast(yPin[7], OUTPUT);
  digitalWriteFast(yPin[7], LOW);

  pinModeFast(zPin[0], OUTPUT);
  digitalWriteFast(zPin[0], LOW);
  pinModeFast(zPin[1], OUTPUT);
  digitalWriteFast(zPin[1], LOW);
  pinModeFast(zPin[2], OUTPUT);
  digitalWriteFast(zPin[2], LOW);
  pinModeFast(zPin[3], OUTPUT);
  digitalWriteFast(zPin[3], LOW);
  pinModeFast(zPin[4], OUTPUT);
  digitalWriteFast(zPin[4], LOW);
  pinModeFast(zPin[5], OUTPUT);
  digitalWriteFast(zPin[5], LOW);
  pinModeFast(zPin[6], OUTPUT);
  digitalWriteFast(zPin[6], LOW);
  pinModeFast(zPin[7], OUTPUT);
  digitalWriteFast(zPin[7], LOW);
  interrupts();
}

void loop()
{

}


//FUNCTIONS
/***************************************************************************************************/

ISR(TIMER5_COMPA_vect)
{
  interrupts();
  reception_done = false;
  SPDR = cube_position; //SEND CURRENT ROW POSITION TO ESP
  
  if(digitalReadFast(esp_active_pin))
  {
    digitalWriteFast(interrupt_pin, HIGH);
    while(!reception_done){}
    digitalWriteFast(interrupt_pin, LOW);
  }

  switch (cube_position)
  {
  case 0:
    digitalWriteFast(zPin[7], LOW);
    digitalWriteFast(yPin[7], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[0], HIGH);
    digitalWriteFast(zPin[0], HIGH);
    break;
     
  case 1:
    digitalWriteFast(zPin[7], LOW);
    digitalWriteFast(yPin[0], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[1], HIGH);
    digitalWriteFast(zPin[0], HIGH);
    break;
     
  case 2:
    digitalWriteFast(zPin[7], LOW);
    digitalWriteFast(yPin[1], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[2], HIGH);
    digitalWriteFast(zPin[0], HIGH);
    break;
     
  case 3:
    digitalWriteFast(zPin[7], LOW);
    digitalWriteFast(yPin[2], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[3], HIGH);
    digitalWriteFast(zPin[0], HIGH);
    break;
     
  case 4:
    digitalWriteFast(zPin[7], LOW);
    digitalWriteFast(yPin[3], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[4], HIGH);
    digitalWriteFast(zPin[0], HIGH);
    break;
     
  case 5:
    digitalWriteFast(zPin[7], LOW);
    digitalWriteFast(yPin[4], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[5], HIGH);
    digitalWriteFast(zPin[0], HIGH);
    break;
     
  case 6:
    digitalWriteFast(zPin[7], LOW);
    digitalWriteFast(yPin[5], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[6], HIGH);
    digitalWriteFast(zPin[0], HIGH);
    break;
     
  case 7:
    digitalWriteFast(zPin[7], LOW);
    digitalWriteFast(yPin[6], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[7], HIGH);
    digitalWriteFast(zPin[0], HIGH);
    break;
/*********************************************************************/
  case 8:
    digitalWriteFast(zPin[0], LOW);
    digitalWriteFast(yPin[7], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[0], HIGH);
    digitalWriteFast(zPin[1], HIGH);
    break;
     
  case 9:
    digitalWriteFast(zPin[0], LOW);
    digitalWriteFast(yPin[0], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[1], HIGH);
    digitalWriteFast(zPin[1], HIGH);
    break;
     
  case 10:
    digitalWriteFast(zPin[0], LOW);
    digitalWriteFast(yPin[1], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[2], HIGH);
    digitalWriteFast(zPin[1], HIGH);
    break;
     
  case 11:
    digitalWriteFast(zPin[0], LOW);
    digitalWriteFast(yPin[2], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[3], HIGH);
    digitalWriteFast(zPin[1], HIGH);
    break;
     
  case 12:
    digitalWriteFast(zPin[0], LOW);
    digitalWriteFast(yPin[3], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[4], HIGH);
    digitalWriteFast(zPin[1], HIGH);
    break;
     
  case 13:
    digitalWriteFast(zPin[0], LOW);
    digitalWriteFast(yPin[4], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[5], HIGH);
    digitalWriteFast(zPin[1], HIGH);
    break;
     
  case 14:
    digitalWriteFast(zPin[0], LOW);
    digitalWriteFast(yPin[5], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[6], HIGH);
    digitalWriteFast(zPin[1], HIGH);
    break;
     
  case 15:
    digitalWriteFast(zPin[0], LOW);
    digitalWriteFast(yPin[6], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[7], HIGH);
    digitalWriteFast(zPin[1], HIGH);
    break;
/*********************************************************************/    
  case 16:
    digitalWriteFast(zPin[1], LOW);
    digitalWriteFast(yPin[7], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[0], HIGH);
    digitalWriteFast(zPin[2], HIGH);
    break;
     
  case 17:
    digitalWriteFast(zPin[1], LOW);
    digitalWriteFast(yPin[0], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[1], HIGH);
    digitalWriteFast(zPin[2], HIGH);
    break;
     
  case 18:
    digitalWriteFast(zPin[1], LOW);
    digitalWriteFast(yPin[1], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[2], HIGH);
    digitalWriteFast(zPin[2], HIGH);
    break;
     
  case 19:
    digitalWriteFast(zPin[1], LOW);
    digitalWriteFast(yPin[2], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[3], HIGH);
    digitalWriteFast(zPin[2], HIGH);
    break;
     
  case 20:
    digitalWriteFast(zPin[1], LOW);
    digitalWriteFast(yPin[3], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[4], HIGH);
    digitalWriteFast(zPin[2], HIGH);
    break;
     
  case 21:
    digitalWriteFast(zPin[1], LOW);
    digitalWriteFast(yPin[4], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[5], HIGH);
    digitalWriteFast(zPin[2], HIGH);
    break;
     
  case 22:
    digitalWriteFast(zPin[1], LOW);
    digitalWriteFast(yPin[5], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[6], HIGH);
    digitalWriteFast(zPin[2], HIGH);
    break;
     
  case 23:
    digitalWriteFast(zPin[1], LOW);
    digitalWriteFast(yPin[6], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[7], HIGH);
    digitalWriteFast(zPin[2], HIGH);
    break;
 /*********************************************************************/    
  case 24:
    digitalWriteFast(zPin[2], LOW);
    digitalWriteFast(yPin[7], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[0], HIGH);
    digitalWriteFast(zPin[3], HIGH);
    break;
     
  case 25:
    digitalWriteFast(zPin[2], LOW);
    digitalWriteFast(yPin[0], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[1], HIGH);
    digitalWriteFast(zPin[3], HIGH);
    break;
     
  case 26:
    digitalWriteFast(zPin[2], LOW);
    digitalWriteFast(yPin[1], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[2], HIGH);
    digitalWriteFast(zPin[3], HIGH);
    break;
     
  case 27:
    digitalWriteFast(zPin[2], LOW);
    digitalWriteFast(yPin[2], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[3], HIGH);
    digitalWriteFast(zPin[3], HIGH);
    break;
     
  case 28:
    digitalWriteFast(zPin[2], LOW);
    digitalWriteFast(yPin[3], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[4], HIGH);
    digitalWriteFast(zPin[3], HIGH);
    break;
     
  case 29:
    digitalWriteFast(zPin[2], LOW);
    digitalWriteFast(yPin[4], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[5], HIGH);
    digitalWriteFast(zPin[3], HIGH);
    break;
     
  case 30:
    digitalWriteFast(zPin[2], LOW);
    digitalWriteFast(yPin[5], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[6], HIGH);
    digitalWriteFast(zPin[3], HIGH);
    break;
     
  case 31:
    digitalWriteFast(zPin[2], LOW);
    digitalWriteFast(yPin[6], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[7], HIGH);
    digitalWriteFast(zPin[3], HIGH);
    break;
/*********************************************************************/     
  case 32:
    digitalWriteFast(zPin[3], LOW);
    digitalWriteFast(yPin[7], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[0], HIGH);
    digitalWriteFast(zPin[4], HIGH);
    break;
     
  case 33:
    digitalWriteFast(zPin[3], LOW);
    digitalWriteFast(yPin[0], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[1], HIGH);
    digitalWriteFast(zPin[4], HIGH);
    break;
     
  case 34:
    digitalWriteFast(zPin[3], LOW);
    digitalWriteFast(yPin[1], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[2], HIGH);
    digitalWriteFast(zPin[4], HIGH);
    break;
     
  case 35:
    digitalWriteFast(zPin[3], LOW);
    digitalWriteFast(yPin[2], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[3], HIGH);
    digitalWriteFast(zPin[4], HIGH);
    break;
     
  case 36:
    digitalWriteFast(zPin[3], LOW);
    digitalWriteFast(yPin[3], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[4], HIGH);
    digitalWriteFast(zPin[4], HIGH);
    break;
     
  case 37:
    digitalWriteFast(zPin[3], LOW);
    digitalWriteFast(yPin[4], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[5], HIGH);
    digitalWriteFast(zPin[4], HIGH);
    break;
     
  case 38:
    digitalWriteFast(zPin[3], LOW);
    digitalWriteFast(yPin[5], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[6], HIGH);
    digitalWriteFast(zPin[4], HIGH);
    break;
     
  case 39:
    digitalWriteFast(zPin[3], LOW);
    digitalWriteFast(yPin[6], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[7], HIGH);
    digitalWriteFast(zPin[4], HIGH);
    break;
 /*********************************************************************/    
  case 40:
    digitalWriteFast(zPin[4], LOW);
    digitalWriteFast(yPin[7], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[0], HIGH);
    digitalWriteFast(zPin[5], HIGH);
    break;
     
  case 41:
    digitalWriteFast(zPin[4], LOW);
    digitalWriteFast(yPin[0], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[1], HIGH);
    digitalWriteFast(zPin[5], HIGH);
    break;
     
  case 42:
    digitalWriteFast(zPin[4], LOW);
    digitalWriteFast(yPin[1], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[2], HIGH);
    digitalWriteFast(zPin[5], HIGH);
    break;
     
  case 43:
    digitalWriteFast(zPin[4], LOW);
    digitalWriteFast(yPin[2], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[3], HIGH);
    digitalWriteFast(zPin[5], HIGH);
    break;
     
  case 44:
    digitalWriteFast(zPin[4], LOW);
    digitalWriteFast(yPin[3], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[4], HIGH);
    digitalWriteFast(zPin[5], HIGH);
    break;
     
  case 45:
    digitalWriteFast(zPin[4], LOW);
    digitalWriteFast(yPin[4], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[5], HIGH);
    digitalWriteFast(zPin[5], HIGH);
    break;
     
  case 46:
    digitalWriteFast(zPin[4], LOW);
    digitalWriteFast(yPin[5], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[6], HIGH);
    digitalWriteFast(zPin[5], HIGH);
    break;
     
  case 47:
    digitalWriteFast(zPin[4], LOW);
    digitalWriteFast(yPin[6], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[7], HIGH);
    digitalWriteFast(zPin[5], HIGH);
    break;
/*********************************************************************/     
  case 48:
    digitalWriteFast(zPin[5], LOW);
    digitalWriteFast(yPin[7], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[0], HIGH);
    digitalWriteFast(zPin[6], HIGH);
    break;
     
  case 49:
    digitalWriteFast(zPin[5], LOW);
    digitalWriteFast(yPin[0], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[1], HIGH);
    digitalWriteFast(zPin[6], HIGH);
    break;
     
  case 50:
    digitalWriteFast(zPin[5], LOW);
    digitalWriteFast(yPin[1], LOW);
      {      
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[2], HIGH);
    digitalWriteFast(zPin[6], HIGH);
    break;
     
  case 51:
    digitalWriteFast(zPin[5], LOW);
    digitalWriteFast(yPin[2], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[3], HIGH);
    digitalWriteFast(zPin[6], HIGH);
    break;
     
  case 52:
    digitalWriteFast(zPin[5], LOW);
    digitalWriteFast(yPin[3], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[4], HIGH);
    digitalWriteFast(zPin[6], HIGH);
    break;
     
  case 53:
    digitalWriteFast(zPin[5], LOW);
    digitalWriteFast(yPin[4], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[5], HIGH);
    digitalWriteFast(zPin[6], HIGH);
    break;
     
  case 54:
    digitalWriteFast(zPin[5], LOW);
    digitalWriteFast(yPin[5], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[6], HIGH);
    digitalWriteFast(zPin[6], HIGH);
    break;
     
  case 55:
    digitalWriteFast(zPin[5], LOW);
    digitalWriteFast(yPin[6], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[7], HIGH);
    digitalWriteFast(zPin[6], HIGH);
    break;
/*********************************************************************/
  case 56:
    digitalWriteFast(zPin[6], LOW);
    digitalWriteFast(yPin[7], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[0], HIGH);
    digitalWriteFast(zPin[7], HIGH);
    break;
     
  case 57:
    digitalWriteFast(zPin[6], LOW);
    digitalWriteFast(yPin[0], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[1], HIGH);
    digitalWriteFast(zPin[7], HIGH);
    break;
     
  case 58:
    digitalWriteFast(zPin[6], LOW);
    digitalWriteFast(yPin[1], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[2], HIGH);
    digitalWriteFast(zPin[7], HIGH);
    break;
     
  case 59:
    digitalWriteFast(zPin[6], LOW);
    digitalWriteFast(yPin[2], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[3], HIGH);
    digitalWriteFast(zPin[7], HIGH);
    break;
     
  case 60:
    digitalWriteFast(zPin[6], LOW);
    digitalWriteFast(yPin[3], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[4], HIGH);
    digitalWriteFast(zPin[7], HIGH);
    break;
     
  case 61:
    digitalWriteFast(zPin[6], LOW);
    digitalWriteFast(yPin[4], LOW);
    {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[5], HIGH);
    digitalWriteFast(zPin[7], HIGH);
    break;
     
  case 62:
    digitalWriteFast(zPin[6], LOW);
    digitalWriteFast(yPin[5], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[6], HIGH);
    digitalWriteFast(zPin[7], HIGH);
    break;
     
  case 63:
    digitalWriteFast(zPin[6], LOW);
    digitalWriteFast(yPin[6], LOW);
      {
      if(((cubeState[cube_position][0] >> 0) & 1U))
      {digitalWriteFast(xPin[0][0], HIGH);}
      else
      {digitalWriteFast(xPin[0][0], LOW);}

      if(((cubeState[cube_position][1] >> 0) & 1U))
      {digitalWriteFast(xPin[0][1], HIGH);}
      else
      {digitalWriteFast(xPin[0][1], LOW);}

      if(((cubeState[cube_position][2] >> 0) & 1U))
      {digitalWriteFast(xPin[0][2], HIGH);}
      else
      {digitalWriteFast(xPin[0][2], LOW);}
/*****************************************/

      if(((cubeState[cube_position][0] >> 1) & 1U))
      {digitalWriteFast(xPin[1][0], HIGH);}
      else
      {digitalWriteFast(xPin[1][0], LOW);}

      if(((cubeState[cube_position][1] >> 1) & 1U))
      {digitalWriteFast(xPin[1][1], HIGH);}
      else
      {digitalWriteFast(xPin[1][1], LOW);}

      if(((cubeState[cube_position][2] >> 1) & 1U))
      {digitalWriteFast(xPin[1][2], HIGH);}
      else
      {digitalWriteFast(xPin[1][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 2) & 1U))
      {digitalWriteFast(xPin[2][0], HIGH);}
      else
      {digitalWriteFast(xPin[2][0], LOW);}

      if(((cubeState[cube_position][1] >> 2) & 1U))
      {digitalWriteFast(xPin[2][1], HIGH);}
      else
      {digitalWriteFast(xPin[2][1], LOW);}

      if(((cubeState[cube_position][2] >> 2) & 1U))
      {digitalWriteFast(xPin[2][2], HIGH);}
      else
      {digitalWriteFast(xPin[2][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 3) & 1U))
      {digitalWriteFast(xPin[3][0], HIGH);}
      else
      {digitalWriteFast(xPin[3][0], LOW);}

      if(((cubeState[cube_position][1] >> 3) & 1U))
      {digitalWriteFast(xPin[3][1], HIGH);}
      else
      {digitalWriteFast(xPin[3][1], LOW);}

      if(((cubeState[cube_position][2] >> 3) & 1U))
      {digitalWriteFast(xPin[3][2], HIGH);}
      else
      {digitalWriteFast(xPin[3][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 4) & 1U))
      {digitalWriteFast(xPin[4][0], HIGH);}
      else
      {digitalWriteFast(xPin[4][0], LOW);}

      if(((cubeState[cube_position][1] >> 4) & 1U))
      {digitalWriteFast(xPin[4][1], HIGH);}
      else
      {digitalWriteFast(xPin[4][1], LOW);}

      if(((cubeState[cube_position][2] >> 4) & 1U))
      {digitalWriteFast(xPin[4][2], HIGH);}
      else
      {digitalWriteFast(xPin[4][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 5) & 1U))
      {digitalWriteFast(xPin[5][0], HIGH);}
      else
      {digitalWriteFast(xPin[5][0], LOW);}

      if(((cubeState[cube_position][1] >> 5) & 1U))
      {digitalWriteFast(xPin[5][1], HIGH);}
      else
      {digitalWriteFast(xPin[5][1], LOW);}

      if(((cubeState[cube_position][2] >> 5) & 1U))
      {digitalWriteFast(xPin[5][2], HIGH);}
      else
      {digitalWriteFast(xPin[5][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 6) & 1U))
      {digitalWriteFast(xPin[6][0], HIGH);}
      else
      {digitalWriteFast(xPin[6][0], LOW);}

      if(((cubeState[cube_position][1] >> 6) & 1U))
      {digitalWriteFast(xPin[6][1], HIGH);}
      else
      {digitalWriteFast(xPin[6][1], LOW);}

      if(((cubeState[cube_position][2] >> 6) & 1U))
      {digitalWriteFast(xPin[6][2], HIGH);}
      else
      {digitalWriteFast(xPin[6][2], LOW);}
/*****************************************/
      
      if(((cubeState[cube_position][0] >> 7) & 1U))
      {digitalWriteFast(xPin[7][0], HIGH);}
      else
      {digitalWriteFast(xPin[7][0], LOW);}

      if(((cubeState[cube_position][1] >> 7) & 1U))
      {digitalWriteFast(xPin[7][1], HIGH);}
      else
      {digitalWriteFast(xPin[7][1], LOW);}

      if(((cubeState[cube_position][2] >> 7) & 1U))
      {digitalWriteFast(xPin[7][2], HIGH);}
      else
      {digitalWriteFast(xPin[7][2], LOW);}
/*****************************************/
      }
    digitalWriteFast(yPin[7], HIGH);
    digitalWriteFast(zPin[7], HIGH);
    break;
/*********************************************************************/
  default:
    break;
  }
  cube_position++;
  if(cube_position == 64) cube_position = 0;
}


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  uint8_t oldsrg = SREG;    //Disables interrupst
  cli();

  if(reception_counter != 0) cubeState[cube_position][reception_counter - 1] = SPDR;

  reception_counter++;
  if(reception_counter > 3)
  {
    reception_counter = 0;
    reception_done = true;
  }

  SREG = oldsrg;
}