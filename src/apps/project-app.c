#include "assert.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "uart.h"
#include "trig.h"
#include "image.h"
#include "project-app.h"
#include "keyboard.h"
#include "randomHardware.h" // for random function


/*
 * Plots the ground at the given ground y-value on both buffers.
 */
void gl_plot_ground(int ground_y_value) {
    gl_draw_rect(0, ground_y_value, SCREEN_WIDTH, 2, GL_RED);
    gl_swap_buffer();
    gl_draw_rect(0, ground_y_value, SCREEN_WIDTH, 2, GL_RED);
    gl_swap_buffer();

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
    bird_position.x = 50;
    bird_position.y = GROUND_Y;

    double velocity = force * velocity_scale_factor;

    // use kinematics trajectory equation:
    //y = h + xtantheta - g/(2v^2cos^2theta)x^2
    while(bird_position.x < SCREEN_WIDTH && bird_position.y >= 0) {
        // virtual x and virtual y represent the actual values of x and y generated from the kinematics trajectory equation
        int virtual_x = (bird_position.x - 50) / x_scale_factor;
        int virtual_y = (int) ((virtual_x * tan(angle) - CONST_G/(2 * velocity *velocity * cos(angle) * cos(angle)) * virtual_x * virtual_x)); // determine virtual y-position

        // actual x position is already stored in bird_position.x
        // convert virtual y to actual y position and store in bird_position.y
        bird_position.y = GROUND_Y - virtual_y*y_scale_factor;

        gl_draw_pixel(bird_position.x, bird_position.y, color);
        // printf("virtual x: %d, virtual y: %d, actual x: %d, actual y: %d\n", virtual_x, virtual_y, position.x, position.y);

        bird_position.x++; // increment to next x-value
    }
}


void gl_plot_image_trajectory(double force, double angle, char first_initial) {
    // set position to initial position
    bird_position.x = 50;

    double velocity = force * velocity_scale_factor;

    struct position_t previous_position;

    /* 
     * To calculate trajectory, use kinematics projectile motion equation below:
     * y = h + xtantheta - g/(2v^2cos^2theta)x^2
     */
    while(bird_position.x < SCREEN_WIDTH && bird_position.y >= 0) {

        // virtual x and virtual y represent the actual values of x and y generated from the kinematics trajectory equation
        int virtual_x = (bird_position.x - 50) / x_scale_factor;
        int virtual_y = (int) ((virtual_x * tan(angle) - CONST_G/(2 * velocity *velocity * cos(angle) * cos(angle)) * virtual_x * virtual_x)); // determine virtual y-position

        // actual x position is already stored in position.x
        // convert virtual y to actual y position and store in position.y
        // must scale y-position by image's height
        bird_position.y = GROUND_Y - get_image_height() - virtual_y*y_scale_factor;

        gl_draw_image(bird_position.x, bird_position.y, first_initial); // draw next

        /* Change the length of timer delay here to affect how fast the projectile moves across the screen. */
        timer_delay_ms(100); // delay 0.1 second before removing and going to next iteration of loop

        // after the first iteration, we must remove the projectile at the previous position
        if(bird_position.x != 50) {
            gl_draw_rect(previous_position.x, previous_position.y, get_image_width(), get_image_height(), GL_BLACK);
            gl_swap_buffer(); 
            gl_draw_rect(previous_position.x, previous_position.y, get_image_width(), get_image_height(), GL_BLACK);
            gl_swap_buffer(); // swap back to current fb
        }

        gl_swap_buffer();

        // store previous position so we can remove it 
        previous_position.x = bird_position.x;
        previous_position.y = bird_position.y;

        bird_position.x += 100; // increment to next x-value
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
 * This function draws the image of a staff member given their first initial
 * at the given (x,y) coordinates.
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
void gl_draw_target(unsigned int leftBound, unsigned int size)
{
    // generate x-coordinate
    unsigned int x = random_getNumber(SCREEN_WIDTH - size, leftBound); 
    // generate y-coordinate
    unsigned int y = random_getNumber(GROUND_Y - size, 0);
    // plot target
    gl_draw_rect(x, y, size, size, GL_WHITE);
    // store target position and size
    target_position.x = x;
    target_position.y = y;
    TARGET_SIZE = size;
}


void angry_nerds_game_init(void) {
    angry_nerds_graphics_init();
    gl_init(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DOUBLEBUFFER);

    // Set background color
    gl_clear(GL_BLACK);
    gl_draw_string(50, SCREEN_HEIGHT/2, "Welcome to Angry Nerds!", GL_WHITE);
    gl_draw_string(50, SCREEN_HEIGHT/2 + 2*gl_get_char_height(), "Choose difficulty level by pressing a key on the RPi keyboard:", GL_WHITE);

    gl_draw_string(50, SCREEN_HEIGHT/2 + 3*gl_get_char_height(), "(1) Easy", GL_GREEN);
    gl_draw_string(350, SCREEN_HEIGHT/2 + 3*gl_get_char_height(), "(2) Medium", GL_BLUE);
    gl_draw_string(650, SCREEN_HEIGHT/2 + 3*gl_get_char_height(), "(3) Hard", GL_RED);
    gl_draw_string(950, SCREEN_HEIGHT/2 + 3*gl_get_char_height(), "(4) EXPERT", GL_YELLOW);

    gl_swap_buffer();

    // Read keyboard scancodes to select game difficulty
    while(1) {
        unsigned char key = keyboard_read_next();
        printf("\nRead: %c\n", key);

        switch(key) {
            case '1' :
                printf("Difficulty selected: Easy");
                angry_nerds_game_start(1);
                break;
            case '2' :
                printf("Difficulty selected: Medium");
                angry_nerds_game_start(2);
                break;
            case '3' :
                printf("Difficulty selected: Hard");
                angry_nerds_game_start(3);
                break;
            case '4' :
                printf("Difficulty selected: EXPERT");
                angry_nerds_game_start(4);
                break;
            default :
                printf("Select a difficulty by pressing 1, 2, 3, or 4!");
        }

    }

}

/*
 * Display countdown on-screen from time to 0.
 * This function is only defined for times between 0 and 9. 
 */
void display_countdown(unsigned char time) {
    for(int i = time; i >= 0; i--) {
        char time_char = i + '0';
        gl_draw_char(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, time_char, GL_RED); // display the time
        if(i == 0) {
            gl_draw_string(SCREEN_WIDTH/2 - 2 * gl_get_char_width(), SCREEN_HEIGHT/2 + gl_get_char_height(), "Fire!", GL_GREEN); // display "Fire!" prompt for 1 second
        } 
        timer_delay_ms(1000); 
        gl_swap_buffer();

        gl_draw_rect(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, gl_get_char_width(), gl_get_char_height(), GL_BLACK); // clear char

    }
}

/* Helper function to display game instructions at top of screen on both framebuffers */
void angry_nerds_game_display_instructions(void) {
    gl_draw_string(0, 0, "Welcome to the Angry Nerds game!", GL_WHITE);
    gl_draw_string(0, gl_get_char_height(), "To shoot, pull back the slingshot and release when the countdown timer displayed on screen is up.", GL_WHITE);
    gl_draw_string(0, 2*gl_get_char_height(), "Your objective is to knock out as many targets as possible in 10 tries!", GL_WHITE);
    gl_swap_buffer();
    gl_draw_string(0, 0, "Welcome to the Angry Nerds game!", GL_WHITE);
    gl_draw_string(0, gl_get_char_height(), "To shoot, pull back the slingshot and release when the countdown timer displayed on screen is up.", GL_WHITE);
    gl_draw_string(0, 2*gl_get_char_height(), "Your objective is to knock out as many targets as possible in 10 tries!", GL_WHITE);
    gl_swap_buffer();
}

// TODO: Replace with real read_angle function @Selena
double TEMP_READ_ANGLE(void) {
    // return some value between 0 and pi/2
    return deg_to_rad(60);
}

// TODO: Replace with real read_force function @James
double TEMP_READ_FORCE(void) {
    // return some value between 0 and 1
    return 0.75;
}

/* 
 * angry_nerds_game_start() is called after the game is initialized and the 
 * difficulty is selected, with param @difficulty between 1-4 (1 = easiest, 4 =
 * hardest).
 * 
 * 
 */
void angry_nerds_game_start(unsigned int difficulty) {
    // clear screen
    gl_swap_buffer();
    gl_clear(GL_BLACK);
    gl_swap_buffer();
    angry_nerds_game_display_instructions(); // show basic text-based game tutorial

    display_countdown(5); // display the countdown

    // while game is running, start a new round of the game:
    for(int i = 0; i < 10; i++) {
        // clear screen
        gl_clear(GL_BLACK);
        gl_swap_buffer();
        gl_clear(GL_BLACK);
        gl_swap_buffer();

        // for each round of the game, start 5 second countdown on screen
        display_countdown(5); // display the countdown

        // at end of 5 second countdown, call read_accel and read_force
        double force = TEMP_READ_FORCE();
        double angle = TEMP_READ_ANGLE();

        // plot ground on both framebuffers
        gl_plot_ground(GROUND_Y);

        // plot trajectory after reading force and angle 
        gl_plot_image_trajectory(force, angle, 'j');

        // wait for "x" key to exit

    }



}


void main (void)
{
    uart_init();
    random_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    angry_nerds_game_init();

    // angry_nerds_graphics_init();
    // gl_init(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DOUBLEBUFFER);

    // // Set background color
    // gl_clear(GL_BLACK);

    // // plot ground on both framebuffers
    // gl_plot_ground(GROUND_Y);

    // // throw julie at 60 degrees
    // gl_plot_image_trajectory(1.0, deg_to_rad(60), 'j');

    // // plot trajectory of bird given angle and force
    // gl_plot_trajectory(1.0, deg_to_rad(60), GL_AMBER);
    // gl_swap_buffer(); // plot on both framebuffers
    // gl_plot_trajectory(1.0, deg_to_rad(60), GL_AMBER);
    // gl_swap_buffer();

    // // plot initial velocity vector given angle and force
    // gl_plot_initial_velocity_vector(1.0, deg_to_rad(60), GL_BLACK);
    // gl_swap_buffer(); // plot on both framebuffers
    // gl_plot_initial_velocity_vector(1.0, deg_to_rad(60), GL_BLACK);
    // gl_swap_buffer();


    // gl_draw_target(SCREEN_WIDTH / 2, 100);

    // // Show buffer with drawn contents
    // gl_swap_buffer();

    uart_putchar(EOT);
}
