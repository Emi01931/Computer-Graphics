#include <stdio.h>
#include <stdint.h>
#include "display.h"

#define maxHt 800
#define maxWd 600
#define maxVer 10000


typedef struct{
    int ymax;   //max y-coordinate of edge
    float xofymin;  //x-coordinate of lowest edge point updated only in aet
    float slopeinverse;
}EdgeBucket;
 
typedef struct{
    // the array will give the scanline number
    // The edge table (ET) with edges entries sorted 
    // in increasing y and x of the lower end
     
    int countEdgeBucket;    //no. of edgebuckets
    EdgeBucket buckets[maxVer];
}EdgeTableTuple;

void initEdgeTable();
void insertionSort(EdgeTableTuple *ett);
void storeEdgeInTuple (EdgeTableTuple *receiver,int ym,int xm,float slopInv);
void storeEdgeInTable (int x1,int y1, int x2, int y2);
void removeEdgeByYmax(EdgeTableTuple *Tup,int yy);
void updatexbyslopeinv(EdgeTableTuple *Tup);
void ScanlineFill(color_t color);
void drawPolyLines(color_t color);
void drawPoly(color_t color);
void pf(color_t color);