/***************************************************************
 * @brief    clear wiringPi pins
 * // gcc wiringClear.c -o wiringClear -lwiringPi -Wall  
 **************************************************************/
#include <stdio.h>
#include <wiringPi.h>

int main()
{   
    int led1 = 4;   
    int led2 = 5;
    int led3 = 6;
    wiringPiSetup();        // 初始化gpio
    pinMode(led1, OUTPUT);  
    printf("Set LOW GPIO in WiringPi:%d\n",led1);
    digitalWrite(led1, LOW);
    
    pinMode(led2, OUTPUT);  
    printf("Set LOW GPIO in WiringPi:%d\n",led2);
    digitalWrite(led2, LOW);

    pinMode(led3, OUTPUT);  
    printf("Set LOW GPIO in WiringPi:%d\n",led3);
    digitalWrite(led3, LOW);


    return 0;
}


