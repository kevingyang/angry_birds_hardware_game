#ifndef PROJECT_APP_H
#define PROJECT_APP_H

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

/* bird_position_t position struct stores the coordinates of bird's current position. */
struct bird_position_t {
    int x, y;
} position;


void gl_plot_ground(int ground_y_value);
void gl_plot_initial_velocity_vector(double force, double angle);
void gl_plot_trajectory(double force, double angle);
int calc_max_height(double force, double angle);
void gl_draw_image(unsigned int x, unsigned int y, char first_initial);
void angry_nerds_graphics_init(void);

#endif