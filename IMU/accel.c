#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include "printf.h"

#include "LSM6DS33.h"
#include "MahonyAHRS.h"

void main(void) {

    timer_init();
	uart_init();

    i2c_init();
	lsm6ds33_init();

    printf("whoami=%x\n", lsm6ds33_get_whoami());

	while(1) { 
        short x, y, z;
        short a, b, c;
        lsm6ds33_read_accelerometer(&x, &y, &z);
        lsm6ds33_read_gyroscope(&a, &b, &c);

        // 16384 is 1g (1g == 1000mg)
        x /= 16;
        y /= 16;
        x /= 16;
        printf("gyro=(%dmg,%dmg,%dmg)\n", x, y, z);

        //MahonyAHRSupdateIMU(a, b, c, x, y, z);
        //printf("corrr=(%dmg,%dmg,%dmg)\n\n", a, b, c);
       
        timer_delay_ms(500);
	}
}

