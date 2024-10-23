#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vector.h"
#include "matrix.h"

float vec2_length(vec2_t v){ //Si
    float resultado = sqrt ( pow(v.x,2)+ pow(v.y,2));
    return resultado;
}
vec2_t vec2_add(vec2_t a, vec2_t b){ //si
    vec2_t suma;
    
    suma.x = a.x + b.x;
    suma.y = a.y + b.y;

    return suma;
}
vec2_t vec2_sub(vec2_t a, vec2_t b){ //si
    vec2_t resta;
    
    resta.x = a.x - b.x;
    resta.y = a.y - b.y;

    return resta;
}
vec2_t vec2_mul(vec2_t v, float factor){ //si
    vec2_t mult;
    
    mult.x = v.x * factor;
    mult.y = v.y * factor;

    return mult;
}
vec2_t vec2_div(vec2_t v, float factor){ //si
    vec2_t resul;
    
    resul.x = v.x / factor;
    resul.y = v.y / factor;

    return resul;
}
void vec2_normalize(vec2_t* v){ //si
    float temp = sqrt ( pow(v->x,2)+ pow(v->y,2));
    v->x = (v->x)/temp;
    v->y = (v->y)/temp;
}

// Vector 3D functions
float vec3_length(vec3_t v){ //si
    float resultado = sqrt ( pow(v.x,2)+ pow(v.y,2) + pow(v.z,2));
    return resultado;
}
vec3_t vec3_add(vec3_t a, vec3_t b){ //si
    vec3_t suma;
    
    suma.x = a.x + b.x;
    suma.y = a.y + b.y;
    suma.z = a.z + b.z;

    return suma;
}
vec3_t vec3_sub(vec3_t a, vec3_t b){ //si
    vec3_t resta;
    
    resta.x = a.x - b.x;
    resta.y = a.y - b.y;
    resta.z = a.z - b.z;

    return resta;
}
vec3_t vec3_mul(vec3_t v, float factor){ //si
    vec3_t mult;
    
    mult.x = v.x * factor;
    mult.y = v.y * factor;
    mult.z = v.z * factor;

    return mult;
}
vec3_t vec3_div(vec3_t v, float factor){ //si
    vec3_t mult;
    
    mult.x = v.x / factor;
    mult.y = v.y / factor;
    mult.z = v.z / factor;

    return mult;
}
vec3_t vec3_cross(vec3_t a, vec3_t b){ //si
    vec3_t resul;

    resul.x = a.y*b.z - a.z*b.y;
    resul.y = a.z*b.x - a.x*b.z;
    resul.z = a.x*b.y - a.y*b.x;

    return resul; 
}
float vec3_dot(vec3_t a, vec3_t b){ //si
    float resul = a.x*b.x + a.y*b.y + a.z*b.z;
    return resul;
}
void vec3_normalize(vec3_t* v){ //si
    float temp = sqrt ( pow(v->x,2) + pow(v->y,2) + pow(v->z,2));
    v->x = (v->x)/temp;
    v->y = (v->y)/temp;
    v->z = (v->z)/temp;
}

vec3_t vec3_rotate_x(vec3_t v, float angle){ //no se
    vec3_t vecRot = {v.x, v.y*cos(angle)-v.z*sin(angle) , v.z*cos(angle)+v.y*sin(angle)};
    return vecRot;
}
vec3_t vec3_rotate_y(vec3_t v, float angle){ //no se
    vec3_t vec = {v.x*cos(angle)+v.z*sin(angle), v.y, v.z*cos(angle)-v.x*sin(angle)};
    return vec;
}
vec3_t vec3_rotate_z(vec3_t v, float angle){ //no se
    vec3_t vec = {v.x*cos(angle)-v.y*sin(angle), v.y*cos(angle)+v.x*sin(angle), v.z};
    return vec;
}

// Vector conversion functions
vec4_t vec4_from_vec3(vec3_t v){
    vec4_t resul = {v.x,v.y,v.z,1.0}; //tenia 0
    return resul;
}
vec3_t vec3_from_vec4(vec4_t v){
    vec3_t resul = {v.x, v.y, v.z};
    //vec3_t resul = {v.x+v.y, v.z+v.w, v.x+v.z};
    //vec3_t resul = {v.x, v.y, v.w};
    return resul;
}

/*
int main(int argc, char *argv[]){
    //vec2_t a = {1.0,2.0};
    //vec2_t b = {3.0,4.0};
    vec3_t a3 = {1.0,2.0,3.0};
    //vec3_t b3 = {4.0,5.0,6.0};
    vec4_t a4 = {1.0, 2.0, 3.0, 4.0};

    float le = vec2_length(a);
    printf("%.2f", le);
    vec2_t res = vec2_add(a,b);
    printf("%f %f", res.x, res.y);
    vec2_t ress = vec2_sub(a,b);
    printf("%f %f", ress.x, ress.y);
    vec2_t resm = vec2_mul(a,2);
    printf("%f %f", resm.x, resm.y);
    vec2_t resd = vec2_div(a,2);
    printf("%f %f", resd.x, resd.y);
    vec2_normalize(&a);
    printf("%.2f %.2f", a.x, a.y);
    float le = vec3_length(a3);
    printf("%.2f ", le);
    vec3_t res = vec3_add(a3,b3);
    printf("%f %f %f", res.x, res.y, res.z);
    vec3_t ress = vec3_sub(a3,b3);
    printf("%f %f %f", ress.x, ress.y, ress.z);
    vec3_t resm = vec3_mul(a3,2);
    printf("%.2f %.2f %.2f ", resm.x, resm.y, resm.z);
    vec3_t resd = vec3_div(a3,2);
    printf("%.2f %.2f %.2f ", resd.x, resd.y, resd.z);
    vec3_t resc = vec3_cross(a3,b3);
    printf("%.2f %.2f %.2f ", resc.x, resc.y, resc.z);
    float resdot = vec3_dot(a3,b3);
    printf("%.2f", resdot);
    vec3_normalize(&a3);
    printf("%.2f %.2f %.2f", a3.x, a3.y, a3.z);


    //vec3_t resrx = vec3_rotate_x(a3,2);
    //printf("%.2f %.2f %.2f \n", resrx.x, resrx.y, resrx.z);

    //vec3_t resry = vec3_rotate_y(a3,2);
    //printf("%.2f %.2f %.2f \n", resry.x, resry.y, resry.z);

    //vec3_t resrz = vec3_rotate_z(a3,2);
    //printf("%.2f %.2f %.2f \n", resrz.x, resrz.y, resrz.z);
    vec3_t res34 = vec3_from_vec4(a4);
    printf("%.1f %.1f %.1f \n", res34.x, res34.y, res34.z);

    vec4_t res43 = vec4_from_vec3(a3);
    printf("%.1f %.1f %.1f %.1f ", res43.x, res43.y, res43.z, res43.w);

}
*/