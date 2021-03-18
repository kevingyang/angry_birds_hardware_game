#ifndef PROJECT_APP_H
#define PROJECT_APP_H

struct position_t {
    int x, y;
};

/* bird_position position struct stores the coordinates of bird's current position, and target_position stores the coordinates of the target's position. */
struct position_t bird_position;
struct position_t target_position;
unsigned int TARGET_SIZE; // stores the size of the target (the width/height of the square)

/* Constants for setting up screen width, height, etc. */
int SCREEN_WIDTH; // set the screen width and height to match your screen size!
int SCREEN_HEIGHT;
int GROUND_Y; // ground is arbitrarily set to 50 px above bottom of screen

/* 
 * Scale factors are set in angry_nerds_graphics_init at beginning of program. 
 * These allow for scaling from virtual kinematics world to graphics world. 
 * 
 * x_scale_factor is defined as ratio between screen width and max width bird 
 * can be launched to (from projectile motion).
 * y_scale_factor is defined as ratio between screen height and max height bird
 * can be launched to (from projectile motion).
 */
double x_scale_factor;
double y_scale_factor;

double velocity_scale_factor; // factor to scale the force vector length by to get velocity (TODO: change later, depending on the force value we get)

/* Keyboard initialization setup */
#define KEYBOARD_CLOCK GPIO_PIN3
#define KEYBOARD_DATA GPIO_PIN4

/* SPI setup */
#define CE0 GPIO_PIN8
#define TX GPIO_PIN10   // input
#define RX GPIO_PIN9    // output
#define CLK GPIO_PIN11
#define FSR_CHANNEL 0

void gl_plot_ground(int ground_y_value);
void gl_plot_initial_velocity_vector(double force, double angle, color_t color);
void gl_plot_trajectory(double force, double angle, color_t color);
int calc_max_height(double force, double angle);
void gl_draw_image(unsigned int x, unsigned int y, char first_initial);
unsigned int gl_plot_image_trajectory(double force, double angle, char first_initial);
void gl_draw_target(unsigned int leftBound, unsigned int size);
unsigned int adc_read(void);

void angry_nerds_graphics_init(void);
void angry_nerds_game_init(void);
void angry_nerds_game_start(unsigned int difficulty);

#endif
