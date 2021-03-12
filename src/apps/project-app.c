#include "assert.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "uart.h"
#include "trig.h"

/* Constants for setting up screen width, height, etc. */
const static int SCREEN_WIDTH = 1400; // set the screen width and height to match your screen size!
const static int SCREEN_HEIGHT = 900;
const static int GROUND_Y = SCREEN_HEIGHT - 50; // ground is arbitrarily set to 50 px above bottom of screen

/* 
 * Scale factors are set in angry_nerds_graphics_init at beginning of program. 
 * These allow for scaling from virtual kinematics world to graphics world. 
 * 
 * x_scale_factor is defined as ratio between screen width and max width bird 
 * can be launched to (from projectile motion).
 * y_scale_factor is defined as ratio between screen height and max height bird
 * can be launched to (from projectile motion).
 */
static double x_scale_factor;
static double y_scale_factor;

const static double velocity_scale_factor = 250.0; // factor to scale the force vector length by to get velocity (TODO: change later, depending on the force value we get)

/* bird_position_t position struct stores the coordinates of bird's current position. */
static struct bird_position_t {
    int x, y;
} position;

/*
 * Plots the ground at the given ground y-value.
 */
static inline void gl_plot_ground(int ground_y_value) {
    gl_draw_rect(0, ground_y_value, SCREEN_WIDTH, 2, GL_RED);
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
    int dy = (int)((force * velocity_scale_factor) * sin(angle));
    int dx = (int)((force * velocity_scale_factor) * cos(angle));

    int dy_scaled = dy * y_scale_factor;
    int dx_scaled = dx * x_scale_factor;

    // plot line for initial velocity vector
    gl_draw_line(50, GROUND_Y, 50 + dx_scaled, GROUND_Y - dy_scaled, GL_BLACK);

}


void gl_plot_trajectory(double force, double angle) {
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

        gl_draw_pixel(position.x, position.y, GL_AMBER);
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
    // calculate max height and width the bird can be launched to from kinematic principles.
    // maximum height results from maximum force (1.0) applied at 45 degree angle.
    int max_height = calc_max_height(1.0, deg_to_rad(45));
    int max_width = 2 * max_height;

    // set scale factors to scale from kinematics world -> graphical world
    y_scale_factor = (double) GROUND_Y / max_height;
    x_scale_factor = (double) (SCREEN_WIDTH - 50) / max_width;
}

void main(void)
{
    uart_init();
    gl_init(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DOUBLEBUFFER);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55));

    angry_nerds_graphics_init();

    // plot ground
    gl_plot_ground(GROUND_Y);

    // plot trajectory of bird given angle and force
    gl_plot_trajectory(1.0, deg_to_rad(45));

    // plot initial velocity vector given angle and force
    gl_plot_initial_velocity_vector(1.0, deg_to_rad(45));

    // Show buffer with drawn contents
    gl_swap_buffer();

    uart_putchar(EOT);
}
