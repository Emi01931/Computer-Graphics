#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"

mat4_t mat4_identity(void){
    mat4_t matriz = {   1.0,    0.0,    0.0,    0.0,
                        0.0,    1.0,    0.0,    0.0,
                        0.0,    0.0,    1.0,    0.0,
                        0.0,    0.0,    0.0,    1.0
                    };
    return matriz;
}

mat4_t mat4_make_scale(float sx, float sy, float sz){
    mat4_t matriz = {   sx,0.0,0.0,0.0,
                        0.0,sy,0.0,0.0,
                        0.0,0.0,sz,0.0,
                        0.0,0.0,0.0,1.0
                    };
    return matriz;
}
mat4_t mat4_make_translation(float tx, float ty, float tz){
    mat4_t matriz = {   1.0,0.0,0.0,tx,
                        0.0,1.0,0.0,ty,
                        0.0,0.0,1.0,tz,
                        0.0,0.0,0.0,1.0
                    };
    return matriz;
}
mat4_t mat4_make_rotation_x(float angle){
    mat4_t matriz = {   1.0           ,0.0         ,0.0      ,0.0,
                        0.0           ,cos(angle),-sin(angle),0.0,
                        0.0           ,sin(angle),cos(angle) ,0.0,
                        0.0           ,0.0       ,0.0        ,1.0
                    };
    return matriz;
}
mat4_t mat4_make_rotation_y(float angle){
    mat4_t matriz = {   cos(angle)  ,0.0         ,sin(angle) ,0.0,
                        0.0         ,1.0         ,0.0        ,0.0,
                        -sin(angle) ,0.0         ,cos(angle) ,0.0,
                        0.0           ,0.0       ,0.0        ,1.0
                    };
    return matriz;
}
mat4_t mat4_make_rotation_z(float angle){
    mat4_t matriz = {   cos(angle)  ,-sin(angle),0.0          ,0.0,
                        sin(angle)  ,cos(angle) ,0.0          ,0.0,
                        0.0         ,0.0        ,1.0          ,0.0,
                        0.0         ,0.0        ,0.0          ,1.0
                    };
    return matriz;
}
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v){ //si sirve
//f,c
    float resultado1 = m.m[0][0]*v.x + m.m[0][1]*v.y + m.m[0][2]*v.z + m.m[0][3]*v.w;
    float resultado2 = m.m[1][0]*v.x + m.m[1][1]*v.y + m.m[1][2]*v.z + m.m[1][3]*v.w;
    float resultado3 = m.m[2][0]*v.x + m.m[2][1]*v.y + m.m[2][2]*v.z + m.m[2][3]*v.w;
    float resultado4 = m.m[3][0]*v.x + m.m[3][1]*v.y + m.m[3][2]*v.z + m.m[3][3]*v.w;
    
    vec4_t vect = {     resultado1,
                        resultado2,
                        resultado3,
                        resultado4
                    };
    return vect;
}
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b){ // si sirve
/**/
    mat4_t producto;
    for (int k = 0; k < 4; k++) {
        for (int i = 0; i < 4; i++) {
            float suma = 0.0;
            for (int j = 0; j < 4; j++) {
                suma += a.m[i][j] * b.m[j][k];
            }
            producto.m[i][k] = suma;
        }
    }
    return producto;
}

/*
int main(int argc, char *argv[]){
    mat4_t matrizA = {3.0, 2.0, 1.0, 3.0, 1.0, 1.0, 3.0, 1.0, 0.0, 2.0, 1.0, 2.0, 3.0, 4.0, 1.0, 5.0};
    vec4_t matrizB = {1.0, 2.0, 3.0, 4.0};

    vec4_t producto = mat4_mul_vec4(matrizA, matrizB);

    printf("%.1f ", producto.x);
    printf("%.1f ", producto.y);
    printf("%.1f ", producto.z);
    printf("%.1f ", producto.w);
    
    /*
    mat4_t matrizA = {3.0, 2.0, 1.0, 3.0, 1.0, 1.0, 3.0, 1.0, 0.0, 2.0, 1.0, 2.0, 3.0, 4.0, 1.0, 5.0};
    mat4_t matrizB = {2.0, 1.0, 3.0, 5.0, 1.0, 0.0, 4.0, 6.0, 3.0, 2.0, 7.0, 2.0, 7.0, 3.0, 4.0, 1.0};

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            matrizA.m[i][j] += i+j;
        }
    }

    printf("Imprimiendo producto\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%f ", matrizA.m[i][j]);
        }
        printf("\n");
    }


    mat4_t producto = mat4_mul_mat4(matrizA, matrizB);

    printf("Imprimiendo producto\n");
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%.1f ", producto.m[i][j]);
        }
        printf("\n");
    }
    //
}
*/