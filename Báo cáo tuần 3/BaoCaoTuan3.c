#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

int MPU;
int MAG;

/// MPU Register ///
#define SampleRate                  25
#define Configuration               26
#define GyroscopeConfiguration      27
#define AccelerometerConfiguration1 28
#define AccelerometerConfiguration2 29
#define PowerManagement1            107
#define Acceleration                59
#define Gyroscope                   67
#define Magnetometer                3
#define BypassEnableConfiguration   55
#define Control1                    0x0A
#define Control2                    0x0B

/// Raspberry Physic Pin ///
#define PWM 12

float ax, ay, az, a;
float vx, vy, vz, v;

float mx, my, mz, m;
float sx, sy, sz;

float wx0, wy0, wz0;
float wx1, wy1, wz1;

float Pitch, Roll, Yaw;

float PitchFromAcceleration, RollFromAcceleration;
float PitchFromGyroscope, RollFromGyroscope, YawFromGyroscope;
float YawFromMagnetometer;

int t0, t1;
int speed = 50;

void PWMSetUp(void)
{
    pinMode(PWM,PWM_OUTPUT);
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(960);
    pwmSetClock(400) ;
    pwmWrite(PWM,speed);
    delay(1000);
}

int flat(int input)
{
    int output = (input + 90)*50 / 180 + 50;
    return output;
}

void SetUpMPU9250(void)
{
    MPU = wiringPiI2CSetup(0x68);
    
    wiringPiI2CWriteReg8(MPU, SampleRate, 0x00);
    wiringPiI2CWriteReg8(MPU, Configuration, 0x03);
    wiringPiI2CWriteReg8(MPU, GyroscopeConfiguration, 0x18);
    wiringPiI2CWriteReg8(MPU, AccelerometerConfiguration1, 0x18);
    wiringPiI2CWriteReg8(MPU, AccelerometerConfiguration2, 0x03);
    wiringPiI2CWriteReg8(MPU, PowerManagement1, 0x01);
    wiringPiI2CWriteReg8(MPU, BypassEnableConfiguration, 0x02);

    MAG = wiringPiI2CSetup(0x0C);
    wiringPiI2CWriteReg8(MAG, Control1, 0x00);
    delay(10);
    wiringPiI2CWriteReg8(MAG, Control1, 0x16);
    delay(10);
}

int16_t ReadSensor(unsigned char sensor)
{
    int16_t high, low, data;
    high = wiringPiI2CReadReg8(MPU, sensor);
    low = wiringPiI2CReadReg8(MPU, sensor + 1);
    data = (high << 8) | low;
    return data;
}

int16_t ReadMagnetometer(unsigned char sensor)
{
    int16_t high, low, data;
    high = wiringPiI2CReadReg8(MAG, sensor + 1);
    low = wiringPiI2CReadReg8(MAG, sensor);
    data = (high << 8) | low;
    return data;
}

void UpdateValue()
{
    ax = (float)9.8 * ReadSensor(Acceleration) / 2048.0;
    ay = (float)9.8 * ReadSensor(Acceleration + 2) / 2048.0;
    az = (float)9.8 * ReadSensor(Acceleration + 4) / 2048.0;
    a = sqrt(pow(ax, 2) + pow(ay, 2) + pow(az, 2));

    wx1 = (float)ReadSensor(Gyroscope) / 16.4;
    wy1 = (float)ReadSensor(Gyroscope + 2) / 16.4;
    wz1 = (float)ReadSensor(Gyroscope + 4) / 16.4 - 1.16;

    mx = (float)ReadMagnetometer(Magnetometer);
    my = (float)ReadMagnetometer(Magnetometer + 2);
    mz = (float)ReadMagnetometer(Magnetometer + 4);

    while (wiringPiI2CReadReg8(MAG, 0x09) != 0x10);

    sx = (float)wiringPiI2CReadReg8(MAG, 0x10);
    sy = (float)wiringPiI2CReadReg8(MAG, 0x11);
    sz = (float)wiringPiI2CReadReg8(MAG, 0x12);

    mx = mx*(((sx - 128) * 0.5) / 128 + 1);
    my = my*(((sy - 128) * 0.5) / 128 + 1);
    mz = mz*(((sz - 128) * 0.5) / 128 + 1);

    t1 = millis();

    PitchFromAcceleration = -atan2(ax, sqrt(pow(ay, 2) + pow(az, 2))) * 180 / M_PI;
    RollFromAcceleration = atan2(ay, sqrt(pow(ax, 2) + pow(az, 2))) * 180 / M_PI;

    PitchFromGyroscope = PitchFromGyroscope + (wy0 + wy1) * (t1 - t0) / 2000.0;
    RollFromGyroscope = RollFromGyroscope + (wx0 + wx1) * (t1 - t0) / 2000.0;
    YawFromGyroscope = YawFromGyroscope + (wz0 + wz1) * (t1 - t0) / 2000.0 ;

    Pitch = 0.02 * PitchFromGyroscope + 0.98* PitchFromAcceleration;
    Roll = 0.02 * RollFromGyroscope + 0.98 * RollFromAcceleration;

    YawFromMagnetometer = atan2((- my * cos(Roll) + mz * sin(Roll)), (mx * cos(Pitch) + my * sin(Pitch) * sin(Roll) + mz * sin(Pitch) * cos(Roll)))* 180 / M_PI;
    // printf("YawFromMagnetometer = %.2f \n", YawFromMagnetometer);

    //Yaw = YawFromGyroscope*0.98 + YawFromMagnetometer*0.02;
    Yaw = YawFromGyroscope;

    // printf("wx0 = %.2f \n",wx0);
    // printf("wx1 = %.2f \n",wx1);
    // printf("wy0 = %.2f \n",wy0);
    // printf("wy1 = %.2f \n",wy1);
    //printf("wz0 = %.2f \n",wz0);
    // printf("wz1 = %.2f \n",wz1);
    // printf("mx = %.2f \n",mx);
    // printf("my = %.2f \n",my);
    // printf("mz = %.2f \n",mz);

    t0 = t1;
    wx0 = wx1;
    wy0 = wy1;
    wz0 = wz1;

}

void main(void)
{
    wiringPiSetupPhys();
    SetUpMPU9250();
    PWMSetUp();

    PitchFromGyroscope = 0;
    RollFromGyroscope = 0;
    YawFromGyroscope = 0;
    vx = 0;
    vy = 0;
    v  = 0;


    wx0 = (float)ReadSensor(Gyroscope) / 16.4;
    wy0 = (float)ReadSensor(Gyroscope + 2) / 16.4;
    wz0 = (float)ReadSensor(Gyroscope + 4) / 16.4 - 1.16;
    t0 = millis();

    while (1)
    {
        UpdateValue();
        printf("Pitch = %.2f \n", Pitch);
        printf("Roll  = %.2f \n", Roll);
        printf("Yaw   = %.2f \n", Yaw);
        // printf("ax = %.2f \n", ax);
        // printf("ay = %.2f \n", ay);
        // printf("az = %.2f \n", az);
        printf("a = %.2f \n", a);
        speed = flat(-(int)Roll);
        pwmWrite(PWM,speed);
        printf("speed = %d \n\n", speed);
    }
}
