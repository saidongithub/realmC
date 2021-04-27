#include <doge/doge.h>
#include <doge/window.h>
#include <doge/graphics.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <random>
#include "asset.h"
#include "nanotime.h"

enum direction_e{
	RIGHT,
	DOWN,
	UP,
	LEFT
};

typedef enum direction_e direction;

direction facing;

asset* players;

const int scaling = 100;
const int char_size = 64;
int charClass;

const int aspectx = 16;
const int aspecty = 9;
const int window_width = (aspectx * scaling);
const int window_height = (aspecty * scaling);

const int camera_width = 12 * scaling;
const int hud_width = 4 * scaling - 1;

const int player_x = (camera_width - char_size)/ 2;
const int player_y = (window_height - char_size)/ 2;

void draw_(){

}

void draw_character(int shooting, int moving){
	int ix = 0;
	int iy = charClass * 8;
	int iwidth = 8;
	int width = char_size;
	int x = player_x;
	if(!shooting){
		if(moving){
			ix = moving * 8;
			if(!(facing % 3)){
				ix -= 8;
			}
		} else{
			ix = 0;
		}
	} else{
		ix = (shooting + 3) * 8;
		iwidth = shooting * 8;
		width = shooting * char_size;
	}
	iy += facing * 8;
	if(facing == LEFT){
		width = width * -1;
		x += char_size;
		iy -= 3 * 8;
	}
	doge_draw_image_section(players -> image, ix, iy, iwidth, 8, x, player_y, width, char_size);
}

int main(){
    int error;

    error = glfwInit();

    if(!error){
        printf("Failed to initialize glfw\n");

        return -1;
    }

    doge_window_t* window;

    window = doge_window_create("Realm of the Mad God C", window_width, window_height);

    if(!window){
        printf("Failed to create window\n");

        return -1;
    }

    doge_window_makecurrentcontext(window);

    error = glewInit();

    if(error){
        printf("Failed to initialize glew\n");
        doge_window_free(window);

        return -1;
    }

	players = asset_load("players.png");

	if(!(players)){
		printf("Failed to load asset\n");
		return -1;
	}


	//mass declaration of variables
	int mouse_x, mouse_y;

	int selected_x = 0;
	int selected_y = 0;

	bool mouse_clicked = false;
	int click_x = 0;
	int click_y = 0;
	int release_x = 0;
	int release_y = 0;

	int shooting = 0;
	int shootick = 0;

	int moving = 0;
	int movetick = 0;

	int directionX = 0;
	int directionY = 0;

	charClass = 0;

	const int tps = 30;
    unsigned long lasttime, now, passed, totalTimepassedTicks;
    totalTimepassedTicks = 0;
    lasttime = nanotime();
	int tick = 0;

    while(!doge_window_shouldclose(window)){
        /* clear the window */
        doge_clear();

		/* update tick */
		now = nanotime();
		passed = now - lasttime;
		lasttime = now;
		totalTimepassedTicks += passed * tps;

		if(totalTimepassedTicks > (unsigned long)(1e9)){
			totalTimepassedTicks -= (unsigned long)(1e9);
			tick++;
		}

		/* check keyboard input */
		directionX = 0;
		directionY = 0;
		if(doge_window_keypressed(window, DOGE_KEY_W)){
			directionY++;
		}
		if(doge_window_keypressed(window, DOGE_KEY_A)){
			directionX--;
		}
		if(doge_window_keypressed(window, DOGE_KEY_S)){
			directionY--;
		}
		if(doge_window_keypressed(window, DOGE_KEY_D)){
			directionX++;
		}
		if(directionX || directionY){
			if(movetick < 15){
				moving = 2;
			} else
			if(movetick <= 30){
				moving = 1;
			} else{
				movetick = -1;
			}
			movetick++;
			if(directionX && directionY){
			/* diagonal movement */
				if(directionX > 0){
					if(directionY > 0){
						facing = UP;
					} else{
						facing = RIGHT;
					}
				} else
				if(directionY > 0){
					facing = LEFT;
				} else{
					facing = DOWN;
				}
			} else{
			/* vertical/horizontal movement */
				if(directionX > 0){
					facing = RIGHT;
				} else
				if(directionY < 0){
					facing = DOWN;
				} else
				if(directionY > 0){
					facing = UP;
				} else
				if(directionX < 0){
					facing = LEFT;
				}
			}
		} else{
			moving = 0;
			movetick = 0;
		}

		/* check mouse input */
		doge_window_getcursorpos(window, &mouse_x, &mouse_y);
		//check if mouse is within window
		if(mouse_x >= 0 && mouse_y >= 0 && mouse_x <= window_width && mouse_y <= window_height){
			//check if leftmouse changed states
			if(mouse_clicked != doge_window_mousepressed(window, DOGE_MOUSE_BUTTON_LEFT)){
				//if leftmouse pushed
				if(!mouse_clicked){
					click_x = mouse_x;
					click_y = mouse_y;
				}
				//if leftmouse released
				else {
					release_x = (int)mouse_x;
					release_y = (int)mouse_y;
				}
				mouse_clicked = doge_window_mousepressed(window, DOGE_MOUSE_BUTTON_LEFT);
			}
			//if leftmouse down
			if(doge_window_mousepressed(window, DOGE_MOUSE_BUTTON_LEFT)){
				if(click_x < camera_width){
					if(shootick < 15){
						shooting = 1;
					} else
					if(shootick <= 30){
						shooting = 2;
					} else{
						shootick = -1;
					}
					shootick++;

					/*
					0 = right
					1 = down
					2 = up
					3 = left */
					//if mouse is top-left of window
					facing = RIGHT;
					if((camera_width - mouse_x) * 3 >= mouse_y * 4)
						facing = UP;
					//if mouse is bottom-left of window
					if(mouse_x * 3 <= mouse_y * 4)
						facing = (direction)((int)facing + 1);
				}
			} else{
				shooting = 0;
			}
		}
		/* draw hud */
		if(charClass == -1){
			for(int y = 0; y < 4; y++){
				for(int x = 0; x < 5; x++){
					if(mouse_x)
					doge_setcolor(0.5, 0.5, 0.5);
					doge_fill_rectangle(aspectx * (scaling / 5) * x + (aspectx * scaling / 20), aspecty * (scaling / 4) * y, (aspecty - 1) * 20, (aspecty - 1) * 20);
					doge_setcolor(1, 1, 1);
					doge_draw_image_section(players -> image, 0, 3 * 8 * (y * 5 + x), 8, 8, 16 * 20 * x + 16 * 5 + char_size / 2, aspecty * (scaling / 4) * y + 16, char_size * 4, char_size * 4);
				}
			}
		} else{
			int hud_x = window_width - hud_width;
			doge_fill_rectangle(hud_x, 0, hud_width, window_height);
			doge_setcolor(1, 1, 1);

			/* draw character */
			draw_character(shooting, moving);
		}

        /* swap the frame buffer */
        doge_window_render(window);

        /* check for keyboard, mouse, or close event */
        doge_window_poll();
    }

	//free all assets
	free(players);

	//free doge_window
	doge_window_free(window);
    return 0;
}