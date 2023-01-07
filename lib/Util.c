#include "Util.h"
#include <string.h>

float scalef(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void  scaleb(float* buffer, int numSamples, float min, float max)
{
  float minSample = 1000000/*FLT_MAX*/, maxSample = 0;
  float* b = buffer; 
  int    N = numSamples;
  
  
  while(N--)
    {
      if(*b < minSample)
        minSample = *b;
      if(*b > maxSample)
        maxSample = *b;
      b++;
    }
    
  N = numSamples;
  b = buffer;
  
  while(N-- > 0)
    {
      *b = scalef(*b, minSample, maxSample, min, max);
      b++;
    }
}

/*-----------------------------------------------------*/
char* basename(char* path)
{
  int len = strlen(path);

  char* base = path;
  int i;
  
  for(i=len-1; i>=0; i--)
    {
      if(path[i] == '.')
        {
          path[i] = '\0';
          break;
        }
    }
  
  len = strlen(path);
  
  for(i=len-1; i>=0; i--)
    {
      if(path[i] == '/')
        {
          base = path + i + 1;
          break;
        }
    }

    return base;
}