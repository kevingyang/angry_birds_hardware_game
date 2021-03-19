#include "timer.h"
#include "uart.h"
#include "i2c.h"
#include "trig.h"
#include "printf.h"

#include "LSM6DS33.h"
#include "imuread.h"

void main(void) {

    timer_init();
	uart_init();

    i2c_init();
	lsm6ds33_init();
//    fusion_init();

    printf("whoami=%x\n", lsm6ds33_get_whoami());

    AccelSensor_t accel;
    GyroSensor_t gyro;

    // test trig.h functions
    double answer = cos(0);
    printf("cos(0) = %d\n", (int)answer);
    answer = sin(0);
    printf("sin(0) = %d\n", (int)answer);
    answer = acosf(0.5);
    printf("acos(0.5) = %d\n", (int)answer);


	while(1) { 
        short x, y, z;
        short a, b, c;
        lsm6ds33_read_accelerometer(&x, &y, &z);
        lsm6ds33_read_gyroscope(&a, &b, &c);
        
        // 16384 is 1g (1g == 1000mg)
        x /= 16;
        y /= 16;
        x /= 16;
        printf("accel=(%dmg,%dmg,%dmg)\n", x, y, z);

        accel.Gp[0] = x;
        accel.Gp[1] = y;
        accel.Gp[2] = z;
        const AccelSensor_t *accel_ptr = &accel;

        gyro.Yp[0] = a;
        gyro.Yp[1] = b;
        gyro.Yp[2] = c;
        const GyroSensor_t *gyro_ptr = &gyro;

//        fusion_update(accel_ptr, NULL, gyro_ptr, NULL);
//        printf("new accel=(%dmg,%dmg,%dmg)\n\n", (int)accel_ptr->Gp[0], (int)accel_ptr->Gp[1], (int)accel_ptr->Gp[2]);

        //MahonyAHRSupdateIMU(a, b, c, x, y, z);
        //printf("corrr=(%dmg,%dmg,%dmg)\n\n", a, b, c);
       
        timer_delay_ms(500);
	}
}

