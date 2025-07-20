# include <SPI.h>
char str[ ]="Imad prica\n";

void setup() 
{
  pinMode(D4, INPUT);
Serial.begin(115200); // set baud rate to 115200 for usart
SPI.begin();
SPI.setClockDivider(SPI_CLOCK_DIV8); //divide the clock by 8
Serial.println("Hello I'm SPI Mega_Master");
} 

void loop (void)
 {
 digitalWrite(SS, LOW); // enable Slave Select
 // send test string
 for(int i=0; i< sizeof(str); i++) 
 SPI.transfer( str [i] );

 digitalWrite(SS, HIGH); // disable Slave Select
 delay(2000);
}