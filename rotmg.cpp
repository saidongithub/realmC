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

asset* players;

int scaling = 100;

const int char_size = 32;
short charClass;

const int window_width = 16 * scaling;
const int window_height = 9 * scaling;

const int camera_width = 12 * scaling;
const int hud_width = 4 * scaling - 1;

const int player_x = (camera_width - char_size)/ 2;
const int player_y = (window_height - char_size)/ 2;

void draw_character(short direction, int shooting, int moving){
	int ix = 0;
	int iy = charClass * 8;
	int iwidth = 8;
	int width = char_size;
	int x = player_x;
	if(!shooting){
		if(moving){
			ix = moving * 8;
			if(!(direction % 3)){
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
	iy += direction * 8;
	if(direction == 3){
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

	//assets
	players = asset_load("players.png");

	//if assets fail, return error
	if(!(players)){
		printf("Failed to load asset\n");
		return -1;
	}


	//mass declaration of variables
	int mouse_x, mouse_y;

	short selected_x = 0;
	short selected_y = 0;

	bool mouse_clicked = false;
	short click_x = 0;
	short click_y = 0;
	short release_x = 0;
	short release_y = 0;

	short shooting = 0;
	short shootick = 0;

	short moving = 0;
	short movetick = 0;

	short direction = 0;
	short directionX = 0;
	short directionY = 0;

	charClass = 3 * 0;

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
			if(directionX > 0){
				direction = 0;
			}
			if(directionX < 0){
				direction = 3;
			}
			if(directionY > 0){
				direction = 2;
			}
			if(directionY < 0){
				direction = 1;
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
					click_x = (short)mouse_x;
					click_y = (short)mouse_y;
				}
				//if leftmouse released
				else {
					release_x = (short)mouse_x;
					release_y = (short)mouse_y;
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
					direction = 0;
					if((camera_width - mouse_x) * 3 >= mouse_y * 4)
						direction = 2;
					//if mouse is bottom-left of window
					if(mouse_x * 3 <= mouse_y * 4)
						direction++;
				}
			} else{
				shooting = 0;
			}
		}
		//draws hud
		int hud_x = window_width - hud_width;
		doge_setcolor(0.5, 0.5, 0.5);
		doge_fill_rectangle(hud_x, 0, hud_width, window_height);
		doge_setcolor(1, 1, 1);
		/*draws character
		if(shooting){
			if(direction != 3){
				doge_draw_image_section(players -> image, (shooting + 3) * 8, (direction + charClass) * 8, shooting * 8, 8, player_x, player_y, char_size * shooting, char_size);
			} else{
				doge_draw_image_section(players -> image, (shooting + 3) * 8, charClass * 8, shooting * 8, 8, player_x + char_size, player_y, -char_size * shooting, char_size);
			}
		} else
		if(moving){
			if(direction != 3){
				doge_draw_image_section(players -> image, 8 * moving, (direction + charClass) * 8, 8, 8, player_x, player_y, char_size, char_size);
			} else{
				doge_draw_image_section(players -> image, 0, 0, 8, 8, player_x, player_y, char_size, char_size);
			}
		} else
		if(direction != 3){
			doge_draw_image_section(players -> image, 0, (direction + charClass) * 8, 8, 8, player_x, player_y, char_size, char_size);
		} else{
			doge_draw_image_section(players -> image, 0, (direction + charClass) * 8, 8, 8, player_x, player_y, char_size, char_size);
		}
		*/
		draw_character(direction, shooting, moving);

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