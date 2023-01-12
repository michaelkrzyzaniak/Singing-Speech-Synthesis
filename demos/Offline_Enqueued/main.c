//OSX Compile with:
//gcc *.c ../../lib/*.c

#include <stdio.h>
#include "../../Singer.h"



/*------------------------------------------------------------------------------------*/
int main(int argc, const char** argv)
{
  const int buffer_size = 1024;
  const int sample_rate = 44100;

  char* wavetable_aiff = "../../audio/wavetables/Cello_41.aiff";
  char* allophone_folder = "../../audio/tenor_allophones/";
  
  Singer* singer = singNew(wavetable_aiff, allophone_folder, sample_rate, buffer_size);
  if(!singer) {perror("unable to create singer object"); exit(-1);}

  MKAiff* aiff = aiffWithDurationInSeconds(1, sample_rate, 16, 60);
  if(!aiff) {perror("unable to create aiff object"); exit(-1);}

  float* buffer = calloc(buffer_size, sizeof(*buffer));
  if(!buffer) {perror("unable to allocate buffer"); exit(-1);}

  char allophones[] = "a-|m|a-|r|i-|l|i-|m|i|a-| |b|e-|l|a-| -";
  singEnqueueAllophones(singer, allophones);
  singSetLoudness(singer, 0.5);
  
  int buffers_per_allophone = ceil(sample_rate / (double)buffer_size);
  int pitch = 60; //MIDI note middle c
  int i;
  
  while(listCount(singEnqueuedAllophones(singer)) > 0)
    {
      singSetPitch(singer, pitch++, YES);
      singTriggerNextVowel(singer);       //go to the next hold symbol and wait
      //singTriggerNextAllophone(singer); //go to the next allophone and wait
      
      for(i=0; i<buffers_per_allophone; i++)
        {
          singFillBuffer(singer, buffer, buffer_size);
          aiffAppendFloatingPointSamples(aiff, buffer, buffer_size, aiffFloatSampleType);
        }
      }
  
  aiffSaveWaveWithFilename(aiff, "Test.wav");
  
  singDestroy(singer);
  
  return 0;	
}
