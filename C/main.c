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

#define N_POINTS (9*9*9)
vec3_t cube_points[N_POINTS];

triangle_t* ArrayTriangle = NULL;
color_t color = 0x00ffff00;

vec3_t cube_rotation = {0,0,0};
vec3_t cube_translation = {0,0,0};
vec3_t cube_scale = {1,1,1};

//variables used in proyection
mat4_t mat_proyection;


bool is_running = false;
int previous_frame_time = 0;
int fov_factor = 600;


int typeOfFigure = 1; //cube = 0, .obj = 1
bool hideColor = false;
bool hideVertex = false;
bool hideEdge = false;

//tried quicksort and insert sort, but they were way too slow
//Algorith to sort for depth each face
void shell(){
    int num = array_length(ArrayTriangle);
    int i, j, k;
    triangle_t tmp;

    for (i = num / 2; i > 0; i = i / 2){
        for (j = i; j < num; j++){
            for(k = j - i; k >= 0; k = k - i){
                if (ArrayTriangle[k+i].depth >= ArrayTriangle[k].depth)
                    break;
                else{
                    tmp = ArrayTriangle[k];
                    ArrayTriangle[k] = ArrayTriangle[k+i];
                    ArrayTriangle[k+i] = tmp;
                }
            }
        }
    }

}

void setup(void){
    //Cada pixel usa el tipo de dato uin32_t
    color_buffer = (uint32_t*) malloc(sizeof (uint32_t)*window_width*window_height);
    if(!color_buffer){fprintf(stderr, "Error allocating memory for frame buffer,\n");}


    //things we need to do once
    mat_proyection = mat4_proyection();

    if (typeOfFigure == 0){
        int point_count = 0;
        for(float x=-1;x<=1;x+=0.25){
            for(float y=-1; y<=1;y+=0.25){
                for(float z=-1;z<=1;z+=0.25){
                    vec3_t new_point = {.x = x,.y = y,.z = z};
                    cube_points[point_count++] = new_point;
                }
            }
        }
    }

    if(typeOfFigure == 1){
        char fileName[] = "cube.obj";
        load_obj_file_data(fileName);
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
            if(event.key.keysym.sym == SDLK_ESCAPE){
                is_running = false;
            }else{
                switch (event.key.keysym.scancode){
                case SDL_SCANCODE_F:
                    if (hideColor == false){
                        hideColor = true;
                    }else{
                        hideColor = false;
                    }                    
                    break;

                case SDL_SCANCODE_L:
                    if (hideEdge == false){
                        hideEdge = true;
                    }else{
                        hideEdge = false;
                    }                    
                    break;

                case SDL_SCANCODE_V:
                    if (hideVertex == false){
                        hideVertex = true;
                    }else{
                        hideVertex = false;
                    }                    
                    break;
                
                default:
                    break;
                }
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


    return projected_point;
}

void update(void){
    //y ->
    //x ^
    ArrayTriangle = NULL;
    cube_rotation.x += 0.01;
    cube_rotation.y += 0.008;
    cube_rotation.z += 0.01;
    //cube_rotation.x = 0;
    //cube_rotation.y = 0;
    cube_rotation.z = 0;

    cube_translation.z = 5;
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

    for(int i = 0; i< array_length(mesh.faces); i++){
        int verticeCara[3] = {          //se guardan los vertices por cara
            mesh.faces[i].a - 1, 
            mesh.faces[i].b - 1,
            mesh.faces[i].c - 1
        };
        vec3_t verticesCara[3];

        verticesCara[0] = mesh.vertices[verticeCara[0]];
        verticesCara[1] = mesh.vertices[verticeCara[1]];
        verticesCara[2] = mesh.vertices[verticeCara[2]];

        vec4_t transformed_points[3];
        vec3_t tempTransformedPoint[3];
        vec2_t projected_points[3];

//Transformando y proyectando los vertices
        for(int j = 0; j<3;j++){
            vec4_t transformed_point = vec4_from_vec3(verticesCara[j]);
            transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
            transformed_points[j] = transformed_point;
            tempTransformedPoint[j] = vec3_from_vec4(transformed_points[j]);
        }
        
//calculando el camRay y si son visibles las caras

        vec3_t camaraPosition = {0,0,0};

        vec3_t vecA = vec3_sub(tempTransformedPoint[0], tempTransformedPoint[1]); //v1-v2
        vec3_t vecB = vec3_sub(tempTransformedPoint[0], tempTransformedPoint[2]); //v1-v3

        vec3_normalize(&vecA);
        vec3_normalize(&vecB);

        vec3_t FaceNormalVect = vec3_cross(vecA, vecB);
        vec3_t camaraRay = vec3_sub(camaraPosition, tempTransformedPoint[0]);
        float RenderingCondition = vec3_dot(camaraRay, FaceNormalVect);

        if(RenderingCondition > 0){
            for(int j = 0 ; j<3 ; j++){
                vec2_t projected_point = project(tempTransformedPoint[j]);
                projected_points[j] = projected_point;
                projected_points[j].x += (window_width/2);
                projected_points[j].y += (window_height/2);
            }

            triangle_t trianguloProyectado = {  //Se guardan los puntos proyectados que conforman el triangulo
                .points[0] = projected_points[0],
                .points[1] = projected_points[1],
                .points[2] = projected_points[2],
                .depth     = (FaceNormalVect.x+FaceNormalVect.y+FaceNormalVect.z)/3
            };
            array_push(ArrayTriangle, trianguloProyectado); 
        }
    }
} 

void render(void){
    draw_grid();
    int ArrayLen = array_length(ArrayTriangle);
    
    shell();

    for (int i = 0; i < ArrayLen ; i++){
        triangle_t tempTriangle = ArrayTriangle[i];

        if(hideVertex == false){
            draw_pixel(tempTriangle.points[0].x, tempTriangle.points[0].y, color);
            draw_pixel(tempTriangle.points[1].x, tempTriangle.points[1].y, color);
            draw_pixel(tempTriangle.points[2].x, tempTriangle.points[2].y, color);
        }

        if(hideEdge == false)
            draw_triangle(tempTriangle.points[0].x, tempTriangle.points[0].y, tempTriangle.points[1].x, tempTriangle.points[1].y, tempTriangle.points[2].x, tempTriangle.points[2].y, 0xFFFF00FF);
        
        if(hideColor == false){
            vec2_t temp0 = tempTriangle.points[0];//3
            vec2_t temp1;
            vec2_t temp2;

            if((int)temp0.y < (int)tempTriangle.points[1].y){
                temp1 = tempTriangle.points[1];
            }else{
                temp1 = temp0;
                temp0 = tempTriangle.points[1];
            }
            if((int)temp1.y < (int)tempTriangle.points[2].y){
                temp2 = tempTriangle.points[2];
            }else if((int)temp0.y < (int)tempTriangle.points[2].y){
                temp2 = temp1;
                temp1 = tempTriangle.points[2];
            }else{
                temp2 = temp1;
                temp1 = temp0;
                temp0 = tempTriangle.points[2];
            }/*
            if((int)temp2.y < (int)temp1.y){
                vec2_t temp = temp1;
                temp1 = temp2;
                temp2 = temp;
            }*/

            //printf("\n%.1f, %.1f, %.1f",temp0.y,temp1.y,temp2.y);

            //if(i>-1)
                //printf("\t%.1f, %.1f, %.1f",tempTriangle.points[0].y,tempTriangle.points[1].y,tempTriangle.points[2].y);


            float mx = (((temp1.y-temp0.y)*(temp2.x-temp0.x))/
                        (temp2.y-temp0.y)) + temp0.x;

            float my = temp1.y;

            //printf(" ,%i", mx);

            if((int)temp0.y == (int)temp1.y){
                draw_flat_top(temp0.x, temp0.y, temp1.x, temp1.y, temp2.x, temp2.y, color);
            }else if((int)temp1.y == (int)temp2.y){
                draw_flat_bottom(temp0.x, temp0.y, temp1.x, temp1.y, temp2.x, temp2.y, color);
            }else{
                draw_flat_bottom(temp0.x, temp0.y, temp1.x, temp1.y, mx, my, color);
                draw_flat_top(temp1.x, temp1.y, mx, my, temp2.x, temp2.y, color);
            }

            if((int)temp0.y == (int)temp2.y && (int)temp1.y == (int)temp0.y){
                draw_pixel(temp0.x, temp0.y, color);
            }
        }
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