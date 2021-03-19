#include "assert.h"
#include "printf.h"
#include "gl.h"
#include "gpio.h"
#include "timer.h"
#include "uart.h"
#include "project-app.h"
#include "trig.h"
#include "image.h"
#include "randomHardware.h"
#include "spi.h"

void test_trajectory_different_forces (void) {
    for(int i = 1; i <= 10; i++) {
        double force = 1.0 / i;
        
        // plot trajectory of bird given angle and force
        gl_plot_trajectory(force, deg_to_rad(45), GL_AMBER);
        // plot initial velocity vector given angle and force
        gl_plot_initial_velocity_vector(force, deg_to_rad(45), GL_BLUE);
    }
}

void test_trajectory_different_angles (void) {
    for(int i = 0; i < 10; i++) {
        // plot trajectory of bird given angle and force
        gl_plot_trajectory(1.0, deg_to_rad(i * 9), GL_AMBER);
        // plot initial velocity vector given angle and force
        gl_plot_initial_velocity_vector(1.0, deg_to_rad(i * 9), GL_BLUE);
    }
}

void test_draw_images (void) {
    // Test drawing the six staff member images
    gl_draw_image(0, 0  , 'j'); // julie
    gl_draw_image(0, 100, 'p'); // pat
    gl_draw_image(0, 200, 's'); // sean
    gl_draw_image(0, 300, 'e'); // peter
    gl_draw_image(0, 400, 'a'); // anna
    gl_draw_image(0, 500, 'l'); // liana

}

void test_plot_image_trajectory(void) {
    // throw julie at 60 degrees
    gl_plot_image_trajectory(1.0, deg_to_rad(60), 'j');

    // plot trajectory of bird given angle and force
    gl_plot_trajectory(1.0, deg_to_rad(60), GL_AMBER);
    gl_swap_buffer(); // plot on both framebuffers
    gl_plot_trajectory(1.0, deg_to_rad(60), GL_AMBER);
    gl_swap_buffer();

    // plot initial velocity vector given angle and force
    gl_plot_initial_velocity_vector(1.0, deg_to_rad(60), GL_BLACK);
    gl_swap_buffer(); // plot on both framebuffers
    gl_plot_initial_velocity_vector(1.0, deg_to_rad(60), GL_BLACK);
    gl_swap_buffer();

    // throw pat at 45 degrees
    gl_plot_image_trajectory(1.0, deg_to_rad(45), 'p');

    // plot trajectory of bird given angle and force
    gl_plot_trajectory(1.0, deg_to_rad(45), GL_AMBER);
    gl_swap_buffer(); // plot on both framebuffers
    gl_plot_trajectory(1.0, deg_to_rad(45), GL_AMBER);
    gl_swap_buffer();

    // plot initial velocity vector given angle and force
    gl_plot_initial_velocity_vector(1.0, deg_to_rad(45), GL_BLACK);
    gl_swap_buffer(); // plot on both framebuffers
    gl_plot_initial_velocity_vector(1.0, deg_to_rad(45), GL_BLACK);
    gl_swap_buffer();

}

void test_gl_draw_target(void) {
    // 20 size-20 targets,  right half of screen
    for (int i = 0; i < 20; i++) {
        gl_draw_target(SCREEN_WIDTH / 2, 20);
    }
}

void test_force_sensor(void) {
    spi_init(SPI_CE0, 125000000);
    while (1) {
        unsigned int val = read_force();
        printf("%d\n", val);
    }
}

void main(void)
{
    uart_init();
    angry_nerds_graphics_init();
    gl_init(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DOUBLEBUFFER);
    gl_plot_ground(GROUND_Y);
    random_init();

    /* TESTS BELOW */
//    test_draw_images();
//    test_trajectory_different_angles();
//    test_trajectory_different_forces();
//    test_plot_image_trajectory();
//    test_gl_draw_target();
    test_force_sensor();
    /* TESTS ABOVE */

    // Final step: buffer with drawn contents
    gl_swap_buffer();

    uart_putchar(EOT);
}
