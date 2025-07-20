# include <SPI.h>
#include <ESP8266WiFi.h>
#include <cmath>

//FUNCTIONS
/************************************************************************************************************************************************************************************************/
//Timer
class TIMER
{
  private:
  uint32_t current_time;
  uint32_t previous_time;
  public:
  //TIMER();
  void initialise(void);
  bool timePassed(uint32_t timeInterval);
};

void TIMER::initialise(void)
{
    current_time = 0;
    previous_time = 0;
}

bool TIMER::timePassed(uint32_t time_interval)
{
  current_time = micros();
  if((current_time - previous_time) >= time_interval)
  {
    previous_time = current_time;
    return true;
  }
  else return false;
}

TIMER timer;

//Cube functions
void setCubeBits(volatile uint8_t (&cubeBits)[3][64][3], uint8_t x_position, uint8_t y_position, uint8_t z_position, uint8_t r_color, uint8_t g_color, uint8_t b_color)
{
  int16_t color_intensity = 1;
  int16_t base_2 = 2;
  int16_t bit_buffer = 0;
  
  uint8_t colors[3];
  colors[0] = r_color;
  colors[1] = g_color;
  colors[2] = b_color;
  for(uint8_t color = 0; color < 3; color++)
  {
    color_intensity = 1;
    for(uint8_t count = 0; count < colors[color]; count++) color_intensity *= base_2;
    color_intensity -= 1; 
    for(uint8_t frame = 0; frame < 3; frame++)
    {
      bit_buffer = (color_intensity >> frame) & 1U;
      cubeBits[frame][(z_position * 8) + y_position][color] = (cubeBits[frame][(z_position * 8) + y_position][color] & ~(1U << x_position)) | (bit_buffer << x_position);
    }
  }
}

/************************************************************************************************************************************************************************************************/

//DECLARATIONS
void arduinoInterrupt();

//PIN CONSTANTS
const uint8_t esp_active_pin = D2;
const uint8_t interrupt_pin = D1;

//SPI
uint8_t cube_row_buffer[3] = {124, 53, 78};

//CUBE  VARIABLES
volatile uint8_t cubeBits[3][64][3];
uint8_t cubeFrame[8][8][8][3];
bool cube_on = false;
uint8_t curr_color = 0;
uint8_t prev_color = 0;

const uint8_t test_loop[64][3] =
{
  {0, 0, 0},
  {1, 0, 0},
  {2, 0, 0},
  {3, 0, 0},
  {0, 1, 0},
  {1, 1, 0},
  {2, 1, 0},
  {3, 1, 0},
  {0, 2, 0},
  {1, 2, 0},
  {2, 2, 0},
  {3, 2, 0},
  {0, 3, 0},
  {1, 3, 0},
  {2, 3, 0},
  {3, 3, 0},

  {0, 0, 1},
  {1, 0, 1},
  {2, 0, 1},
  {3, 0, 1},
  {0, 1, 1},
  {1, 1, 1},
  {2, 1, 1},
  {3, 1, 1},
  {0, 2, 1},
  {1, 2, 1},
  {2, 2, 1},
  {3, 2, 1},
  {0, 3, 1},
  {1, 3, 1},
  {2, 3, 1},
  {3, 3, 1},

  {0, 0, 2},
  {1, 0, 2},
  {2, 0, 2},
  {3, 0, 2},
  {0, 1, 2},
  {1, 1, 2},
  {2, 1, 2},
  {3, 1, 2},
  {0, 2, 2},
  {1, 2, 2},
  {2, 2, 2},
  {3, 2, 2},
  {0, 3, 2},
  {1, 3, 2},
  {2, 3, 2},
  {3, 3, 2},

  {0, 0, 3},
  {1, 0, 3},
  {2, 0, 3},
  {3, 0, 3},
  {0, 1, 3},
  {1, 1, 3},
  {2, 1, 3},
  {3, 1, 3},
  {0, 2, 3},
  {1, 2, 3},
  {2, 2, 3},
  {3, 2, 3},
  {0, 3, 3},
  {1, 3, 3},
  {2, 3, 3},
  {3, 3, 3}
};

/************************************************************************************************************************************************************************************************/

void setup() 
{
  pinMode(esp_active_pin, OUTPUT);
  pinMode(interrupt_pin, INPUT);
  pinMode(SS, OUTPUT);
  
  digitalWrite(esp_active_pin, HIGH);
  digitalWrite(SS, LOW); 
  Serial.begin(9600);

  //ARDUINO INTERRUPT
  attachInterrupt(digitalPinToInterrupt(interrupt_pin), arduinoInterrupt, RISING);
 

  //SPI
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV16);

  //TIMER
  timer.initialise();

  //CUBE STATE INITIALISATION
  for(uint8_t frame = 0; frame < 3; frame++)
  {
    for(uint8_t position = 0; position < 64; position++)
    {
      for(uint8_t color = 0; color < 3; color++)
      {
        cubeBits[frame][position][color] = 0;
      }
    }
  }

  interrupts();
} 

void loop (void)
{ 
    digitalWrite(esp_active_pin, HIGH);

    for(uint8_t z_pos = 0; z_pos < 8; z_pos++)
    {
      for(uint8_t y_pos = 0; y_pos < 8; y_pos++)
      {
        for(int16_t x_pos = 0; x_pos < 8; x_pos++)
        {

          setCubeBits(cubeBits, x_pos, y_pos, z_pos, test_loop[curr_color][0], test_loop[curr_color][1], test_loop[curr_color][2]);
        }
      }
    }
    curr_color++;
    if(curr_color >= 64) curr_color = 0;
    delay(1000);
}

ICACHE_RAM_ATTR void arduinoInterrupt()
{ 
  static uint8_t frame = 0;
  uint8_t cube_position = 0;
  cube_position = SPI.transfer(0);
  for(uint8_t color = 0; color < 3; color++) SPI.transfer( cubeBits[frame][cube_position][color]);
  frame++;
  if(frame == 3) frame = 0;
}

