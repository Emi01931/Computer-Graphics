#include "display.h"
#include "vector.h"


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;
uint32_t* color_buffer = NULL; // Some books like to call this as "frame buffer." For all practical purposes, color buffer & frame buffer are the same thing; they are a mirror in memory of the pixels that we want to see in our display.
int window_width = 0;
int window_height = 0;

bool initialize_window(void){
    if(SDL_Init(SDL_INIT_EVERYTHING) !=0){
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Use SDL to query what is the fullscreen maximum width and height
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(
            0, // the device
            &displayMode
    );
    window_width = displayMode.w; //800;
    window_height = displayMode.h; //600;

    //Create a SDL window
    window = SDL_CreateWindow(
            NULL, // title
            SDL_WINDOWPOS_CENTERED, //pos x
            SDL_WINDOWPOS_CENTERED, //pos y
            window_width, // width
            window_height, // height
            SDL_WINDOW_BORDERLESS
    );

    if(!window){
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    //Create a SDL renderer
    renderer = SDL_CreateRenderer(
            window,
            -1,
            0
    );
    if(!renderer){
        fprintf(stderr, "Error creating SDL renderer.\n");
        return false;
    }
    // Change the video mode to be real fullscreen, no "fake full screen"
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    // Creating a SDL texture that is used to display the color buffer
    color_buffer_texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            window_width,
            window_height
    );

    return true;
}

void draw_grid(void) {
    for (int y = 0; y < window_height; y++) {
        for (int x = 0; x < window_width; x++) {
            if (x % 10 == 0 || y % 10 == 0) {
                color_buffer[(window_width * y) + x] = 0xFF444444;
            }
        }
    }
}

/*void draw_grid(void) {
    for (int y = 0; y < window_height; y += 10) {
        for (int x = 0; x < window_width; x += 10) {
            color_buffer[(window_width * y) + x] = 0xFF444444;
        }
    }
}*/

void draw_pixel(int x, int y, uint32_t color){
    if(x>=0 && x<window_width && y>=0 && y<window_height){
        color_buffer[(window_width * y) + x] = color;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
            // color_buffer[(window_width * current_y) + current_x] = color;
        }
    }
}

void render_color_buffer(void){
    // Copy the frame buffer to the texture
    SDL_UpdateTexture(
            color_buffer_texture,
            NULL, // No rectangle area to update. NULL indicates that update the entire texture
            color_buffer, // the source, raw pixel data
            (int)(window_width * sizeof(uint32_t)) // The number of bytes in a row of pixel data
    );

    // Copy the texture (or portion) to the current rendering target
    SDL_RenderCopy(renderer,
                   color_buffer_texture,
                   NULL, // render the entire texture, no specific rectangle
                   NULL // the entire rendering target, no specific rectangle (section)
    );
}

void clear_color_buffer(uint32_t color){
    for(int i = 0; i < window_width * window_height; i++){
        color_buffer[i] = color;
    }
}

void destroy_window(void){
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_line(int x0, int y0, int x1, int y1, color_t color){  
    int dx, dy, p, x, y;
	dx=x1-x0;   dy=y1-y0;   x=x0;   y=y0;   p=2*dy-dx;

    if(x<x1){   //for lines which goes from left to right
        while(x<x1){
            if(p>=0)
            {
                draw_pixel(x,y,color);
                y=y+1;
                p=p+2*dy-2*dx;
            }
            else
            {
                draw_pixel(x,y,color);
                p=p+2*dy;
            }
            x=x+1;
        }
    }

    if(x>x1){   //for lines which goes from right to left 
        while(x>x1){
            if(p>=0)
            {
                draw_pixel(x,y,color);
                y=y+1;
                p=p+2*dy-2*dx;
            }
            else
            {
                draw_pixel(x,y,color);
                p=p+2*dy;
            }
            x=x-1;
        }
    }

    if(x=x1){   //for vertical lines
        if (y<y1){
            while (y<y1){
                draw_pixel(x,y,color);
                y++;
            }
        }
        
        if (y>y1){
            while (y>y1){
                draw_pixel(x,y,color);
                y--;
            }
        }
    }

}  