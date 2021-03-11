#include "assert.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "uart.h"
/* Constants required for trig functions */
#define CONST_PI  3.14159265
#define CONST_2PI 6.28318531
#define CONST_G 9.81
#define modd(x, y) ((x) - (int)((x) / (y)) * (y))

/* Constants for setting up screen width, height, etc. */
const static int SCREEN_WIDTH = 800;
const static int SCREEN_HEIGHT = 512;
const static int GROUND_Y = SCREEN_HEIGHT - 50;

/* 
 * Scale factors set in angry_nerds_graphics_init at beginning of program. 
 * These allow for scaling from virtual kinematics world to graphics world. 
 * 
 * x_scale_factor is defined as ratio between screen width and max width bird 
 * can be launched to (from projectile motion).
 * y_scale_factor is defined as ratio between screen height and max height bird
 * can be launched to (from projectile motion).
 */
static double x_scale_factor;
static double y_scale_factor;

static struct bird_position_t {
    int x, y;
} position;

const double SCALE_FACTOR = 250.0; // factor to scale the force vector length by to get velocity (TODO: change later, depending on the force value we get)


/*
 * Simple approximation of cos(x) using a 6th-order Taylor polynomial.
 * Citation: https://web.eecs.utk.edu/~azh/blog/cosine.html
 */
static inline double cos(double x)
{
    x = modd(x, CONST_2PI);
    char sign = 1;
    if (x > CONST_PI)
    {
        x -= CONST_PI;
        sign = -1;
    }
    double xx = x * x;

    return sign * (1 - ((xx) / (2)) + ((xx * xx) / (24)) - ((xx * xx * xx) / (720)) + ((xx * xx * xx * xx) / (40320)) - ((xx * xx * xx * xx * xx) / (3628800)) + ((xx * xx * xx * xx * xx * xx) / (479001600)));
}

/*
 * Use sinx = cos(x-pi/2).
 */
static inline double sin(double x) 
{
    return cos(x - CONST_PI/2);
}

/*
 * Use tanx = sinx/cosx.
 */
static inline double tan(double x) 
{
    return sin(x)/cos(x);
}


/*
 * Converts radians to degrees.
 */
static inline double deg_to_rad(int degrees) 
{
    return (double)(((double)degrees) * CONST_PI / 180);
}

/*
 * Plots the ground at the given ground y-value.
 */
static inline void gl_plot_ground(void) {
    gl_draw_rect(0, GROUND_Y, SCREEN_WIDTH, 2, GL_RED);
}

/*
 * Parameters: angle (in radians) and relative force, between 0 (no force)
 * and 1 (100% force))
 * Plots the initial velocity vector from a point on the ground.
 */
void gl_plot_initial_velocity_vector(double force, double angle) {
    // plot initial position
    gl_draw_rect(45, GROUND_Y - 5, 10, 10, GL_AMBER);

    // virtual dx and dy, faithful to the actual angle supplied
    int dy = (int)((force * SCALE_FACTOR) * sin(angle));
    int dx = (int)((force * SCALE_FACTOR) * cos(angle));

    int dy_scaled = dy * y_scale_factor;
    int dx_scaled = dx * x_scale_factor;

    // plot line for initial velocity vector
    gl_draw_line(50, GROUND_Y, 50 + dx_scaled, GROUND_Y - dy_scaled, GL_BLACK);

}


int calc_max_height(double force, double angle) {
    double velocity = force * SCALE_FACTOR;
    return (int) (velocity * velocity / (4 * CONST_G));
}


void gl_plot_trajectory(double force, double angle) {
    // set position to initial position
    position.x = 50;
    position.y = GROUND_Y;

    double velocity = force * SCALE_FACTOR;

    // use kinematics trajectory equation:
    //y = h + xtantheta - g/(2v^2cos^2theta)x^2
    while(position.x < SCREEN_WIDTH && position.y >= 0) {
        // virtual x and virtual y represent the actual values of x and y generated from the kinematics trajectory equation
        int virtual_x = (position.x - 50) / x_scale_factor;
        int virtual_y = (int) ((virtual_x * tan(angle) - CONST_G/(2 * velocity *velocity * cos(angle) * cos(angle)) * virtual_x * virtual_x)); // determine virtual y-position

        // actual x position is already stored in position.x
        // convert virtual y to actual y position and store in position.y
        position.y = GROUND_Y - virtual_y*y_scale_factor;

        gl_draw_pixel(position.x, position.y, GL_AMBER);
        // printf("virtual x: %d, virtual y: %d, actual x: %d, actual y: %d\n", virtual_x, virtual_y, position.x, position.y);

        position.x++; // increment to next x-value
    }
}

void angry_nerds_graphics_init(void)
{
    // calculate max height and width the bird can be launched to from kinematic principles.
    // maximum height results from maximum force (1.0) applied at 45 degree angle.
    int max_height = calc_max_height(1.0, deg_to_rad(45));
    int max_width = 4 * max_height;

    // set scale factor
    y_scale_factor = (double) GROUND_Y / max_height;
    x_scale_factor = (double) (SCREEN_WIDTH - 50) / max_width;
}

void main(void)
{
    uart_init();
    gl_init(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DOUBLEBUFFER);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55));
    timer_delay(3);

    angry_nerds_graphics_init();

    // plot ground
    gl_plot_ground();


    // plot trajectory of bird given angle and force
    gl_plot_trajectory(1.0, deg_to_rad(45));

    // plot initial velocity vector given angle and force
    gl_plot_initial_velocity_vector(1.0, deg_to_rad(45));


    // Show buffer with drawn contents
    gl_swap_buffer();

    uart_putchar(EOT);
}
