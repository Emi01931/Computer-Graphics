#ifndef INC_3DRENDERER_DISPLAY_H
#define INC_3DRENDERER_DISPLAY_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS) // 1000 miliseconds (1 seg) between the FPS = time take every frame

// Declare a new type definition to hold 32-bit colors value
typedef uint32_t color_t;

extern SDL_Window* window ;
extern SDL_Renderer* renderer;
extern SDL_Texture* color_buffer_texture;
extern uint32_t* color_buffer; // Some books like to call this as "frame buffer." For all practical purposes, color buffer & frame buffer are the same thing; they are a mirror in memory of the pixels that we want to see in our display.
extern int window_width;
extern int window_height;

bool initialize_window(void);                                                               //
void draw_grid(void);                                                                       //
void draw_pixel(int x, int y, color_t color);                                               //
void draw_rect(int x, int y, int width, int height, uint32_t color);                        //
void draw_line(int x0, int y0, int x1, int y1, color_t color);                              //
void draw_flat_bottom(int x0, int y0, int x1, int y1, int mx, int my, color_t color);       //
void draw_flat_top(int x0, int y0,  int mx, int my, int x2, int y2, color_t color);         //
void render_color_buffer(void);                                                             //
void clear_color_buffer(color_t color);                                                     //
void destroy_window(void);                                                                  //
color_t light_apply_intensity (color_t original_color, float percentage_factor);            //

#endif //INC_3DRENDERER_DISPLAY_H
