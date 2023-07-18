/***************************************************************
 * @file       src\device\deviceControl.c
 * @brief      IoT device attribute manage and control.This file is
 *  for device Manuc to modify.Device's attributes in include\deviceControl.h.   
 **************************************************************/
#include "deviceControl.h"

//record operate device times
int OP_TIME = 0; //First time operate Flag,0:no opeate, 1 first opeate


/***************************************************************
  *  @brief    execute verfied operations
  *  @param    short channel, short right, short param
  *  @return    0 success(write opertion success return), -1 right in req not found(out right range),-2 = other error 
  *  @return    RIGHT_STATUS_READ: 1=on 2=off ;RIGHT_STATUS_WRITE: 0=success;
  *  @return    RIGHT_BRIGHTNESS_READ: Ret=readDeviceBrightness() + 10; RIGHT_BRIGHTNESS_WRITE: 0=success
  *  @return    RIGHT_COLOR_READ: 4=red 5=yellow 6=green; RIGHT_COLOR_WRITE: 0=success
 **************************************************************/
int executeOperate(short channel, short right, short param)
{   
    OP_TIME = OP_TIME + 1;  // Device Operate time plus one, for Dmc auto shutoff
    // int executeRet = 0; // 
    //if Opeate first time close other channel
    if (OP_TIME == 1)
    {
        if (channel == 0){closeOtherChannel("amazonMQTT");}
        else if (channel == 1){closeOtherChannel("Zigbee");}
        else if (channel == 2){closeOtherChannel("Speaker");}    
        else if (channel == 3){closeOtherChannel("Local");}  
    }

    switch (right)
    {
        //status read
        case RIGHT_STATUS_READ:
        {
            // bool status = readDeviceStatus();
            bool status = readDeviceStatus();
            if(status == true) {return 1;}  //light on
            if(status == false) {return 2;} //light off
            break;
        }
        case RIGHT_STATUS_WRITE:
        {
            bool value = false;
            if (param == RIGHT_STATUS_WRITE_PARAM_ON) {value = true;} // param 0 --> on
            if (param == RIGHT_STATUS_WRITE_PARAM_OFF) {value = false;}  // param 1 --> off
            
            changeLightStatus(value);
            operateDeviceLight();
            return 0;
            break; 
        }

        case RIGHT_BRIGHTNESS_READ:
        {
            int brightnessRet = readDeviceBrightness();
            brightnessRet = brightnessRet + 10;//Return of brightness plus 10 (Ret Range 10 -110)
            return brightnessRet;
            break;
        }
        case RIGHT_BRIGHTNESS_WRITE:
        {
            int brightnessValue = 0;
            if (param == RIGHT_BRIGHTNESS_WRITE_PARAM_PLUS20) {brightnessValue = 20;}    // param 0 --> plus 20
            if (param == RIGHT_BRIGHTNESS_WRITE_PARAM_MINUS20) {brightnessValue = -20;}   // param 1 --> minus 20

            changeLightBrightness(brightnessValue);
            operateDeviceLight();
            return 0;
            break;
        }

        case RIGHT_COLOR_READ:
        {
            int colorRet = readDeviceColor(); // 4=red, 5=yellow, 6=green
            return colorRet;
            break;
        }
        case RIGHT_COLOR_WRITE:
        {   
            int colorValue;
            if (param == RIGHT_COLOR_WRITE_RED)    {colorValue = 4;} //param 4 --> led 4 red
            if (param == RIGHT_COLOR_WRITE_YELLOW) {colorValue = 5;} //param 5 --> led 5 yellow
            if (param == RIGHT_COLOR_WRITE_GREEN)  {colorValue = 6;} //param 6 --> led 6 green

            changeLightColor(colorValue);
            operateDeviceLight();
            return 0;
            break;
        }  
            
        default:
        {
            printf("[ExecuteOp] Req right: %hd not found(out right range)\n", right);
            return -1;
            break;
        }
    }
    return -1;

}

// change light status (on/off)
void changeLightStatus(bool value)
{
    //mutex_lock
    pthread_mutex_lock(&gMutexLightBulbStatus); 
    gLightBulbStatus = value;
    //mutex_unlock
    pthread_mutex_unlock(&gMutexLightBulbStatus);    
}

// change light brightness
void changeLightBrightness(int value)
{
    //mutex_lock
    pthread_mutex_lock(&gMutexLightBulbBrightness); 

    int brightness = 0;
    brightness = gLightBulbBrightness + value;
    // check brightness 20-100
    if (brightness >=20 && brightness <= 100)
    {
        gLightBulbBrightness = brightness; 
    } else
    {
        printf("\nError Brightness value overflow (20-100),no changes\n");
    }
    //mutex_unlock
    pthread_mutex_unlock(&gMutexLightBulbBrightness);    
}

// change light color
void changeLightColor(int color)
{
    //mutex_lock
    pthread_mutex_lock(&gMutexLightBulbColor); 
    gLightBulbColor = color;
    //mutex_unlock
    pthread_mutex_unlock(&gMutexLightBulbColor);    
}

/***************************************************************
  *  @brief     clear raspberry gpio to Output LOW
  *  @param     void
  *  @return    void 
 **************************************************************/
void clearGPIO(void)
{
    int led1 = 4, led2 = 5, led3 = 6; // red yellow green
    if(wiringPiSetup() < 0){
        printf("wiringPi error\n");
    }
    pinMode(led1, OUTPUT);  
    digitalWrite(led1, LOW);
    
    pinMode(led2, OUTPUT);  
    digitalWrite(led2, LOW);

    pinMode(led3, OUTPUT);  
    digitalWrite(led3, LOW);

    delay(20);  //20ms
}


/***************************************************************
  *  @brief     operte Device Light to the configured status/brighness/color
  *  @param     bool value,set light to value
  *  @return    void 
 **************************************************************/
void operateDeviceLight(void)
{   
    // int led1 = 4, led2 = 5, led3 = 6; // red yellow green
    clearGPIO();     //led init, clear gpio status to Output Low.

    // int softPwmCreate (int pin, int initialValue, int pwmRange) ; pwmRange 最大用100 之后cpu占用就高了  

    int colorLed = gLightBulbColor;  //switch light color, default is pin4 = red led 
    int brightness = gLightBulbBrightness;

    if (gLightBulbStatus == true) 
    {
        softPwmCreate(colorLed, 0, 100);
        softPwmWrite(colorLed, brightness);
        printf("\n****Set light (Color: %d) to (Brighness: %d) ****\n", colorLed, brightness);
    } else 
    {
        printf("\n****light status is OFF ****\n");  //light is off
    }
    
}


// // 现在的方案是brightness默认设置为60，灯关时brightness有数值，但不能进行修改。灯开时使用brightness值亮起
// /***************************************************************
//   *  @brief     operate bulb brightnessm,20-100 init=60
//   *  @param     int value, plus light value
//   *  @return    void 
//  **************************************************************/
// void operateDeviceBrightness(int value)
// {   
//     //check Light on
//     if(gLightBulbStatus == false)
//     {
//         printf("\nLight Status OFF, please turn on it before opearting brightness.\n");
//         return;
//     }

//     //led init
//     char led = 4;// 4 in <wiringPi.h> == 23 in BCM, Eighth on the upper right
//     if(wiringPiSetup() < 0){
//         printf("wiringPi error\n");
//     }
//     pinMode (led,OUTPUT); 


//     //mutex_lock
//     pthread_mutex_lock(&gMutexLightBulbBrightness); 

//     int brightness = 0;
//     brightness = gLightBulbBrightness + value;
//     // check brightness 20-100
//     if (brightness >=20 && brightness <= 100)
//     {
//        gLightBulbBrightness = brightness; 
//     }
    
//     else{
//         printf("\nError Brightness value overflow (20-100),no changes\n");
        
//     }
//     //led brighness opeate 使用pwn波brightness决定占空比 具体实现代码在/test/light/ 里,还没实装到deviceControl.c
//     delay(300); //ms

//     //mutex_unlock
//     pthread_mutex_unlock(&gMutexLightBulbBrightness);
//     printf("\n****Set light brightness to %d ****\n", gLightBulbBrightness);

// }

//just check bulb status, no output
bool CheckDeviceStatus(void)
{
    //printf("\n****Light status is %d ****\n", gLightBulbStatus);
    return gLightBulbStatus;
}

//print bulb on/off and return
bool readDeviceStatus(void)
{
    printf("\n****Light status is %d ****\n", gLightBulbStatus);
    return gLightBulbStatus;
}

//print bulb brightness and return
int readDeviceBrightness(void)
{
    printf("\n****Light Brightness is %d ****\n", gLightBulbBrightness);
    return gLightBulbBrightness;
}

//print bulb color and return
int readDeviceColor(void)
{
    printf("\n****Light Color is NO.%d ****\n", gLightBulbColor);
    return gLightBulbColor;
}


