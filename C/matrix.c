#include <math.h>
#include "matrix.h"

mat4_t mat4_identity(void){
    mat4_t matriz = {{
            {1,    0,    0,    0},
            {0,    1,    0,    0},
            {0,    0,    1,    0},
            {0,    0,    0,    1}
                    }};
    return matriz;
}
mat4_t mat4_proyection(){
    double pi = 3.14159265358979323846;
    double fov_factor = pi/3;
    
    float AspectRatio = (float)window_height/window_width;
    float zfar = 100.0;
    float znear = 0.1;
    float scalingFactor = (float)1/tan(fov_factor/2);
    float zNormalitation = (float)zfar/(zfar-znear);
    float scalingOffset = (float)zNormalitation*znear;


    mat4_t m = {{ 
            {AspectRatio*scalingFactor,  0,                  0,                                  0},
            {0,                          scalingFactor,      0,                                  0},
            {0,                          0,                  zNormalitation,        -scalingOffset},    
            {0,                          0,                  1,                                  0}              
    }}; 

    return m;
}

mat4_t mat4_make_scale(float sx, float sy, float sz){
    mat4_t m = mat4_identity();
        m.m[0][0] = sx;
        m.m[1][1] = sy;
        m.m[2][2] = sz;
    return m;
}
mat4_t mat4_make_translation(float tx, float ty, float tz){
    mat4_t m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}
mat4_t mat4_make_rotation_x(float angle){
    float c = cos(angle);
    float s = sin(angle);

    mat4_t m = mat4_identity();
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
}
mat4_t mat4_make_rotation_y(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    // |  c  0  s  0 |
    // |  0  1  0  0 |
    // | -s  0  c  0 |
    // |  0  0  0  1 |
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;
}
mat4_t mat4_make_rotation_z(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    // | c -s  0  0 |
    // | s  c  0  0 |
    // | 0  0  1  0 |
    // | 0  0  0  1 |
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    return m;
}
vec4_t mat4_mul_vec4(mat4_t m, vec4_t v) {
    vec4_t result;

    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;
        
    return result;
}
mat4_t mat4_mul_mat4(mat4_t a, mat4_t b) {
    mat4_t m;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }
    return m;
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