#include "assert.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "uart.h"
#include "trig.h"
#include "image.h"
#include "project-app.h"
#include "randomHardware.h"


/*
 * Plots the ground at the given ground y-value.
 */
void gl_plot_ground(int ground_y_value) {
    gl_draw_rect(0, ground_y_value, SCREEN_WIDTH, 2, GL_RED);
}

/*
 * Parameters: angle (in radians) and relative force, between 0 (no force)
 * and 1 (100% force))
 * Plots the initial velocity vector from a point on the ground.
 */
void gl_plot_initial_velocity_vector(double force, double angle, color_t color) {
    // plot initial position
    gl_draw_rect(45, GROUND_Y - 5, 10, 10, GL_AMBER);

    // virtual dx and dy, faithful to the actual angle supplied
    int dy = (int)((force * velocity_scale_factor) * sin(angle));
    int dx = (int)((force * velocity_scale_factor) * cos(angle));

    int dy_scaled = dy * y_scale_factor;
    int dx_scaled = dx * x_scale_factor;

    // plot line for initial velocity vector
    gl_draw_line(50, GROUND_Y, 50 + dx_scaled, GROUND_Y - dy_scaled, color);

}


void gl_plot_trajectory(double force, double angle, color_t color) {
    // set position to initial position
    position.x = 50;
    position.y = GROUND_Y;

    double velocity = force * velocity_scale_factor;

    // use kinematics trajectory equation:
    //y = h + xtantheta - g/(2v^2cos^2theta)x^2
    while(position.x < SCREEN_WIDTH && position.y >= 0) {
        // virtual x and virtual y represent the actual values of x and y generated from the kinematics trajectory equation
        int virtual_x = (position.x - 50) / x_scale_factor;
        int virtual_y = (int) ((virtual_x * tan(angle) - CONST_G/(2 * velocity *velocity * cos(angle) * cos(angle)) * virtual_x * virtual_x)); // determine virtual y-position

        // actual x position is already stored in position.x
        // convert virtual y to actual y position and store in position.y
        position.y = GROUND_Y - virtual_y*y_scale_factor;

        gl_draw_pixel(position.x, position.y, color);
        // printf("virtual x: %d, virtual y: %d, actual x: %d, actual y: %d\n", virtual_x, virtual_y, position.x, position.y);

        position.x++; // increment to next x-value
    }
}


void gl_plot_image_trajectory(double force, double angle, color_t color) {
    // set position to initial position
    position.x = 50;
    position.y = GROUND_Y;

    double velocity = force * velocity_scale_factor;

    // use kinematics trajectory equation:
    //y = h + xtantheta - g/(2v^2cos^2theta)x^2
    while(position.x < SCREEN_WIDTH && position.y >= 0) {
        // virtual x and virtual y represent the actual values of x and y generated from the kinematics trajectory equation
        int virtual_x = (position.x - 50) / x_scale_factor;
        int virtual_y = (int) ((virtual_x * tan(angle) - CONST_G/(2 * velocity *velocity * cos(angle) * cos(angle)) * virtual_x * virtual_x)); // determine virtual y-position

        // actual x position is already stored in position.x
        // convert virtual y to actual y position and store in position.y
        position.y = GROUND_Y - virtual_y*y_scale_factor;

        gl_draw_pixel(position.x, position.y, color);
        // printf("virtual x: %d, virtual y: %d, actual x: %d, actual y: %d\n", virtual_x, virtual_y, position.x, position.y);

        position.x++; // increment to next x-value
    }
}

/*
 * Given the force and angle the bird is launched from, this helper function 
 * calculates the maximum height the bird ascends to by kinematic principles.
 */
int calc_max_height(double force, double angle) {
    double velocity = force * velocity_scale_factor;
    return (int) (velocity * velocity / (2 * CONST_G));
}

void angry_nerds_graphics_init(void)
{
    // 1. Set initial screen parameters.
    SCREEN_WIDTH = 1400;
    SCREEN_HEIGHT = 900;
    GROUND_Y = SCREEN_HEIGHT - 50;
    velocity_scale_factor = 250.0; // (TODO: change later, depending on the force value we get)
    
    // 2. Calculate max height and width the bird can be launched to from kinematic principles.
    // maximum height results from maximum force (1.0) applied at 45 degree angle.
    int max_height = calc_max_height(1.0, deg_to_rad(45));
    int max_width = 2 * max_height;

    // 3. Set scale factors to scale from kinematics world -> graphical world
    y_scale_factor = (double) GROUND_Y / max_height;
    x_scale_factor = (double) (SCREEN_WIDTH - 50) / max_width;
}


/*
 * Given the force and angle the bird is launched from, this helper function 
 * calculates the maximum height the bird ascends to by kinematic principles.
 */
void gl_draw_image(unsigned int x, unsigned int y, char first_initial)
{
    unsigned int buf[get_image_size()];
    get_image_to_draw(buf, sizeof(buf), first_initial);
    unsigned int (*img)[get_image_width()] = (unsigned int (*)[get_image_width()]) buf; 

    for (int j = y; j < get_image_height() + y; j++) {
        for (int i = x; i < get_image_width() + x; i++) {
            gl_draw_pixel(i, j, img[j - y][i - x]);
        }
    }
}

/* 
 * Generate randomly-located target to the right of a given vertical line
 */ 
#define TARGETSIZE 20
void gl_draw_target(unsigned int bound)
{
    // generate x-coordinate
    unsigned int x = random_getNumber(SCREEN_WIDTH - TARGETSIZE, bound); 
    // generate y-coordinate
    unsigned int y = random_getNumber(GROUND_Y - TARGETSIZE, 0);
    // plot target
    gl_draw_rect(x, y, TARGETSIZE, TARGETSIZE, GL_WHITE);
}


void main (void)
{
    uart_init();
    random_init();

    angry_nerds_graphics_init();
    gl_init(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DOUBLEBUFFER);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55));

    // plot ground
    gl_plot_ground(GROUND_Y);

    // plot trajectory of bird given angle and force
    gl_plot_trajectory(1.0, deg_to_rad(45), GL_AMBER);

    // plot initial velocity vector given angle and force
    gl_plot_initial_velocity_vector(1.0, deg_to_rad(45), GL_BLACK);

    gl_draw_image(0, 0, 'j');
    gl_draw_image(0, 100, 'p');
    gl_draw_image(0, 200, 's');
    gl_draw_image(0, 300, 'e');
    gl_draw_image(0, 400, 'a');
    gl_draw_image(0, 500, 'l');

    // plot target
    for (int i = 0; i < 25; i++) {
        gl_draw_target(SCREEN_WIDTH / 2);
    }

    // Show buffer with drawn contents
    gl_swap_buffer();

    uart_putchar(EOT);
}
