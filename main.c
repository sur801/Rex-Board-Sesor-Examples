#include <SHT35-DIS.h>
#include <MAX30105.h>
#include <APDS9960.h>
#include <MAX30101.h>
#include <Si1145.h>
#include <mcp3021_ra12p.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

// 6가지 센서 라이브러리를 불러와 테스트 해보는 main 파일.

void MAX30105_Setting(){
    // setting for MAX30105 SENSOR
    if((MAX30105_fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{	
		perror("Failed to open i2c-2\n");
		exit(1);
	}
	MAX30105_I2C_MUX();
	MAX30105_I2C_Slave_Check();
	MAX30105_Reset(); 
	MAX30105_Init();
	MAX30105_Data_Setup();

}

void MAX30101_Setting(){
	// setting for MAX30101 SENSOR
	if((MAX30101_fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{
        printf("Failed to open i2c-2");
        exit(1);
    }

	MAX30101_I2C_MUX();
	MAX30101_I2C_Slave_Check();
	MAX30101_Reset(); 
	MAX30101_Init();
    MAX30101_Data_Setup();
}


void SHT35_Setting(){
    // setting for SHT35-DIS SENSOR
    if((SHT35_DIS_fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{	
		perror("Failed to open i2c-2\n");
		exit(1);
	}
	SHT35_I2C_Slave_Check();
	SHT35_Reset();
}
void APDS9960_Setting(){
    if((APDS_9960_fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{
        perror("Failed to open i2c-2");
        exit(1);
    }
	
    if(ioctl(APDS_9960_fd, I2C_SLAVE, APDS9960_SLAVE_ADDRESS) < 0) 
	{
        perror("Failed to acquire bus access and/or talk to slave\n");
        exit(1);
    }
	
    if(!APDS9960_Init()) 
	{
        printf("[APDS-9960(0x39)] I2C Sensor Is Missing\n\n");
        exit(1);
    }	
	
	printf("Check OK!! [APDS-9960(0x39)] I2C Sensor\n\n");
	
	if(!APDS9960_enableProximity(0))
	{
		printf("Enable Proximity Sensor Failed\n");
		exit(1);
	}
	
	if(!APDS9960_enableGesture())
	{
		printf("Enable Gesture Sensor Failed\n");
		exit(1);
	}

}

void mcp3021_ra12p_Setting(){
	
	if((mcp3021_fd = open("/dev/i2c-2", O_RDWR)) < 0)
	{
		perror("Failed to open i2c-2");
		exit(1);
	}	
	
    if(ioctl(mcp3021_fd, I2C_SLAVE, MCP3021_SLAVE_ADDRESS) < 0) 
	{
        	perror("Failed to acquire bus access and/or talk to slave\n");
       		exit(1);
    	}
	
	printf("Searching For I2C Sensor [MCP3021-RA12P(0x4D)]\n");

}

void Si1145_Setting(){
	if((Si1145_fd = open("/dev/i2c-2", O_RDWR)) < 0) 
	{
		perror("Failed to open i2c-2");
		exit(1);
	}

	Si1145_I2C_Slave_Check();
	Si1145_Reset();
	Si1145_Init();
	usleep(1000*100);	
}

int main(void) {
	
    MAX30105_Setting();
    SHT35_Setting();
    APDS9960_Setting();
	MAX30101_Setting();
	mcp3021_ra12p_Setting();
	Si1145_Setting();

    while(1) {
        //SHT35-DIS SENSOR DATA PRINT
        Temp_Hum_Extraction();
		SHT35_Data_Print();
        //MAX30105 SENSOR DATA PRINT
        MAX30105_Data_Analisis();
        //APDS9960 SENSOR DATA PRINT
        APDS9960_printGesture();
		//MAX30101 SENSOR DATA PRINT
		MAX30101_Data_Analisis();
		//MCP3021_RA12P SENSOR DATA PRINT
		mcp3021_Data_Print(mcp3021_fd);
		//Si1145 SENSOR DATA PRINT
		UV_Visible_IR_Prox_Extraction();
		Si1145_Data_print();
		usleep(1000*100);


    }
}
