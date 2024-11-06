#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include <math.h> 
#include "array.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"
#include "pf.h"

#define N_POINTS (9*9*9)

vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];


vec3_t cube_rotation = {0,0,0};
vec3_t cube_translation = {0,0,0};
vec3_t cube_scale = {1,1,1};

float fov_factor = 200;
bool is_running = false;
int previous_frame_time = 0;


int r = 150;
int xc = 550;
int yx = 240;

int rx = 100;
int ry = 150;

int typeOfFigure = 4; //cube = 0, circle = 1, .obj = 2, ellipse = 3, polygonfilling = 4 

void setup(void){
    //Cada pixel usa el tipo de dato uin32_t
    color_buffer = (uint32_t*) malloc(sizeof (uint32_t)*window_width*window_height);
    if(!color_buffer){
        fprintf(stderr, "Error allocating memory for frame buffer,\n");
    }
    int point_count = 0;
    if (typeOfFigure == 0)
    {
        for(float x=-1;x<=1;x+=0.25){
            for(float y=-1; y<=1;y+=0.25){
                for(float z=-1;z<=1;z+=0.25){
                    vec3_t new_point = {.x = x,.y = y,.z = z};
                    cube_points[point_count++] = new_point;
                }
            }
        }
    }

    if(typeOfFigure == 2){
        char fileName[] = "cube.obj";
        //load_obj_file_data(fileName);
        load_cube_mesh_data();
        printf("Length Faces: %i", array_length(mesh.faces));
    }
}


void process_input(void){
    SDL_Event event;
    SDL_PollEvent(&event);//ve si paso algo por el teclado o el mouse y lo guarda aqui

    switch (event.type){
        case SDL_QUIT:
            is_running = false;
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE){
                is_running = false;
            }
            break;
    }
}


vec2_t project(vec3_t v3){
/*
    //Ortografica
    vec2_t projected_point = {
            .x = (fov_factor * v3.x),
            .y = (fov_factor * v3.y)
    };

    //perspectiva
    vec2_t projected_point = {
            .x = (fov_factor * v3.x) / v3.z,
            .y = (fov_factor * v3.y) / v3.z
    };
*/

    //not project
    vec2_t projected_point = {
            .x = v3.x,
            .y = v3.y
    };

    return projected_point;
}


void update(void){
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;
    cube_rotation.x = 0;
    cube_rotation.y = 0;
    cube_rotation.z = 0;

    cube_translation.z = 1.0;
    cube_translation.z = 0;

    mat4_t scale_matrix = mat4_make_scale(cube_scale.x, cube_scale.y, cube_scale.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(cube_rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(cube_rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(cube_rotation.z);
    mat4_t translation_matrix = mat4_make_translation(cube_translation.x, cube_translation.y, cube_translation.z);

    mat4_t world_matrix = mat4_identity();

    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

    vec4_t transformed_points[N_POINTS];

    for(int i = 0; i< N_POINTS; i++){
        vec4_t transformed_point = vec4_from_vec3(cube_points[i]);

        transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
        transformed_points[i] = transformed_point;

        vec2_t projected_point = project(vec3_from_vec4(transformed_points[i]));

        //poner bresenhamm y DDA

        projected_points[i] = projected_point;
    }

} 


void render(void){
    draw_grid();

    if(typeOfFigure == 0){
        for(int i = 0; i<N_POINTS; i++){
            vec2_t projected_point = projected_points[i];
            if(i != N_POINTS-1){
                draw_line(projected_points[i].x, projected_points[i].y, projected_points[i+1].x, projected_points[i+1].y, 0xFFFF00FF);
            }
        }
    }
    
    if(typeOfFigure == 1){
        int d = 3-2*r;
        int x = 0;
        int y = r;

        draw_circle(x,y,xc,xc,0x00FF0000);
        while (y>=x){
            if (d>=0){
                y--;
                d = d+4*(x-y)+r;
            }else{
                d = d+4+x+6;
            }
            x++;
            draw_circle(x,y,xc,xc,0x00FF0000);
        }
    }

    if(typeOfFigure == 3){
        draw_ellipse(rx, ry, xc, yx, 0x0000ff00);
    }

    if(typeOfFigure == 4){
        pf(0x0000ff00);
    }

    render_color_buffer();
    clear_color_buffer(0xFF000000);

    SDL_RenderPresent(renderer);
}


int main(int argc, char *argv[]){

    is_running = initialize_window();
    setup();
    while (is_running){
        previous_frame_time = SDL_GetTicks();
        process_input();
        update();
        render();
        int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
        if(time_to_wait >0 && time_to_wait <= FRAME_TARGET_TIME){
            SDL_Delay(time_to_wait);
        }
    }
    destroy_window();
    return 0;
}