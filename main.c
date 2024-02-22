#include <msp430.h>
#include <driverlib.h>
#include <stdio.h>
#include "hal_LCD.h"



// Timers

unsigned int red=0,green=0,blue=0;
unsigned int count = 0;
unsigned int wait = 0;
unsigned int which_filter = 0;

// Timer A0 interrupt
#pragma vector = TIMER0_A0_VECTOR
__interrupt void delay1 ()
{
  P1OUT ^= BIT0; // blinking LED to confirm functioning
  
  wait++;
  
  if (wait == 10){
    
    which_filter++;
    
    if (which_filter == 1) {

        red = count;
        count = 0;
        P2OUT = 0x80; // colour filter to  blue 2.7 high, 2.5 low
        
    
    }
    else if (which_filter == 2) {

        blue = count;
        count = 0;
        P2OUT = 0x20; // colour filter to green 2.7, low 2.5 high
    }
    else if (which_filter == 3) {

        green = count;
        count = 0;
        P2OUT = 0x20; // colour filter to green 2.7, low 2.5 high
    
    }
    else{
      
        which_filter = 0;
    }
    
    wait = 0;
    count = 0;
  
  }
  

}

// Timer A1 interrupt
#pragma vector = TIMER1_A1_VECTOR
__interrupt void increment(void) 
{
        switch (TA1IV) {
        case 0x04:              //Capture/compare interrupt 2
             count++;
                break;
        default:
                break;
        }
  
  
}

// timer registers
void init_timers()
{
  
  // Timer A1 assigned to pin 8.3 as a counter
  TA1CCTL2 |= CM_1; // capture on rising edge of clock
  TA1CCTL2 |= CCIS_0; // select pin 8.3
  TA1CCTL2 |= SCS; // synchronous capture
  TA1CCTL2 |= CAP; // Capture mode
  TA1CCTL2 |= CCIE; // enable interupts
  TA1CTL |= TASSEL_2 | MC_2 | TACLR; // SMCLK, continuous mode, clear timer
  // pins for timer A1
  P8DIR = ~0x08; // port 8.3 set as input for OUT to connect to timer TA0.1
  P8SEL0 = 0x08; // port 8.3 selected as primary function (timer A0)

   // Timer A0 - confirmed working
  TA0CTL |= TACLR; // reset timer
  TA0CTL |= TASSEL_2; // SMCLK
  TA0CTL |= MC_1; // timer starts counts to TA0CCR0
  TA0CCTL0 = CCIE; // interupts enabled
  TA0CCR0 |= 10000 - 1 ;
  
}


void displayIntegerOnLCD(int value) {
    char str[20]; // Assuming a maximum of 5 digits for the integer
    sprintf(str, "%d", value); // Convert integer to string

    displayScrollText(str); // Display the string on LCD using displayScrollText function
}

void init_ColourSensor(){
  
    P1DIR = 0x31; // ports 1.4 and 1.5 as outputs for S0 and S1
    
    P2DIR = 0xA0; // port 2.5 and 2.7 as outputs for S2 and S3 respectivly
     
    P1OUT = 0x30; // 1.4 high and 1.5 low: output frequency scaling = 100%
    P2OUT = 0x00; // red colour filter initially

}




int  main(void){

        WDTCTL = WDTPW + WDTHOLD;  // Stop WDT
        P1OUT = 0x00;
      
        PMM_unlockLPM5();
        
        init_timers(); // initialise timers 
        init_ColourSensor(); // initialise colour sensor
        Init_LCD(); // initialise LCD
        
        __enable_interrupt(); // enable global interrupts
       __bis_SR_register(GIE); // makes timer interrupts work unsure why
          
        

          clearLCD();
          
          while(1){
            
      
             displayIntegerOnLCD(green);
            
            
          }
          
          


}

