#include <doge/doge.h>
#include <doge/window.h>
#include <doge/graphics.h>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <random>

typedef struct asset_s{
	doge_image_t* image;
	int width;
	int height;
} asset;

asset* asset_load(const char* filename){
	//load image
	doge_image_t* image;
	image = doge_image_load(filename);
	if(!image){
		printf("Failed loading img\n");
		return nullptr;
	}

	//get memory for new asset
	asset* newasset;
	newasset = (asset*)malloc(sizeof(asset));
	if(!newasset){
		doge_image_free(image);
		printf("Failed to malloc\n");
		return nullptr;
	}
	//make new asset
	newasset -> image = image;
	newasset -> width = doge_image_width(image);
	newasset -> height = doge_image_height(image);

	return newasset;
}

void asset_free(asset* asset){
	doge_image_free(asset -> image);

	free(asset);
}

int main(){
    int error;

    error = glfwInit();

    if(!error){
        printf("Failed to initialize glfw\n");

        return -1;
    }

    doge_window_t* window;

	int window_width = 1440;
	int window_height = 810;
	int camera_width = window_height;
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
	asset* players = asset_load("players.png");

	//if assets fail, return error
	if(!(players)){
		printf("Failed to load asset\n");
		return -1;
	}


	//mass declaration of variables
	int mouse_x, mouse_y;

	int selected_x = 0;
	int selected_y = 0;

	int mouse_clicked = 0;
	int click_x = 0;
	int click_y = 0;
	int release_x = 0;
	int release_y = 0;

    while(!doge_window_shouldclose(window)){
        /* clear the window */
        doge_clear();

		//set mouse_x and mouse_y to mouse's x and y position relative to window's 0,0
		doge_window_getcursorpos(window, &mouse_x, &mouse_y);

		//check if mouse is within window
		if(mouse_x >= 0 && mouse_y >= 0 && mouse_x <= window_width && mouse_y <= window_height){
			//click update
			if(mouse_clicked != doge_window_mousepressed(window, DOGE_MOUSE_BUTTON_LEFT)){
				//if the mouse went from normal to pushed (was pushed)
				if(!mouse_clicked){
					click_x = mouse_x;
					click_y = mouse_y;
				}
				//if the mouse went from pushed to normal (was released)
				else {
					release_x = mouse_x;
					release_y = mouse_y;
				}

				mouse_clicked = doge_window_mousepressed(window, DOGE_MOUSE_BUTTON_LEFT);
			}
		}
		//draws
		for(int i = 0; i <= 16; i++){
			doge_draw_image_section(players -> image, 0, 24 * i, 8, 8, i * 64, 0, 64, 64);
		}

		//if mouse held down selected

        /* swap the frame buffer */
        doge_window_render(window);

        /* check for keyboard, mouse, or close event */
        doge_window_poll();
    }
	//loop through board to free all pieces

	//free all assets
	//free(assets);

	//free doge_window
	doge_window_free(window);
    return 0;
}