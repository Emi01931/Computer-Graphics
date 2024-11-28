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

triangle_t* ArrayTriangle = NULL;
light_t light = {.directions = {0,0,1}};
color_t originalColor = 0x00ffff00;
color_t lineColor = 0xFFFF00FF;

vec3_t Figure_rotation = {0,0,0};
vec3_t Figure_translation = {0,0,0};
vec3_t Figure_scale = {1,1,1};

mat4_t mat_proyection;

bool is_running = false;
int previous_frame_time = 0;


bool hideColor = false;
bool hideVertex = false;
bool hideEdge = true;
bool hideFlatShading = false;

//Algorith to sort faces by depth
void shell(){
    int num = array_length(ArrayTriangle);
    int i, j, k;
    triangle_t tmp;

    for (i = num / 2; i > 0; i = i / 2){
        for (j = i; j < num; j++){
            for(k = j - i; k >= 0; k = k - i){
                if (ArrayTriangle[k+i].depth <= ArrayTriangle[k].depth)
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
    mat_proyection = mat4_proyection();


    char fileName[] = "shield1.obj";
    load_obj_file_data(fileName);
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

                case SDL_SCANCODE_S:
                    if (hideFlatShading == false){
                        hideFlatShading = true;
                    }else{
                        hideFlatShading = false;
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
    //Orthographic
    vec2_t projected_point = {
            .x = (fov_factor * v3.x),
            .y = (fov_factor * v3.y)
    };
*/


    //perspective
    //first we change the vector to R3 so we can multiply it with the matriz of proyection
    vec4_t tv4 = vec4_from_vec3(v3);
    vec4_t v4 = mat4_mul_vec4(mat_proyection, tv4);

    //perspective divide
    vec3_t perspective_divide = {
            .x = v4.x / v4.w,
            .y = v4.y / v4.w,
            .z = v4.z / v4.w
    };

    //inverting the Y axis
    perspective_divide.y *= -1;

    //scaling
    perspective_divide.x = perspective_divide.x*(window_width/2.0);
    perspective_divide.y = perspective_divide.y*(window_height/2.0);

    //centering
    //perspective_divide.x += window_width/2.0;
    //perspective_divide.y += window_height/2.0;

    vec2_t projected_point = {
            .x = perspective_divide.x,
            .y = perspective_divide.y
    };

    return projected_point;
}

void update(void){
    //y ->
    //x ^
    ArrayTriangle = NULL;
    Figure_rotation.x += 0.01;
    Figure_rotation.y += 0.008;
    Figure_rotation.z = 0;
    Figure_translation.z = 5;



    mat4_t scale_matrix = mat4_make_scale(Figure_scale.x, Figure_scale.y, Figure_scale.z);
    mat4_t rotation_matrix_x = mat4_make_rotation_x(Figure_rotation.x);
    mat4_t rotation_matrix_y = mat4_make_rotation_y(Figure_rotation.y);
    mat4_t rotation_matrix_z = mat4_make_rotation_z(Figure_rotation.z);
    mat4_t translation_matrix = mat4_make_translation(Figure_translation.x, Figure_translation.y, Figure_translation.z);

    mat4_t world_matrix = mat4_identity();
    world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
    world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
    world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);



    for(int i = 0; i< array_length(mesh.faces); i++){
        vec4_t transformed_points[3];
        vec3_t tempTransformedPoint[3];
        vec2_t projected_points[3];
        vec3_t verticesCara[3];

        int verticeCara[3] = {
            mesh.faces[i].a - 1, 
            mesh.faces[i].b - 1,
            mesh.faces[i].c - 1
        };
        verticesCara[0] = mesh.vertices[verticeCara[0]];
        verticesCara[1] = mesh.vertices[verticeCara[1]];
        verticesCara[2] = mesh.vertices[verticeCara[2]];

        //Transforming and projecting vertex
        for(int j = 0; j<3;j++){
            vec4_t transformed_point = vec4_from_vec3(verticesCara[j]);
            transformed_point = mat4_mul_vec4(world_matrix, transformed_point);
            transformed_points[j] = transformed_point;
            tempTransformedPoint[j] = vec3_from_vec4(transformed_points[j]);
        }
        
        //Calculating camRay and if the faces are visible
        vec3_t camaraPosition = {0,0,0};

        vec3_t vecA = vec3_sub(tempTransformedPoint[0], tempTransformedPoint[1]); //v1-v2
        vec3_t vecB = vec3_sub(tempTransformedPoint[0], tempTransformedPoint[2]); //v1-v3

        vec3_normalize(&vecA);
        vec3_normalize(&vecB);

        vec3_t FaceNormalVect = vec3_cross(vecA, vecB);
        vec3_t camaraRay = vec3_sub(camaraPosition, tempTransformedPoint[0]);
        float RenderingCondition = vec3_dot(camaraRay, FaceNormalVect);


        if(RenderingCondition > 0){
            //flat shading
            color_t newColor;
            if(hideFlatShading == false){
                vecA = vec3_sub(tempTransformedPoint[0], tempTransformedPoint[1]);
                vecB = vec3_sub(tempTransformedPoint[0], tempTransformedPoint[2]);
                FaceNormalVect = vec3_cross(vecA, vecB);
                vec3_normalize(&FaceNormalVect);

                float lightIntensity = - vec3_dot(FaceNormalVect, light.directions);
                newColor = light_apply_intensity(originalColor, lightIntensity);
            }else{
                newColor = originalColor;
            }



            for(int j = 0 ; j<3 ; j++){
                vec2_t projected_point = project(tempTransformedPoint[j]);
                projected_points[j] = projected_point;
                projected_points[j].x += (window_width/2);
                projected_points[j].y += (window_height/2);
            }

            triangle_t trianguloProyectado = {
                .points[0] = projected_points[0],
                .points[1] = projected_points[1],
                .points[2] = projected_points[2],
                .depth     = (FaceNormalVect.x+FaceNormalVect.y+FaceNormalVect.z)/3,
                .color     = newColor
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
            draw_pixel(tempTriangle.points[0].x, tempTriangle.points[0].y, tempTriangle.color);
            draw_pixel(tempTriangle.points[1].x, tempTriangle.points[1].y, tempTriangle.color);
            draw_pixel(tempTriangle.points[2].x, tempTriangle.points[2].y, tempTriangle.color);
        }
        if(hideEdge == false)
            draw_triangle(tempTriangle.points[0].x, tempTriangle.points[0].y, tempTriangle.points[1].x, tempTriangle.points[1].y, tempTriangle.points[2].x, tempTriangle.points[2].y, lineColor);
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
            }


            float mx = (((temp1.y-temp0.y)*(temp2.x-temp0.x))/
                        (temp2.y-temp0.y)) + temp0.x;

            float my = temp1.y;

            if((int)temp0.y == (int)temp1.y){
                draw_flat_top(temp0.x, temp0.y, temp1.x, temp1.y, temp2.x, temp2.y, tempTriangle.color);
            }else if((int)temp1.y == (int)temp2.y){
                draw_flat_bottom(temp0.x, temp0.y, temp1.x, temp1.y, temp2.x, temp2.y, tempTriangle.color);
            }else{
                draw_flat_bottom(temp0.x, temp0.y, temp1.x, temp1.y, mx, my, tempTriangle.color);
                draw_flat_top(temp1.x, temp1.y, mx, my, temp2.x, temp2.y, tempTriangle.color);
            }

            if((int)temp0.y == (int)temp2.y && (int)temp1.y == (int)temp0.y){
                draw_pixel(temp0.x, temp0.y, tempTriangle.color);
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