/*
 * File:   main.c
 * 
 * Author01: BASIT AKRAM    (5161322)
 * Author02: ANKUR KOHLI    (5160903)
 * Author03: AMMAR IQBAL    (5183355)
 * 
 * Created on December 15, 2022, 5:24 PM
 */

// FOSC
#pragma config FPR = XT                 // Primary Oscillator Mode (XT)
#pragma config FOS = PRI                // Oscillator Source (Primary Oscillator)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config LPOL = PWMxL_ACT_HI      // Low-side PWM Output Polarity (Active High)
#pragma config HPOL = PWMxH_ACT_HI      // High-side PWM Output Polarity (Active High)
#pragma config PWMPIN = RST_IOPIN       // PWM Output Pin Reset (Control with PORT/TRIS regs)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

#include "stdio.h"
#include <stdlib.h>
#include "xc.h"
#include "timer.h"
#include "spi.h"
#include"parser.h"
#include"circularBuffer.h"
#include <string.h>

#define MAX_TASKS 3

void Motor_Control_Task();      //TASK 01
void ADC_Task();                //TASK 02
void blink_led();               //TASK 03

typedef struct {                //scheduler heartbeat structure
    int n;
    int N;
} heartbeat;

volatile circular_buffer_t circularBuffer;

//interrupt to receive data from UART receiver
void __attribute__ (( __interrupt__ , __auto_psv__ ) ) _U2RXInterrupt() {
    IFS1bits.U2RXIF = 0;
    while (U2STAbits.URXDA == 1) {
        write_cb(&circularBuffer, U2RXREG);
    }
}

/*void __attribute__((__interrupt__)) _U2TXInterrupt(void){
    int i;
    for (i = 0; string[i] != '\0'; i++) {
        while (U2STAbits.UTXBF == 1);
        U2TXREG = string[i];
    }
}
*/

int extract_integer(const char* str, int* n) {
    int i = 0, number = 0, sign = 1;
    if (str[i] == '-') {
        sign = -1;
        i++;
    } else if (str[i] == '+') {
        sign = 1;
        i++;
    }
    while (str[i] != ',' && str[i] != '\0') {
        if ((str[i] - '0') < 0 || (str[i] - '0') > 9)
            return -1;
        number *= 10; // multiply the current number by 10;
        number += str[i] - '0'; // converting character to decimal number
        i++;
    }
    *n = sign*number;
    return 0;
}

//function to map the input data to output ranges 
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

//ADC configuration
void adc_configuration() {
    // Setting the Tad
    ADCON3bits.ADCS = 8; // Tad = 4.5 Tcy
    // sampling mode: automatic sampling, automatic conversion
    ADCON1bits.ASAM = 1; // start
    ADCON1bits.SSRC = 7; // end
    ADCON3bits.SAMC = 16; // auto sampling time
    // selecting the channel to convert
    ADCON2bits.CHPS = 0b01;
    // chose the positive input of the channels
    ADCHSbits.CH0SA = 0b0010;
    ADCHSbits.CH123SA = 1;
    // select the AN2 and AN3 pin as analogue
    ADPCFG = 0xfff3;    
    ADCON1bits.SIMSAM = 1;
    // turn the ADC on
    ADCON1bits.ADON = 1;
}

//PWM configuration
void PWM_setup(){
    PTCONbits.PTMOD = 0; // free running
    PTCONbits.PTCKPS = 1; // 1:4 prescaler
    PWMCON1bits.PEN2H = 1;          
    PTPER = 1842; 
    PDC2 = 0;
    PTCONbits.PTEN = 1; // enable pwm
}

//scheduler synchronization
void scheduler(heartbeat schedInfo[]) {
    int i;
    for (i = 0; i < MAX_TASKS; i++) {
        schedInfo[i].n++;
        if (schedInfo[i].n >= schedInfo[i].N) {
            
            switch (i) {
                case 0:
                    Motor_Control_Task();
                    break;
                case 1:
                    ADC_Task();
                    break;
                case 2:
                    blink_led();
                    break;
            }
            schedInfo[i].n = 0;
        }
    }
}

//**********MAIN FUNCTION**********//

int main(void) {

    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    
    heartbeat schedInfo[MAX_TASKS];
    
    adc_configuration();
    SPI_setup();
    UART_setup();
    PWM_setup();
    tmr_wait_ms(2, 1000);
    tmr_setup_period(1, 5);
    
    schedInfo[0].n = 0;
    schedInfo[0].N = 1;
    
    schedInfo[1].n = 0;
    schedInfo[1].N = 200;
    
    schedInfo[2].n = 0;
    schedInfo[2].N = 100;
    
    while(1){
        scheduler(schedInfo);
        tmr_wait_period(1);
    }
    return 0;
}

//**********TASK 01 FUNCTION**********//

void Motor_Control_Task(){
    // parser initialization
    parser_state pstate;
	pstate.state = STATE_DOLLAR;
	pstate.index_type = 0; 
	pstate.index_payload = 0;   
    
    IEC1bits.U2RXIE = 0;
    int avl = avl_bytes(&circularBuffer);
    IEC1bits.U2RXIE = 1;
 
    int count = 0;
    while (count < avl) {
        char byte;
        int rpm;
        IEC1bits.U2RXIE = 0;
        read_cb(&circularBuffer, &byte);            //read data from buffer
        IEC1bits.U2RXIE = 1;

        int ret = parse_byte(&pstate, byte);        //send data to parser to decode it

         if (ret == NEW_MESSAGE) {
             if (strcmp(pstate.msg_type, "MCREF") == 0) {
                ret = extract_integer(pstate.msg_payload, &rpm);
                
                //ANOTHER WAY TO DO THE ABOVE TASK IS COMMENTED DOWN
                /*memcpy(string, pstate.msg_payload , pstate.msg_payload);
                string[(int) pstate.msg_payload] = '\0';
                write_string_LCD(string); */
                
                if (ret == 0) {
                    if (rpm < 0) {
                        send_string_uart("INVALID RPM");
                    } else {
                        char str[16];
                        double volt = map(rpm, 0.0, 1000.0, 0.0, 5.0);   //recieved rpm is to map on the value ranges between 0 to 5 volts.
                        PDC2 = volt * 2 * PTPER;            //generate duty cycle

                        sprintf(str, "Volts = %.2f", volt);
                        move_cursor_first_row(0);
                        clear_LCD(0);
                        move_cursor_first_row(0);
                        write_string_LCD(str);

                        sprintf(str, "PDC2 = %d", PDC2);
                        move_cursor_second_row(0);
                        clear_LCD(1);
                        move_cursor_second_row(0);
                        write_string_LCD(str);

                        //send_string_uart("OK");
                    }
                } //else {
                   //send_string_uart("ERROR IN PAYLOAD"); }           
             }
        }
        else {
            send_string_uart("  NO MESSAGE RECEIVED  ");
            }
        count++;
    }
}

//**********TASK 02 FUNCTION**********//

void ADC_Task(){        
    char str[16];
    double volts = 0.0, current = 0.0, temp = 0.0, mVolts = 0.0;        //initialization of all variables
    LATBbits.LATB1 = 0;         //set D4 low
    
    ADCON1bits.SAMP = 1; // start sampling
    while(!ADCON1bits.DONE);
    unsigned int AN2 = ADCBUF0;
    unsigned int AN3 = ADCBUF1;
    
    //Potentiometer Resistor (AN2) Value
    volts = 5.0 * (AN2/1023.0);     //convert in volts
    if(volts >= 3.0){               //check if the voltage is greater than 3 then current value should start increasing
        current = map(volts,3,5,0,20);
        
        /***SUBTASK 01***/ 
        //turn on D4 whenever the current exceeds 15 A.
        if (current >= 15)          
        {
            LATBbits.LATB1 = !LATBbits.LATB1;
        } else{
            LATBbits.LATB1 = 0;
        }
    } else{
            current = 0;
        }
    
    //Temperature Sensor (AN3) Value
    temp = AN3 * 0.161290323 - 40;         //temperature range between -40c - 125c
    mVolts = map(temp,-40, 125, 0.1, 1.75);
    
    sprintf(str, "$MCFBK,%.1f,%.1f*",current,temp);
    send_string_uart(str);
    
    //CHECK VALUES ON LCD
/*  sprintf(str, "V=%.1fV,Curr=%.1fA", volts, current);
    move_cursor_first_row(0);
    clear_LCD(0);
    move_cursor_first_row(0);
    write_string_LCD(str);
    sprintf(str, "T=%.1f C,V=%.1f mV", temp, mVolts);
    move_cursor_second_row(0);
    clear_LCD(1);
    move_cursor_second_row(0);
    write_string_LCD(str);    */
}

//**********TASK 03 FUNCTION**********//

void blink_led() {
    LATBbits.LATB0 = !LATBbits.LATB0;       //Toggle LED D3
}