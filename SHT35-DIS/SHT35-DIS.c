#include "SHT35-DIS.h"
#include "../I2Cconf.h"
/*"/dev/i2c-0"'s file descriptor*/

uint8_t reg, cmd;
uint8_t Temp_Hum_Raw_Data[5];
uint16_t ST, SRH;
double temp, hum;

//-----------------------------------------------------------

//SHT3X-DIS Sensor Check
void I2C_Slave_Check(void)
{	
    if(ioctl(fd, I2C_SLAVE, SHT_SLAVE_ADDRESS) < 0)
	{
        printf("Failed to acquire bus access and/or talk to slave\n");
        exit(1);
    }
	
	if(write(fd, NULL, 0) < 0)
	{
		printf("[SHT35-DIS(0x45)] I2C Sensor Is Missing\n");
		exit(1);
	}
	else
	{
		printf("Check OK!! [SHT35-DIS(0x45)] I2C Sensor\n");
	}
}

//---------------------------------------------------------------

//SHT3X-DIS Sensor Reset
void Reset(void)
{	
	reg = SHT_SOFTRESET >> 8 & 0xFF; //0x30
	cmd = SHT_SOFTRESET & 0xFF; //0xA2
	WriteData(reg, &cmd, 1);
}

//---------------------------------------------------------------

//Temperature, Humidity Data Extraction
/*
 * description : temperature,Humidity 센서 데이터를 Datasheet에 해당하는 방식에 맞게 바꿔주고 temp와 hum 변수에 저장하는 함수
 */
void Temp_Hum_Extraction(void)
{
	reg = SHT_MEAS_HIGHREP >> 8 & 0xFF; //0x24
	cmd = SHT_MEAS_HIGHREP & 0xFF; //0x00
	WriteData(reg, &cmd, 1);
	usleep(1000*100);

	read(fd, Temp_Hum_Raw_Data, 5); //Data receiving format 8bit |Temp MSB|TEMPLSB|CRC|HUM MSB|HUM LSB|CRC|

	//Obtaining Temperature Data
	ST = Temp_Hum_Raw_Data[0]; //Front 8bit Temperature
	ST <<= 8;
	ST |= Temp_Hum_Raw_Data[1];

	//Obtaining Humidity Data
	SRH = Temp_Hum_Raw_Data[3];
	SRH <<= 8;
	SRH |= Temp_Hum_Raw_Data[4];

	//temperature Data Save
	temp = ST;
	temp *= 175;
	temp = temp / (0xFFFF - 0x0001);
	temp += -45;

	//Humidity Data Save
	hum = SRH;
	hum *= 100;
	hum = hum / (0xFFFF - 0x0001);  
}

//---------------------------------------------------------------

//SHT3X-DIS Temperature, Humidity Data Print
void Data_Print(void)
{	
	printf("│ Temperature: %.3lf °C │ Humidity: %.3lf %RH │\r\n", temp, hum);
}

//---------------------------------------------------------------

int main(int argc, char *argv[])
{	
	//초기 설정
	if((fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{
		perror("Failed to open i2c-2");
		exit(1);
	}

	I2C_Slave_Check();
	Reset();
	usleep(1000*100);	
	
	while(1)
	{		
		Temp_Hum_Extraction();
		Data_Print();
	}
	
	return 0;
}
