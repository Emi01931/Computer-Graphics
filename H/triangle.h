#ifndef INC_3DRENDERER_TRIANGLE_H
#define INC_3DRENDERER_TRIANGLE_H

#include <stdint.h>
#include "vector.h"

typedef uint32_t color_t;

typedef struct {
    int a;
    int b;
    int c;
    color_t color;
} face_t;

typedef struct {
    vec2_t points[3];
    float depth;
    color_t color;
    vec3_t normalVec;
    float lightI1;
    float lightI2;
    float lightI3;
    vec3_t I1;
    vec3_t I2;
    vec3_t I3;
} triangle_t;

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);

#endif //INC_3DRENDERER_TRIANGLE_H
