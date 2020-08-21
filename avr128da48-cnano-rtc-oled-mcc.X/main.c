/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include <stdio.h>
#include "mcc_generated_files/mcc.h"
#include <util/delay.h>
#include "OLED_functions.h"
#include "CLOCK_functions.h"

#define BUTTON_PRESSED                  false
#define BUTTON_LONG_PRESS_THRESHOLD     900
#define BUTTON_TIME_STEP                10


typedef enum{
    BT_NOCHANGE,
    BT_SHORT_PRESS,
    BT_LONG_PRESS
} button_t;

static button_t buttonCheck(void);
void RTC_ISR(void);

uint8_t volatile second_passed = 0;
button_t button_state = BT_NOCHANGE;

int main(void)
{
    SYSTEM_Initialize();
    RTC_SetOVFIsrCallback(RTC_ISR);
    oledC_setup();
    OLED_displayInit();

    while (1){
        button_state = buttonCheck();
        if(second_passed == 1)
        {
            second_passed = 0;
            CLOCK_updateTime();
        }
        if(button_state == BT_SHORT_PRESS)
        {
            CLOCK_updateSelectedUnit();
        }
        else if(button_state == BT_LONG_PRESS)
        {
            CLOCK_updateSelection();
        }
        
    }
}


static button_t buttonCheck(void)
{
    button_t result = BT_NOCHANGE;
    uint8_t counter = 0;
    static bool old_button_state = !BUTTON_PRESSED;
    bool button_state = Button_GetValue();
    /* detecting only the button-press event */
    if( (button_state == BUTTON_PRESSED) && (old_button_state != BUTTON_PRESSED) )
    {
        /*  wait for debouncing time */
        _delay_ms(BUTTON_TIME_STEP);
        while( (Button_GetValue() == BUTTON_PRESSED) && \
                (counter < (BUTTON_LONG_PRESS_THRESHOLD/BUTTON_TIME_STEP)) )
        {
            /* then stay in the loop until either */
            /* is button released or long-press encountered*/
            counter++;
            _delay_ms(BUTTON_TIME_STEP);
        }
        if(counter)
        {
            result = BT_SHORT_PRESS;
            if(counter >= (BUTTON_LONG_PRESS_THRESHOLD/BUTTON_TIME_STEP))
                result = BT_LONG_PRESS;
        }
    }
    old_button_state = button_state;
    return result;
}

void RTC_ISR(void)
{
    second_passed = 1;
}
/**
    End of File
*/