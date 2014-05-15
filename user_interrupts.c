// This is where the "user" interrupts handlers should go
// The *must* be declared in "user_interrupts.h"

#include "maindefs.h"
#include <stdlib.h>
#include <stdio.h>
#ifndef __XC8
#include <timers.h>
#else
#include <plib/timers.h>
#endif
#include <plib/usart.h>
#include "user_interrupts.h"
#include "messages.h"

// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer0 interrupt

void timer0_int_handler(unsigned char *adcbuffer) {
    WriteTimer0(0);
//    unsigned int val;
//    int length, msgtype;

    // toggle an LED
#ifdef __USE18F2680
    LATBbits.LATB0 = !LATBbits.LATB0;
#endif
    // reset the timer

    // try to receive a message and, if we get one, echo it back
//    length = FromMainHigh_recvmsg(sizeof(val), (unsigned char *)&msgtype, (void *) &val);
//    if (length == sizeof (val)) {
//        ToMainHigh_sendmsg(sizeof (val), MSGT_TIMER0, (void *) &val);
//    }


    ConvertADC();
    //while( BusyADC()) {
        //LATBbits.LATB1 = 1;
    //}
    //LATBbits.LATB1 = 0;


}

// A function called by the interrupt handler
// This one does the action I wanted for this program on a timer1 interrupt

void timer1_int_handler(unsigned char *adcbuffer) {
    //unsigned int result;

    // read the timer and then send an empty message to main()
#ifdef __USE18F2680
    LATBbits.LATB1 = !LATBbits.LATB1;
#endif

    //result = ReadTimer1();
    //ToMainLow_sendmsg(0, MSGT_TIMER1, (void *) 0);

    // reset the timer
    WriteTimer1(0);
}

void adc_int_handler(unsigned char *adcbuffer){
    // adcbuffer[0] stores the count
    if(adcbuffer[0] < 30)                 // increment counter
        adcbuffer[0] = adcbuffer[0] + 1;
    else
        adcbuffer[0] = 0;
    // ADRESL less significant, but is more accurate than our resolution

    int voltage = (int)ADRESH;




//    float voltage = 3.4*k/256;
//    int roundDist;
//    if (voltage > .7) {
//        float dist = 24/(voltage - 0.1);
//        roundDist = (int)(dist + 0.5);
//    }
//    else
//        roundDist = 0xFF;

/*
    if (adcbuffer[0] == 1){
        adcbuffer[3] = roundDist;
        SetChanADC(ADC_CH1);
    }
    else if (adcbuffer[0] == 2){
        adcbuffer[1] = roundDist;
        SetChanADC(ADC_CH2);
    }
    else if (adcbuffer[0] == 3){
        adcbuffer[2] = roundDist;
        SetChanADC(ADC_CH3);
    }
    */


    ////////Low///////////
    // > 48 too close for front
    // 00 is an error code
    // 1 is too close < 28 cm > 4C for channel 2      > 4C for channel 3
    // 3 is too far, ignore for front > 32 cm  < 45 for channel       2 < 46 for channel 3
    // 2 is correct

    //////////High////////
    // > 5C too close for front
    // 00 is an error code
    // 1 is too close < 28 cm > 5D for channel 2      > 5F for channel 3
    // 3 is too far, ignore for front > 32 cm  < 58 for channel 2    < 59 for channel 3
    // 2 is correct

    // Greater than 4.2 volts for on the ground
    ///////////////////////////////////////////////////////////////////////////

    int channel = (int)adcbuffer[0];
    int line = (int)adcbuffer[0] % 3;
    unsigned char output;

    if (channel == 0) {
            LATBbits.LATB3 = 0x01;
        if (voltage < 0x4F) {
            adcbuffer[4] = 0x01;
        }
        else
            adcbuffer[4] = 0x00;
        SetChanADC(ADC_CH0);
    }
    else if (channel == 1) {
        if (ADRESH > 0x68) //if (ADRESH > 0x55)
            output = 0x01;
        else
            output = 0x02;
        if (output != adcbuffer[1]) {
            if (output == adcbuffer[8]) {
                adcbuffer[1] = output;
                adcbuffer[8] = 0xFF;
            }
            else
                adcbuffer[8] = output;
        }
        else {
            adcbuffer[8] = 0xFF;
            adcbuffer[1] = output;
        }
        SetChanADC(ADC_CH10);
    }
    else if (channel == 10) {
            LATBbits.LATB3 = 0x01;
        if (voltage < 0x4F) {
            adcbuffer[4] = 0x01;
        }
        else
            adcbuffer[4] = 0x00;
        SetChanADC(ADC_CH1);
    }
    else if (channel == 11) {
        if (ADRESH - 4 > adcbuffer[2] || ADRESH + 4 < adcbuffer[2]) {
            if (ADRESH - 2 < adcbuffer[9] && ADRESH + 2 > adcbuffer[9]) {
                adcbuffer[2] = ADRESH;
                adcbuffer[9] = 0xFF;
            }
            else
                adcbuffer[9] = ADRESH;
        }
        else {
            adcbuffer[9] = 0xFF;
            adcbuffer[2] = ADRESH;
        }
        SetChanADC(ADC_CH12);
    }
    else if (channel == 20) {
            LATBbits.LATB3 = 0x01;
        if (voltage < 0x4F) {
            adcbuffer[4] = 0x01;
        }
        else
            adcbuffer[4] = 0x00;
        SetChanADC(ADC_CH2);
    }
    else if (channel == 21) {
        if (ADRESH - 4 > adcbuffer[3] || ADRESH + 4 < adcbuffer[3]) {
            if (ADRESH - 2 < adcbuffer[10] && ADRESH + 2 > adcbuffer[10]) {
                adcbuffer[3] = ADRESH;
                adcbuffer[10] = 0xFF;
            }
            else
                adcbuffer[10] = ADRESH;
        }
        else {
            adcbuffer[10] = 0xFF;
            adcbuffer[3] = ADRESH;
        }
        SetChanADC(ADC_CH8);
    }
    else {
        TXREG = voltage;
        if (line == 0) {
            LATBbits.LATB3 = 0x01;
            if (voltage < 0x4F) {
                adcbuffer[4] = 0x01;
            }
            else
                adcbuffer[4] = 0x00;
            SetChanADC(ADC_CH8);
        }
        else if (line == 1) {
            LATBbits.LATB4 = 0x01;
            if (voltage < 0x4F) {
                adcbuffer[5] = 0x01;
            }
            else
                adcbuffer[5] = 0x00;
            SetChanADC(ADC_CH10);
        }
        else{
            LATBbits.LATB5 = 0x01;
            if (voltage < 0x4F) {
                adcbuffer[6] = 0x01;
            }
            else
                adcbuffer[6] = 0x00;
            SetChanADC(ADC_CH12);
        }
    }
    if((adcbuffer[4] == 0x01) && (adcbuffer[5] == 0x01) && (adcbuffer[6] == 0x01)) {
        adcbuffer[7] = 0x01;
    }
    LATBbits.LATB3 = 0x00;
    LATBbits.LATB4 = 0x00;
    LATBbits.LATB5 = 0x00;
    ///////////////////////////////////////////////////////////////////////////

    /*
    if (channel == 0) {
        if (voltage < 3)
            adcbuffer[4] = 0x01;
        else
            adcbuffer[4] = 0x00;
        SetChanADC(ADC_CH1);
    }
    else if (channel == 1) {
    if (((int)roundDist - 4 > (int)adcbuffer[1]) || ((int)roundDist + 4 < (int)adcbuffer[1])) {
        if (((int)roundDist - 4 > (int)adcbuffer[8]) || ((int)roundDist + 4 < (int)adcbuffer[8])) {
            adcbuffer[1] = roundDist;
            adcbuffer[8] = 0xFF;
        }
        else
            adcbuffer[8] = 0xFF;
    }
    else {
        adcbuffer[8] = 0xFF;
        adcbuffer[1] = roundDist;
    }
        //adcbuffer[1] = roundDist;
        TXREG = adcbuffer[1];
        SetChanADC(ADC_CH6);
    }
    else if (channel == 10) {
        if (voltage < 3)
            adcbuffer[4] = 0x01;
        else
            adcbuffer[4] = 0x00;
        SetChanADC(ADC_CH2);
    }
    else if (channel == 11) {
        if (((int)roundDist - 4 > (int)adcbuffer[2]) || ((int)roundDist + 4 < (int)adcbuffer[2])) {
            if (((int)roundDist - 4 > (int)adcbuffer[9]) || ((int)roundDist + 4 < (int)adcbuffer[9])) {
                adcbuffer[2] = roundDist;
                adcbuffer[9] = 0xFF;
            }
            else
                adcbuffer[9] = 0xFF;
        }
        else {
            adcbuffer[9] = 0xFF;
            adcbuffer[2] = roundDist;
        }
        //adcbuffer[2] = roundDist;
        SetChanADC(ADC_CH7);
    }
    else if (channel == 20) {
        if (voltage < 3)
            adcbuffer[4] = 0x01;
        else
            adcbuffer[4] = 0x00;
        SetChanADC(ADC_CH3);
    }
    else if (channel == 21) {
        if (((int)roundDist - 4 > (int)adcbuffer[1]) || ((int)roundDist + 4 < (int)adcbuffer[1])) {
            if (((int)roundDist - 4 > (int)adcbuffer[8]) || ((int)roundDist + 4  (int)adcbuffer[8])) {
                adcbuffer[3] = roundDist;
                adcbuffer[10] = 0xFF;
            }
            else
                adcbuffer[10] = 0xFF;
        }
        else {
            adcbuffer[10] = 0xFF;
            adcbuffer[3] = roundDist;
        }

        //adcbuffer[3] = roundDist;
        SetChanADC(ADC_CH5);
    }
    else {
        if (line == 0) {
            if (voltage < 3)
                adcbuffer[4] = 0x01;
            else
                adcbuffer[4] = 0x00;
            SetChanADC(ADC_CH5);
        }
        else if (line == 1) {
            if (voltage < 3)
                adcbuffer[5] = 0x01;
            else
                adcbuffer[5] = 0x00;
            SetChanADC(ADC_CH6);
        }
        else{
            if (voltage < 3)
                adcbuffer[6] = 0x01;
            else
                adcbuffer[6] = 0x00;
            SetChanADC(ADC_CH7);
        }
    }*/
}