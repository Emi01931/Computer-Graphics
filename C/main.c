#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <strings.h>
#include "array.h"
#include "display.h"
#include "matrix.h"
#include "mesh.h"
#include "triangle.h"
#include "vector.h"

#define N_POINTS (9*9*9)
vec3_t cube_points[N_POINTS];
vec2_t projected_points[N_POINTS];

//triangle_t* ArrayTriangle = NULL;

vec3_t cube_rotation = {0,0,0};
vec3_t cube_translation = {0,0,0};
vec3_t cube_scale = {1,1,1};

float fov_factor = 200;
bool is_running = false;
int previous_frame_time = 0;

int radious = 10;
int radiousA = 35;
int typeOfFigure = 2; // 0 = cube, 1 = circle, 2 .obj

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
    
    if (typeOfFigure == 1){
        for(float row = -radious; row <= radious; row += 0.1){
            for(float col = -radious; col <= radious; col += 0.1){
                float x = col - radious/2;
                float y = radious/2 - row;
                float sumsq = x*x + y*y;

                vec3_t vec_temp = {
                    .x = row*radiousA,
                    .y = col*radiousA,
                    .z = 0
                };

                //printf("\n%i: x=%f    y=%f",point_count+1, vec_temp.x, vec_temp.y);

                if((sumsq < radious + 0.5) && (sumsq > radious - 0.5)){
                    cube_points[point_count] = vec_temp;
                    //printf("\n%i: x=%f    y=%f",point_count+1, cube_points[point_count].x, cube_points[point_count].y);
                    point_count++;
                }
            }
        }
    }

    if(typeOfFigure == 2){
        char fileName[] = "cube.obj";
        //load_obj_file_data(fileName);
        load_cube_mesh_data();
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
*/
    //perspectiva
    vec2_t projected_point = {
            .x = (fov_factor * v3.x) / v3.z,
            .y = (fov_factor * v3.y) / v3.z
    };

/*
    //not project
    vec2_t projected_point = {
            .x = v3.x,
            .y = v3.y
    };
*/
    return projected_point;
}


void update(void){
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.01;
    cube_rotation.z += 0.01;
    //cube_rotation.x = 0;
    //cube_rotation.y = 0;
    //cube_rotation.z = 0;

    cube_translation.z -= 0.02;
    //cube_translation.z = 0;

    mat4_t scale_matrix = mat4_make_scale(cube_scale.x, cube_scale.y, cube_scale.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(cube_rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(cube_rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(cube_rotation.z);
    mat4_t translation_matrix = mat4_make_translation(cube_translation.x, cube_translation.y, cube_translation.z);

    mat4_t world_matrix = mat4_identity();

    //O quizas lo del object va aqui arriba, debajo del world matrix

    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

    for(int i = 0; i<array_length(mesh.faces); i++){
        int verticeCara[3] = {          //se guardan los vertices por cara
            mesh.faces[i].a - 1, 
            mesh.faces[i].b - 1,
            mesh.faces[i].c - 1
        };
        vec3_t va = {
            .x = mesh.vertices[verticeCara[0]].x,
            .y = mesh.vertices[verticeCara[0]].y,
            .z = mesh.vertices[verticeCara[0]].z
        };
        vec3_t vb = {
            .x = mesh.vertices[verticeCara[1]].x,
            .y = mesh.vertices[verticeCara[1]].y,
            .z = mesh.vertices[verticeCara[1]].z
        };
        vec3_t vc = {
            .x = mesh.vertices[verticeCara[2]].x,
            .y = mesh.vertices[verticeCara[2]].y,
            .z = mesh.vertices[verticeCara[2]].z
        };

//Transformando y proyectando el vertice a
        vec4_t transformed_points[3];
        vec4_t transformed_point = vec4_from_vec3(va);
        transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
        transformed_points[0] = transformed_point;
        vec2_t projected_point = project(vec3_from_vec4(transformed_points[0]));
        projected_points[0] = projected_point;
        draw_rect(projected_points[0].x + window_width/2, projected_points[0].y + window_height/2, 4, 4, 0x0000ff00); //green

//Transformando y proyectando el vertice b
        transformed_point = vec4_from_vec3(vb);
        transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
        transformed_points[1] = transformed_point;
        projected_point = project(vec3_from_vec4(transformed_points[1]));
        projected_points[1] = projected_point;
        draw_rect(projected_points[1].x + window_width/2, projected_points[1].y + window_height/2, 4, 4, 0x000000ff); //blue

//Transformando y proyectando el vertice c
        transformed_point = vec4_from_vec3(vc);
        transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
        transformed_points[2] = transformed_point;
        projected_point = project(vec3_from_vec4(transformed_points[2]));
        projected_points[2] = projected_point;
        draw_rect(projected_points[2].x + window_width/2, projected_points[2].y + window_height/2, 4 , 4 , 0x00ff0000); //red

        triangle_t trianguloProyectado = {  //Se guardan los puntos proyectados que conforman el triangulo
            .points[0] = projected_points[0],
            .points[1] = projected_points[1],
            .points[2] = projected_points[2]
        };

        array_push(ArrayTriangle, trianguloProyectado);
        //extrae los vertices, vect3 para cada vertice, transformar esos 3 vertices, proyectarlos y dibujarlos a la mitad de la pantalla, por cada cara crear un traingle_t, array push a un apuntador de triangle_t 
    }
} 


void render(void){
    draw_grid();

    printf("\n%i", array_length(ArrayTriangle));
    for (int i = 0; i < array_length(ArrayTriangle); i++)
    {
        draw_triangle(ArrayTriangle->points[0].x, ArrayTriangle->points[0].y, ArrayTriangle->points[1].x, ArrayTriangle->points[1].y, ArrayTriangle->points[2].x, ArrayTriangle->points[2].y, 0xFFFF00FF);
    }

    render_color_buffer();
    clear_color_buffer(0xFF000000);
    printf("\nBefore Array free");
    array_free(ArrayTriangle);
    //memset(ArrayTriangle, 0, sizeof(ArrayTriangle));
    //free(ArrayTriangle);
    printf("\tAfter Array free");
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