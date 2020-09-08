#include "I2Cconf.h"
#include "./Si1145/Si1145.h"
#include "./SHT35-DIS/SHT35-DIS.h"
#include "./MAX30101/SPO2.h"
#include "./APDS-9960/APDS9960.h"


uint16_t uv, visible;
uint8_t reg, cmd;
uint8_t Temp_Hum_Raw_Data[5];
uint16_t ST, SRH;
double temp, hum;
//-----------------------------------------------------------

//SI1145 Sensor Check

void I2C_Slave_Check(void)
{	
    if(ioctl(fd, I2C_SLAVE, SI1145_I2C_ADDRESS) < 0)
	{
        printf("Failed to acquire bus access and/or talk to slave\n");
        exit(1);
    }
	if(write(fd, NULL, 0) < 0)
	{
		printf("[SI1145(0x70)] I2C Sensor Is Missing\n");
		exit(1);
	}
	else
	{
		printf("Check OK!! [SI1145(0x60)] I2C Sensor\n");
	}
}

//-----------------------------------------------------------

/* [in] : p는 쓰려는 인자 RAM, v는 쓰려는 인자
 * description : 센서에 필요한 parmeter를 해당하는 register에 작성하는 함수
 */
void WriteParam(uint8_t p, uint8_t v)
{
    uint8_t data = v;
    WriteData(SI1145_REG_PARAMWR, &v, 1);
    
	data = p | 0xA0;
    WriteData(SI1145_REG_COMMAND, &data, 1);
}

//-----------------------------------------------------------

void Reset(void)
{
	uint8_t data;
    
	data = 0x00;
    WriteData(SI1145_REG_MEASRATE0, &data, 1);
    WriteData(SI1145_REG_MEASRATE1, &data, 1);
    WriteData(SI1145_REG_INTCFG, &data, 1);  
	WriteData(SI1145_REG_IRQEN, &data, 1);
    
	data = 0xFF;
    WriteData(SI1145_REG_IRQSTAT, &data, 1);
    
	data = 0x01;
    WriteData(SI1145_REG_COMMAND, &data, 1);
    usleep(1000 * 10); //10msec 대기
    
	data = 0x17;
    WriteData(SI1145_REG_HWKEY, &data, 1);

    usleep(1000 * 10); 
}

//-----------------------------------------------------------

void Init(void)
{    
	uint8_t data;

	data = 0x29;
    WriteData(SI1145_REG_UCOEFF0, &data, 1);
    
	data = 0x89;
    WriteData(SI1145_REG_UCOEFF1, &data ,1);
    
	data = 0x02;
    WriteData(SI1145_REG_UCOEFF2, &data, 1);
    
	data = 0x00;
    WriteData(SI1145_REG_UCOEFF3, &data, 1);

    // UV 센서 사용을 허용해주는 파라미터 설정
    WriteParam(SI1145_PARAM_CHLIST, SI1145_PARAM_CHLIST_ENUV |
    SI1145_PARAM_CHLIST_ENALSVIS | SI1145_PARAM_CHLIST_ENPS1);

    data = 0x03;
    WriteData(SI1145_REG_PSLED21, &data, 1); // LED 1에만 20mA
    WriteParam(SI1145_PARAM_PS1ADCMUX, SI1145_PARAM_ADCMUX_LARGEIR);
	
    // 근접 감지 1센서와 LED 1번 사용하는 파라미터 설정
    WriteParam(SI1145_PARAM_PSLED12SEL, SI1145_PARAM_PSLED12SEL_PS1LED1);
    WriteParam(SI1145_PARAM_PSADCGAIN, 0);
    WriteParam(SI1145_PARAM_PSADCOUNTER, SI1145_PARAM_ADCCOUNTER_511CLK);
    WriteParam(SI1145_PARAM_PSADCMISC, SI1145_PARAM_PSADCMISC_RANGE|
        SI1145_PARAM_PSADCMISC_PSMODE);

    WriteParam(SI1145_PARAM_ALSVISADCGAIN, 1);
    WriteParam(SI1145_PARAM_ALSVISADCOUNTER, SI1145_PARAM_ADCCOUNTER_511CLK);
    WriteParam(SI1145_PARAM_ALSVISADCMISC, SI1145_PARAM_ALSVISADCMISC_VISRANGE);

    /************************/

    // 자동 측정
    data = 0xFF;
    WriteData(SI1145_REG_MEASRATE0, &data, 1); // 255 * 31.25uS = 8ms
    
    // 자동 실행
    data = 0x0F;
    WriteData(SI1145_REG_COMMAND, &data, 1);
}

//-----------------------------------------------------------

//UV_Visible_IR_Prox_Extraction
/*
 * description : UV, Visible 센서 데이터를 Datasheet에 해당하는 방식에 맞게 바꿔주고 uv와 visible 변수에 저장하는 함수
 */
void UV_Visible_IR_Prox_Extraction(void)
{
    uint8_t buf[2];
    
    uint16_t tmp;
	
	//UV Extraction
    ReadData(SI1145_REG_UVINDEX0, buf, 2);
	tmp = buf[1] << 8;
    uv = (buf[0] + tmp) / 100; //해당하는 데이터에 100을 나눠주어야 uv값을 정확히 읽을 수 있다.
	
	//Visible Extraction
    ReadData(SI1145_REG_ALSVISDATA0, buf, 2);
    tmp = buf[1] << 8;
    visible = (buf[0] + tmp);
}

//-----------------------------------------------------------

//SI1145 UV Visible_Light IR Proximity Print
void Data_Print(void)
{	
	printf("UV: %hu, Visible Light: %hu lx\n", uv, visible);
}

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
    //초기설정
	if((fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{
		perror("Failed to open i2c-2");
		exit(1);
	}

	I2C_Slave_Check();
	Reset();
	Init();
	
    
    //초기 설정
	if((fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{
		perror("Failed to open i2c-2");
		exit(1);
	}

	I2C_Slave_Check();
	Reset();

    //100msec 대기
	usleep(1000*100);	
	
	while(1)
	{		
		UV_Visible_IR_Prox_Extraction();
		Data_Print();
		
		usleep(1000*100);
	}
	while(1)
	{		
		Temp_Hum_Extraction();
		Data_Print();
	}
	
	return 0;
}
