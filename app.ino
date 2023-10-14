#include <avr/io.h>
#include <string.h>
#include "uart.h"

#define BAUD 9600 //defining the BAUD rate

#define PUSH_BUTTON 7 //push button at port d 7
#define LED 0 //LED at port b 0

const char ButOnText[] = "Button Pressed\n";
const char ButOffText[] = "Button not Pressed\n";
const char LedOnCMD [] ="led_on";
const char LedOffCMD [] ="led_off";

char receive_string[8];
int counter = 0;
char received;

int main(void){
  DDRD |= 1 << PUSH_BUTTON;
  DDRB &= ~(1 << LED);


  uart_init(BAUD); //initialize the BAUDRate setup

  int prevButton_state = PIND & (1<<PUSH_BUTTON); //use prev state to read changes 

  while(1){ //loop for running system
    if((UCSR0A)&(1<<RXC0)){

      received = uart_receive(); //user input command for LED

      if(received != '\n'){ // look for newline
        receive_string[counter++] = received;
        if(counter >= 8) {
          counter = 0;
        }
      }

      else{
        if( counter == 6 && strncmp(receive_string,LedOnCMD,6)==0 ){ //compare users inputted command with correct command
          uart_transmit_string("LED Is On\n",10);
          PORTB |= 1 << LED;
        }

        else if( counter == 7 && strncmp(receive_string,LedOffCMD,7)==0){ //compare users inputted command with correct command
          uart_transmit_string("LED Is Off\n",11);
          PORTB &= ~(1 << LED);
        }

        else{
          uart_transmit_string("ERROR Incorrect Command Inputted\n",33); //error message if command incorrect
        }

        counter = 0;
        }
    }

    int Button_state = PIND & (1<<PUSH_BUTTON); //put the state of the button in a variable

    if(Button_state != 0 && Button_state != prevButton_state){ //check for rising edge signal of a button
      uart_transmit_string(ButOnText,15);
    }

    else if (Button_state == 0 && Button_state != prevButton_state){
      uart_transmit_string(ButOffText,19);
    }

    prevButton_state  = Button_state; //setting new previous button state for rising edge detection
  }

  return 0;
}

