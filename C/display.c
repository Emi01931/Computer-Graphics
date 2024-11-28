#include "display.h"
#include "vector.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Texture *color_buffer_texture = NULL;
uint32_t *color_buffer = NULL; // Some books like to call this as "frame buffer." For all practical purposes, color buffer & frame buffer are the same thing; they are a mirror in memory of the pixels that we want to see in our display.
int window_width = 0;
int window_height = 0;
int ceroDiv = 999999999;

bool initialize_window(void){
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    // Use SDL to query what is the fullscreen maximum width and height
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(
        0, // the device
        &displayMode);
    window_width = displayMode.w;  // 800;
    window_height = displayMode.h; // 600;

    // Create a SDL window
    window = SDL_CreateWindow(
        NULL,                   // title
        SDL_WINDOWPOS_CENTERED, // pos x
        SDL_WINDOWPOS_CENTERED, // pos y
        window_width,           // width
        window_height,          // height
        SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        fprintf(stderr, "Error creating SDL window.\n");
        return false;
    }

    // Create a SDL renderer
    renderer = SDL_CreateRenderer(
        window,
        -1,
        0);
    if (!renderer)
    {
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
        window_height);

    return true;
}

void draw_grid(void){
    for (int y = 0; y < window_height; y++)
    {
        for (int x = 0; x < window_width; x++)
        {
            if (x % 10 == 0 || y % 10 == 0)
            {
                color_buffer[(window_width * y) + x] = 0xFF444444;
            }
        }
    }
}
void draw_pixel(int x, int y, uint32_t color){
    if (x >= 0 && x < window_width && y >= 0 && y < window_height)
    {
        color_buffer[(window_width * y) + x] = color;
    }
}
void draw_rect(int x, int y, int width, int height, uint32_t color){
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
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
        NULL,                                  // No rectangle area to update. NULL indicates that update the entire texture
        color_buffer,                          // the source, raw pixel data
        (int)(window_width * sizeof(uint32_t)) // The number of bytes in a row of pixel data
    );

    // Copy the texture (or portion) to the current rendering target
    SDL_RenderCopy(renderer,
                   color_buffer_texture,
                   NULL, // render the entire texture, no specific rectangle
                   NULL  // the entire rendering target, no specific rectangle (section)
    );
}
void clear_color_buffer(uint32_t color){
    for (int i = 0; i < window_width * window_height; i++)
    {
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
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int xinc = (x0 < x1) ? 1:-1;
    int yinc = (y0 < y1) ? 1:-1;
    int err = dx - dy;

    while (true){
        draw_pixel(x0, y0, color);
        if(x0 == x1 && y0 == y1)
            break;
        int e2 = 2*err;
        if(e2>-dy){
            err -= dy;
            x0 += xinc;
        }
        if (e2<dx){
            err += dx;
            y0 += yinc;
        }
        
    }
    
}
void draw_flat_bottom(int x0, int y0, int x1, int y1, int x2, int y2, color_t color){
    float xStart = x0;
    float xEnd = x0;
    //printf("\tB");
    float m2;
    float m1;
    if(y1 != y0){
        m1 = (float)(x1-x0)/(y1-y0);
    }else{
        m1 = -(float)ceroDiv;
    }

    if(y2 != y0){
        m2 = (float)(x2-x0)/(y2-y0);
    }else{
        m2 = (float)ceroDiv;
    }

    for(int y = y0; y < y2; y++){
        draw_line(xStart,y,xEnd,y, color);
        xStart += m1;
        xEnd += m2;
    }
}
void draw_flat_top(int x0, int y0, int x1, int y1, int x2, int y2, color_t color){
    float xStart = x2;
    float xEnd = x2;
    //printf("\tT");
    float m2;
    float m1;
    if(y2 != y0){
        m1 = (float)(x2-x0)/(y2-y0);
    }else{
        m1 = (float)ceroDiv;
    }

    if(y2 != y1){
        m2 = (float)(x2-x1)/(y2-y1);
    }else{
        m2 = -(float)ceroDiv;
    }
    
    for(int y = y2; y >= y0; y--){
        draw_line(xStart,y,xEnd,y, color);
        xStart -= m1;
        xEnd -= m2;
    }
}

color_t light_apply_intensity (color_t original_color, float percentage_factor) {
    if (percentage_factor < 0) percentage_factor = 0;
    if (percentage_factor > 1) percentage_factor = 1;

    //The original color is divided in the RGB and each one is multiplied by the light intensity factor
    color_t a = (original_color & 0xFF000000); 
    color_t r = (original_color & 0x00FF0000)*percentage_factor;
    color_t g = (original_color & 0x0000FF00)*percentage_factor;
    color_t b = (original_color & 0x000000FF)*percentage_factor;
    color_t new_color = a | (r & 0x00FF0000) | (g & 0x0000FF00) | (b & 0x000000FF);
    return new_color;
}