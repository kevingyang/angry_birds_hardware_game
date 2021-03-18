#include "assert.h"
#include "printf.h"
#include "gl.h"
#include "timer.h"
#include "uart.h"
#include "trig.h"
#include "image.h"
#include "keyboard.h"
#include "randomHardware.h" // for random function
#include "project-app.h"

#define LINE_HEIGHT (gl_get_char_height() + 5) // line spacing of 5 px

// TODO: Replace with real read_angle function @Selena
double TEMP_READ_ANGLE(void) {
    // return some value between 0 and pi/2
    return deg_to_rad(random_getNumber(89, 1));
}

// TODO: Replace with real read_force function @James
double TEMP_READ_FORCE(void) {
    // return some value between 0.9 and 1
    unsigned int precision = random_getNumber(1000, 900);
    return precision/1000.0;
}

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

/* 
 * Using kinematics, this function plots the trajectory of a single point
 * in some color given the initial force and angle that it starts at.
 */
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

/*
 * This function plots the trajectory of an image on-screen given a force and
 * angle at which it's launched, with the image customizable with the first 
 * initial of the instructor.
 * 
 *  
 */
unsigned int gl_plot_image_trajectory(double force, double angle, char first_initial) {
    unsigned int target_hit = 0; // if target hasn't been hit, return 0.
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
        // must move y-position up by image's height
        bird_position.y = GROUND_Y - get_image_height() - virtual_y*y_scale_factor;

        /* Change the length of timer delay here to affect how fast the projectile moves across the screen. */
        timer_delay_ms(150); // delay 1/4 second before removing and going to next iteration of loop

        // after the first iteration, we must remove the projectile at the previous position
        if(bird_position.x != 50) {
            gl_draw_rect(previous_position.x, previous_position.y, get_image_width(), get_image_height(), GL_BLACK);
            gl_swap_buffer(); 
            gl_draw_rect(previous_position.x, previous_position.y, get_image_width(), get_image_height(), GL_BLACK);
            gl_swap_buffer(); // swap back to current fb
        }

        gl_draw_image(bird_position.x, bird_position.y, first_initial); // draw next

        gl_swap_buffer(); // show what's been drawn

        // check if the image has hit the target
        if(target_position.x < bird_position.x + get_image_width() &&
           target_position.x + TARGET_SIZE > bird_position.x &&
           target_position.y < bird_position.y + get_image_height() &&
           target_position.y + TARGET_SIZE > bird_position.y) {
               target_hit = 1; // turn on target hit flag to return
        }

        // store previous position so we can remove it 
        previous_position.x = bird_position.x;
        previous_position.y = bird_position.y;

        bird_position.x += 20; // increment to next x-value
    }
    return target_hit;
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
 * with its left corner at the given (x,y) coordinates.
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
    gl_swap_buffer();
    gl_draw_rect(x, y, size, size, GL_WHITE);
    gl_swap_buffer();

    // store target position and size
    target_position.x = x;
    target_position.y = y;
    TARGET_SIZE = size;
}

/* Helper function that randomly selects one of the TAs as the bird to be launched! */
char select_random_fighter() {
    unsigned int randomNumber = random_getNumber(6, 1);
    switch(randomNumber) {
        case 1:
            return 'j'; // julie
        case 2:
            return 'p'; // pat
        case 3:
            return 's'; // sean
        case 4:
            return 'e'; // pEter
        case 5:
            return 'a'; // anna
        case 6:
            return 'l'; // liana
        default:
            return '0'; // ??
    }
}

/* 
 * This function is called to begin the game, and initalizes the graphics 
 * necessary.
 * It also sets up the keyboard interface to allow the user to select
 * game difficulty, leading to the start of a new game.
 */
void angry_nerds_game_init(void) {
    random_init();
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);
    angry_nerds_graphics_init(); // must be initialized before graphics init (sets SCREEN_WIDTH and SCREEN_HEIGHT)
    gl_init(SCREEN_WIDTH, SCREEN_HEIGHT, GL_DOUBLEBUFFER);

    // Set background color
    gl_clear(GL_BLACK);
    gl_draw_string(50, SCREEN_HEIGHT/2, "Welcome to Angry Nerds!", GL_WHITE);
    gl_draw_string(50, SCREEN_HEIGHT/2 + 2*LINE_HEIGHT, "Choose difficulty level by pressing a key on the RPi keyboard:", GL_WHITE);

    gl_draw_string(50, SCREEN_HEIGHT/2 + 3*LINE_HEIGHT, "(1) Easy", GL_GREEN);
    gl_draw_string(350, SCREEN_HEIGHT/2 + 3*LINE_HEIGHT, "(2) Medium", GL_BLUE);
    gl_draw_string(650, SCREEN_HEIGHT/2 + 3*LINE_HEIGHT, "(3) Hard", GL_RED);
    gl_draw_string(950, SCREEN_HEIGHT/2 + 3*LINE_HEIGHT, "(4) EXPERT", GL_YELLOW);

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
        gl_draw_rect(SCREEN_WIDTH/2  - 2 * gl_get_char_width(), SCREEN_HEIGHT/2, gl_get_char_width() * 5, LINE_HEIGHT * 2, GL_BLACK); // clear previous screen

        char time_char = i + '0';
        if(i == 0) {
            gl_swap_buffer();
            gl_draw_string(SCREEN_WIDTH/2 - 2 * gl_get_char_width(), SCREEN_HEIGHT/2 + LINE_HEIGHT, "Fire!", GL_GREEN); // display "Fire!" prompt for 1 second
            gl_swap_buffer();
        } 

        gl_draw_char(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, time_char, GL_RED); // display the time
        timer_delay_ms(1000); 
        gl_swap_buffer();

        gl_draw_rect(SCREEN_WIDTH/2  - 2 * gl_get_char_width(), SCREEN_HEIGHT/2, gl_get_char_width() * 5, LINE_HEIGHT * 2, GL_BLACK); // clear previous screen
    }
    gl_swap_buffer();
    gl_draw_rect(SCREEN_WIDTH/2  - 2 * gl_get_char_width(), SCREEN_HEIGHT/2, gl_get_char_width() * 5, LINE_HEIGHT * 2, GL_BLACK); // on final iteration of loop, clear all contents.

}

/* 
 * Helper function to display game instructions at top of screen on both 
 * framebuffers.
 */
void angry_nerds_game_display_instructions(void) {
    gl_draw_string(0, 0, "Welcome to the Angry Nerds game!", GL_WHITE);
    gl_draw_string(0, LINE_HEIGHT, "To shoot, pull back the slingshot and release when the countdown timer displayed on screen is up.", GL_WHITE);
    gl_draw_string(0, 2*LINE_HEIGHT, "Your objective is to knock out as many targets as possible in 10 tries!", GL_WHITE);
    gl_draw_string(SCREEN_WIDTH/2 - 7 * gl_get_char_width(), SCREEN_HEIGHT/2 - LINE_HEIGHT, "Game begins in:", GL_GREEN);
    gl_swap_buffer();
    // same text, on other buffer
    gl_draw_string(0, 0, "Welcome to the Angry Nerds game!", GL_WHITE);
    gl_draw_string(0, LINE_HEIGHT, "To shoot, pull back the slingshot and release when the countdown timer displayed on screen is up.", GL_WHITE);
    gl_draw_string(0, 2*LINE_HEIGHT, "Your objective is to knock out as many targets as possible in 10 tries!", GL_WHITE);
    gl_draw_string(SCREEN_WIDTH/2 - 7 * gl_get_char_width(), SCREEN_HEIGHT/2 - LINE_HEIGHT, "Game begins in:", GL_GREEN);
    gl_swap_buffer();
}

/* 
 * This helper function sets the size of the target given the difficulty level 
 * ranging from 1-4. 
 */
unsigned int set_target_size(unsigned int difficulty) {
    switch (difficulty) {
        case 1 :
            return 250; // these values are arbitrary and can be modified to tailor difficulty.
        case 2 : 
            return 150;
        case 3 : 
            return 100;
        case 4 :
            return 50;
        default :
            printf("Invalid difficulty!");
            return 0;
    }
}

/* This simple helper function displays the number of targets hit so far and number of birds left at the bottom of the screen. It's called for every round of the game. */
void gl_display_num_targets_hit_num_birds_left(int num_targets_hit, int num_attempt) {
    // display number of targets hit so far
    gl_draw_string(0, SCREEN_HEIGHT - LINE_HEIGHT, "Number of targets hit: ", GL_WHITE);
    gl_draw_char(24 * gl_get_char_width(), SCREEN_HEIGHT - LINE_HEIGHT, '0' + num_targets_hit, GL_GREEN);
    gl_swap_buffer(); // plot on both framebuffers
    gl_draw_string(0, SCREEN_HEIGHT - LINE_HEIGHT, "Number of targets hit: ", GL_WHITE);
    gl_draw_char(24 * gl_get_char_width(), SCREEN_HEIGHT - LINE_HEIGHT, '0' + num_targets_hit, GL_GREEN);
    gl_swap_buffer();
    // display number of birds left
    gl_draw_string(SCREEN_WIDTH - 15 * gl_get_char_width(), SCREEN_HEIGHT - LINE_HEIGHT, "Birds left: ", GL_WHITE);
    gl_draw_char(SCREEN_WIDTH - 2 * gl_get_char_width(), SCREEN_HEIGHT - LINE_HEIGHT, '0' + (10 - num_attempt - 1), GL_RED);
    gl_swap_buffer(); // plot on both framebuffers
    gl_draw_string(SCREEN_WIDTH - 15 * gl_get_char_width(), SCREEN_HEIGHT - LINE_HEIGHT, "Birds left: ", GL_WHITE);
    gl_draw_char(SCREEN_WIDTH - 2 * gl_get_char_width(), SCREEN_HEIGHT - LINE_HEIGHT, '0' + (10 - num_attempt - 1), GL_RED);
    gl_swap_buffer();
}

/* 
 * angry_nerds_game_start() is called after the game is initialized and the 
 * difficulty is selected, with param @difficulty between 1-4 (1 = easiest, 4 =
 * hardest).
 * 
 * 
 */
void angry_nerds_game_start(unsigned int difficulty) {
    unsigned int num_targets_hit = 0;
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

        // for each round of the game, start 3 second countdown on screen
        display_countdown(3); // display the countdown

        // at end of 5 second countdown, call read_accel and read_force
        double force = TEMP_READ_FORCE();
        double angle = TEMP_READ_ANGLE();
        
        gl_plot_ground(GROUND_Y); // plot ground on both framebuffers
        gl_draw_target(SCREEN_WIDTH / 2, set_target_size(difficulty)); // plot target
        gl_display_num_targets_hit_num_birds_left(num_targets_hit, i); // display at bottom of screen the number of targets hit and number of birds left

        // plot "bird" trajectory after reading force and angle 
        unsigned int target_hit = gl_plot_image_trajectory(force, angle, select_random_fighter());
        // plot trajectory of bird given angle and force
        gl_plot_trajectory(force, angle, GL_AMBER);
        gl_swap_buffer(); // plot on both framebuffers
        gl_plot_trajectory(force, angle, GL_AMBER);
        gl_swap_buffer();
        // plot initial velocity vector given angle and force
        gl_plot_initial_velocity_vector(force, angle, GL_BLUE);
        gl_swap_buffer(); // plot on both framebuffers
        gl_plot_initial_velocity_vector(force, angle, GL_BLUE);
        gl_swap_buffer();

        if(target_hit) {
            num_targets_hit++;
            gl_draw_string(SCREEN_WIDTH/2 - 15*gl_get_char_width(), SCREEN_HEIGHT/2, "Congrats, you hit the target!", GL_GREEN);
            gl_swap_buffer();
        }

        timer_delay_ms(5000); // delay 5 seconds so user can see the trajectory of what they shot before going to next round

    }
}


void main (void)
{
    uart_init();
    angry_nerds_game_init(); // start the angry nerds game!
    uart_putchar(EOT);
}
