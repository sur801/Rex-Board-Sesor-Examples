#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>

#include "APDS9960.h"

int APDS9960_setProximityGain(uint8_t drive)
{
    uint8_t val;
    
    if( !ReadData(APDS9960_CONTROL, &val, 1) ) {
        return 0;
    }
    
    drive &= 0b00000011;
    drive = drive << 2;
    val &= 0b11110011;
    val |= drive;
    
    if( !WriteData(APDS9960_CONTROL, &val, 1) ) {
        return 0;
    }
    
    return 1;
}

int APDS9960_setProximityIntEnable(uint8_t enable)
{
    uint8_t val;
    
    if( !ReadData(APDS9960_ENABLE, &val, 1) ) {
        return 0;
    }
    
    enable &= 0b00000001;
    enable = enable << 5;
    val &= 0b11011111;
    val |= enable;
    
    if( !WriteData(APDS9960_ENABLE, &val, 1) ) {
        return 0;
    }
    
    return 1;
}

int APDS9960_enableProximity(int interrupt) {
    if( !APDS9960_setProximityGain(PGAIN_1X) ) {
        return 0;
    }
    if( !APDS9960_setLEDDrive(DEFAULT_LDRIVE) ) {
        return 0;
    }
    if( interrupt ) {
        if( !APDS9960_setProximityIntEnable(1) ) {
            return 0;
        }
    } else {
        if( !APDS9960_setProximityIntEnable(0) ) {
            return 0;
        }
    }
    if( !APDS9960_enablePower() ){
        return 0;
    }
    if( !APDS9960_setMode(PROXIMITY, 1) ) {
        return 0;
    }
}

/* APDS9960_readProximity
 * description :ReadData 함수를 통해 지정된 주소에서 근접도 값을 계속 읽어옴.
*/
int APDS9960_readProximity(uint8_t *val)
{
    *val = 0;
    
    if( !ReadData(APDS9960_PDATA, val, 1) ) {
        return 0;
    }
    
    return 1;
}
/* APDS9960_printProximity
 * description : APDS9960_printProximity 함수를 통해 읽어온 근접도를 출력해줌.
*/
void APDS9960_printProximity(void)
{
    uint8_t prox;
    
    if( !APDS9960_readProximity(&prox)) {
		printf("ERROR\n");
    } else {
		printf("Proximity: %d\n", prox);    
	}
}
