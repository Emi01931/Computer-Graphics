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

void draw_line(int x0, int y0, int x1, int y1, color_t color)  
{  
    float dx, dy, p, x, y;  
    dx=x1-x0;  
    dy=y1-y0;  
    x=x0;  
    y=y0;  
    p=2*dy-dx;  

    while(x<=x1)  
    { 
        draw_rect(x + window_width/2, y + window_height/2, 4, 4, color);    
        x++;
        if(p<0){
            p=p+2*dy;
        }
        else{
            p = p+2*dy-2*dx;
            y++;
        }
    }  
}  

void draw_circle(int x, int y, int xc, int yc, color_t color){
    draw_pixel(xc+x, yc+y, color);      //1
    draw_pixel(xc-x, yc+y, color);
    draw_pixel(xc-y, yc+x, color);     //3
    draw_pixel(xc-y, yc-x, color);
    draw_pixel(xc-x, yc-y, color);     //5
    draw_pixel(xc+x, yc-y, color);
    draw_pixel(xc+y, yc-x, color);
    draw_pixel(xc+y, yc+x, color);     //8
}

void draw_ellipse(int rx, int ry, int xc, int yc, color_t color){
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = ry;
 
    // Decision parameter for region 1, since the ellipse starts at (0,ry) the desicion parameter is as follows
    d1 = (ry * ry) - (rx * rx * ry) + (0.25 * rx * rx); //funtion of an ellipse
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    while (dx < dy) {   //if dy is minor than dx then that means that the slope is <1, this is a sign that region 2 started
 
        // first quadrant point
        draw_pixel(xc + x, yc + y, color);
        //mirror points of the first quadrant to draw half of the ellipse
        draw_pixel(xc - x, yc + y, color);
        draw_pixel(xc + x, yc - y, color);
        draw_pixel(xc - x, yc - y, color);

        //to draw we have two options (x_k + 1, y_k) or (x_k + 1, y_k -1)

        if (d1 < 0){    //inside ellipse
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + (ry * ry) + dx;
        }
        else{
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }
 
    // Decision parameter for region 2
    d2 = ((ry * ry) * ((x + 0.5) * (x + 0.5)))
         + ((rx * rx) * ((y - 1) * (y - 1)))
         - (rx * rx * ry * ry);
 
    // Plotting points of region 2
    while (y >= 0) {
 
        draw_pixel(xc + x, yc + y, color);
        draw_pixel(xc - x, yc + y, color);
        draw_pixel(xc + x, yc - y, color);
        draw_pixel(xc - x, yc - y, color);
 
        // (x_k, y_k -1) or (x_k + 1, y_k -1)
        if (d2 > 0) { //outside ellipse
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        }
        else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
}