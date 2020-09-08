#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <string.h>

/*"/dev/i2c-0"'s file descriptor*/
int fd;

//Sensor Register Data Write
/* [in] :   reg_addr - 사용하는 센서의 register 주소, data - reg_addr의 주소에서 읽어온 데이터를 써줄 목적지 주소
            size - reg_addr에서 읽어올 데이터의 크기
 * [out] :  WriteData 함수를 잘 실행했다는 의미로 1을 return 해줌.
 * description : 센서의 register 주소에서 size 만큼 byte를 읽어와 data 매개변수의 주소에 찾아가 써준다.
 */
int WriteData(uint8_t reg_addr, uint8_t *data, int size) 
{
    
    uint8_t *buf;
    buf = (uint8_t*)malloc(size + 1);
    buf[0] = reg_addr;
    memcpy(buf + 1, data, size);
    write(fd, buf, size + 1);
    free(buf); 

    return 1;
}
//-----------------------------------------------------------

//Register Data Read
/* [in] :   reg_addr - 사용하는 센서의 register 주소, data - reg_addr의 주소에서 읽어온 데이터를 써줄 목적지 주소,
            size - reg_addr에서 읽어올 데이터의 크기
 * [out] :  ReatData 함수를 잘 실행했다는 의미로 1을 return 해줌.
 * description : 센서의 register 주소에서 size 만큼의 data 를 읽는 함수.
 */

int ReadData(uint8_t reg_addr, uint8_t *data, int size) 
{
    write(fd, &reg_addr, 1);
    read(fd, data, size);

    return 1;
}