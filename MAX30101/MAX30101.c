#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

//MAX30105 Sensor Slave Register
#define MAX30101_ADDRESS            0x57

//FIFO Register
#define MAX30101_FIFO_WR_PTR        0x04
#define MAX30101_OVF_COUNTER        0x05
#define MAX30101_FIFO_RD_PTR        0x06
#define MAX30101_FIFO_DATA          0x07

//Setting Register
#define MAX30101_FIFO_CONFIG        0x08
#define MAX30101_MODE_CONFIG        0x09
#define MAX30101_SPO2_CONFIG        0x0A

#define MAX30101_LED1_PA            0x0C
#define MAX30101_LED2_PA            0x0D
#define MAX30101_LED3_PA            0x0E
#define MAX30101_LED_PROX_AMP       0x10

//TCA9543A I2C MUX
#define TCA9543A_ADDRESS				0x73


//Data Important Variable
uint32_t red_led, ir_led;
uint32_t irBuffer[101]; 
uint32_t redBuffer[101];

int32_t bufferLength = 100;
int32_t spo2;
int8_t validSPO2;
int32_t heartRate;
int8_t validHeartRate;

uint8_t i;

/*"/dev/i2c-0"'s file descriptor*/
int fd;

//-----------------------------------------------------------

// MAX30101 Sensor Slave Address
void I2C_Slave_Check(void)
{
    if (ioctl(fd, I2C_SLAVE, MAX30101_ADDRESS) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave\n");
        exit(1);
    }

    if (write(fd, NULL, 0) < 0)
    {
        printf("[MAX30101(0x57)] I2C Sensor Is Missing\n");
        exit(1);
    }
    else
    {
        printf("Check OK!! [MAX30101(0x57)] I2C Sensor\n");
    }
}

//-----------------------------------------------------------

//MAX30101 Sensor Register Data Write
int WriteData(uint8_t reg_addr, uint8_t *data, int size) 
{
    uint8_t *buf;
    buf = malloc(size + 1);
    buf[0] = reg_addr;
    memcpy(buf + 1, data, size);
    write(fd, buf, size + 1);
    free(buf);

    return 1;
}

//-----------------------------------------------------------

//MAX30101 Sensor Register Data Read
int ReadData(uint8_t reg_addr, uint8_t *data, int size) 
{
    write(fd, &reg_addr, 1);
    read(fd, data, size);

    return 1;
}

//-----------------------------------------------------------

void I2C_MUX(void)
{
	uint8_t data = 0x01;

	if (ioctl(fd, I2C_SLAVE, TCA9543A_ADDRESS) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave\n");
        exit(1);
    }

    if (write(fd, NULL, 0) < 0)
    {
        printf("[TCA9543A(0x70)] I2C Sensor Is Missing\n");
		exit(1);
    }
    else
    {
        printf("Check OK!! [TCA9543A(0x70)] I2C Sensor\n");
		WriteData(TCA9543A_ADDRESS, &data, 1); 		
	}
}

//-----------------------------------------------------------

//MAX30101 Sensor Setting Reset(MAX30105 Sensor Standby)
void Reset(void) 
{
    uint8_t data = 0x40;
    WriteData(MAX30101_MODE_CONFIG, &data, 1);
    usleep(1000 * 100); 
}

//-----------------------------------------------------------

//MAX30101 Sensor Register Setting
int Init(void) 
{
    uint8_t data;

    data = 0x04;
    WriteData(MAX30101_FIFO_CONFIG, &data, 1); //Address : 0x08

    data = 0x03;
    WriteData(MAX30101_MODE_CONFIG, &data, 1); //Address : 0x09

    data = 0x27;
    WriteData(MAX30101_SPO2_CONFIG, &data, 1); //Address : 0x0A

    data = 0x2A;
    WriteData(MAX30101_LED1_PA, &data, 1); //Address : 0x0C

    data = 0x2A;
    WriteData(MAX30101_LED2_PA, &data, 1); //Address : 0x0D

    data = 0x2A;
    WriteData(MAX30101_LED3_PA, &data, 1); //Address : 0x0E

    data = 0x2A;
    WriteData(MAX30101_LED_PROX_AMP, &data, 1); //Address : 0x10

    data = 0x00;
    WriteData(MAX30101_FIFO_WR_PTR, &data, 1); //Address : 0x04

    data = 0x00;
    WriteData(MAX30101_OVF_COUNTER, &data, 1); //Address : 0x05

    data = 0x00;
    WriteData(MAX30101_FIFO_RD_PTR, &data, 1); //Address : 0x06

    usleep(1000 * 100); 

    return 1;
}

//-----------------------------------------------------------

//Obtaining Red_LED, IR_LED Data, GREEN_LED 
void MAX30101_Read_FIFO(void) 
{
    uint8_t buf[9];
    red_led = 0;
    ir_led = 0;

	if(!ReadData(MAX30101_FIFO_DATA, buf, 9))
    {
        printf("MAX30105 Data Read Error\n");
        exit(1);
    }
		
	//Obtaining RED_LED Data
	red_led += (uint32_t)buf[0] << 16;
    red_led += (uint32_t)buf[1] << 8;
    red_led += (uint32_t)buf[2];

	//Obtaining IR_LED Data
    ir_led += (uint32_t)buf[3] << 16;
    ir_led += (uint32_t)buf[4] << 8;
    ir_led += (uint32_t)buf[5];

	//RED_LED Data Save
    red_led &= 0x03FFFF;
    
	//IR_LED Data Save
	ir_led &= 0x03FFFF;
}

//-----------------------------------------------------------

//MAX30101 HR, SPO2 Data Print
void Data_Print(uint8_t data_check)
{
    switch(data_check)
    {
        case 0 :
            printf("Red: %7d, IR: %7d\n", red_led, ir_led);    
            break;
        
        case 1 : 
            //printf("Red: %7d, IR: %7d, BPM: %7d, HRvalid: %7d, SPO2: %7d, SPO2Valid: %7d\n", redBuffer[i], irBuffer[i], heartRate, validHeartRate, spo2, validSPO2);
            printf("HeartRate: %7d BPM, SPO2: %7d %\n", heartRate, spo2);
			break;
    }
}

//-----------------------------------------------------------

/* MAX30101 Data Setup
 * description : MAX30101 센서의 LED 값을 읽어와서 심장박동 수와 산소포화도를 계산한다.
 */
void Data_Setup(void)
{
    uint8_t finger_message_flag = 0;

    printf("---------------------------------------------------------------------------------\n");
    printf("Start MAX30101 Data Setup\n");
    usleep(1000 * 1000); 
    printf("Put On Your Finger In Sensor\n\n");

    while(1)
    {
	
        for (i = 0; i < bufferLength; i++)
		{
            // MAX30101에 있는 Red_LED, IR_LED Data, GREEN_LED 의 센서 값을 읽어 온다.
            MAX30101_Read_FIFO();
            if(ir_led >= 145000)
            {
                redBuffer[i] = red_led;
                irBuffer[i] = ir_led;       
                //Data_Print(0);
            }
            else
            {
                break;
            }    
        }
        if(i >= bufferLength-1)
        {
            break;
        }
    }
    // ir_led와 red_led에 저장된 값을 매개변수로 넘겨주고, spo2, vaslidSPO2, heartRate, validHeartrate 변수에 계산해서 저장한다.
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    printf("---------------------------------------------------------------------------------\n");
    printf("Data Setup Finish\n");
    printf("HR, SPO2 Data Print Start\n");
    printf("\n");
}

//-----------------------------------------------------------

//MAX30101 HR, SPO2 Data Analisis
void Data_Analisis(void)
{
    static uint16_t printf_data_number = 0;
    static uint8_t finger_message_flag = 0;

    // red_led와 ir_led 데이터 buffer 배열에서 25~99 번째 값들을 0~74번째로 옮긴다.
    for (i = 25; i < bufferLength; i++)
    {
        redBuffer[i - 25] = redBuffer[i];
        irBuffer[i - 25] = irBuffer[i];
    }


    // red_led와 ir_led 데이터 buffer 배열의 75~99번째 값들을 다시 센서에서 읽어온 값들로 채운다.
    for (i = 75; i < bufferLength; i++)
    {      
        MAX30101_Read_FIFO();    
        redBuffer[i] = red_led;
        irBuffer[i] = ir_led;  

        if(printf_data_number == 1000)
        {
            if(heartRate != -999 && spo2 != -999)
            {
                if(ir_led >= 145000)
                {
                    Data_Print(1);
                    finger_message_flag = 0;
                    printf_data_number = 0;
                }
                else
                {
                    if(finger_message_flag == 0)
                    {
                        printf("---------------------------------------------------------------------------------\n");
                        printf("Put On Your Finger In Sensor\n\n");
                        finger_message_flag = 1;
                    }
                    printf_data_number = 0;
                }
            }
        }
        else
        {
            printf_data_number++;
        }
    }
    // ir_led와 red_led에 저장된 값을 매개변수로 넘겨주고, spo2, vaslidSPO2, heartRate, validHeartrate 변수에 계산해서 저장한다.
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
}

//-----------------------------------------------------------

int main(int argc, char *argv[])// /dev/i2c-0 
{
    if((fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{
        printf("Failed to open i2c-2");
        exit(1);
    }

	I2C_MUX();
	I2C_Slave_Check();
	Reset(); 
	Init();
    Data_Setup();
        
	while(1) 
	{
        Data_Analisis();
    }
    return 0;
}
