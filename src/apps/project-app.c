#include "assert.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "uart.h"

#define CONST_PI  3.14159265
#define CONST_2PI 6.28318531
#define modd(x, y) ((x) - (int)((x) / (y)) * (y))



const static int SCREEN_WIDTH = 800;
const static int SCREEN_HEIGHT = 512;
const static int GROUND_Y = SCREEN_HEIGHT - 50;

static struct bird_position_t {
    int x, y;
} position;


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
void gl_plot_initial_vector(double force, double angle) {
    // initial position
    gl_draw_rect(50, GROUND_Y, 10, 10, GL_AMBER);

    const double SCALE_FACTOR = 200.0; // factor to scale the force vector length by (TODO: change later, depending on the force value we get)

    int dy = (int)((force * SCALE_FACTOR) * sin(angle));
    int dx = (int)((force * SCALE_FACTOR) * cos(angle));

    gl_draw_line(50, GROUND_Y, 50 + dx, GROUND_Y - dy, GL_BLACK);

}


void gl_plot_trajectory(int angle, int force) {
    // initial position
    position.x = 50;
    position.y = GROUND_Y;
}

void main(void)
{

    uart_init();
    gl_init(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DOUBLEBUFFER);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55));
    timer_delay(3);

    // plot ground
    gl_plot_ground();

    // plot initial vector from starting point given angle and force
    gl_plot_initial_vector(2.0, deg_to_rad(30));

    // Show buffer with drawn contents
    gl_swap_buffer();

    printf("Hello, world!\n");
    uart_putchar(EOT);
}
