#include <SHT35-DIS.h>
#include <MAX30105.h>
#include <APDS9960.h>
#include <MAX30101.h>
#include <Si1145.h>
#include <mcp3021_ra12p.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>

/* 
 * 6가지 센서 library를 include해서 테스트 해보는 main 파일.
 * 6개의 센서 폴더에서 각각 make를 해주어야 해당 센서의 library가 생성됨.
 * 6개의 센서 폴더에 각각 생성된 library가 있어야만 include해서 정상적으로 실행 가능. 
*/ 

//연기(입자) 변화량 측정 센서 설정
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

//심박수, 산소포화도 측정 센서 설정
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

//온도, 습도 측정 센서 설정
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

//제스처 센서 설정
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

//압력 측정 센서 설정
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

//자외선, 적외선, 가시광 센서 설정
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

/* main 함수에서 원하는 센서의 setting 함수와 Print data를 
  제외하고, 나머지는 주석해서 사용한다.
*/
int main(void) {

	//6가지 센서 설정값 초기화
    MAX30105_Setting(); // 연기(입자) 변화량 측정 센서 설정
    SHT35_Setting(); // 온도, 습도 데이터 측정 센서 설정
    APDS9960_Setting(); // 제스처 감지 센서 설정
	MAX30101_Setting(); // 심박수, 산소포와도 측정 센서 설정
	mcp3021_ra12p_Setting(); // 압력 측정 센서 설정
	Si1145_Setting(); // 자외선, 적외선, 가시광 센서 설정

    while(1) {
        
		// 온도, 습도 출력
        Temp_Hum_Extraction();
		SHT35_Data_Print();

        // 연기(입자) 변화량 출력
        MAX30105_Data_Analisis();

        // 인식된 제스처 출력
        APDS9960_printGesture();

		//심장박동, 산소 포화도 출력
		MAX30101_Data_Analisis();

		//압력 값 출력
		mcp3021_Data_Print(mcp3021_fd);
		
		// 자외선, 가시광 출력
		UV_Visible_IR_Prox_Extraction();
		Si1145_Data_print();
		usleep(1000*100);
    }
}
