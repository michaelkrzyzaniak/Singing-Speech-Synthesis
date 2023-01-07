#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TWO_PI 6.283185307179586

#ifndef BOOL 
#define BOOL int
#define NO 0
#define YES (!NO)
#endif

#define CONSTRAIN(x, MIN, MAX) ((x) = ((x) < (MIN)) ? (MIN) : ((x) > (MAX)) ? (MAX) : (x))
float scalef(float x, float in_min, float in_max, float out_min, float out_max);
void  scaleb(float* buffer, int numSamples, float min, float max);
char* basename(char* path);
