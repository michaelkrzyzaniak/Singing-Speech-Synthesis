#ifndef  __MK_SINE_TABLE__     
#define  __MK_SINE_TABLE__ 1 

#ifdef __cplusplus            
extern "C" {                
#endif                      

#include <stdint.h>

#define  SIN_NUM_SAMPLES        4096
#define  SIN_TWO_PI             0xFFFFFFFF
#define  SIN_PI                 0x7FFFFFFF
#define  SIN_HALF_PI            0x3FFFFFFF
#define  SIN_QUARTER_PI         0x1FFFFFFF

float fastsin  (uint32_t angle);
float fastcos  (uint32_t angle);

#define fastsin(angle) (*(sinTable + ((angle) >> 20)))
#define fastcos(angle) (*(sinTable + (((angle) + SIN_HALF_PI) >> 20)))

const extern float sinTable[SIN_NUM_SAMPLES];

#ifdef __cplusplus            
}                             
#endif                      

#endif//__MK_SINE_TABLE__     
