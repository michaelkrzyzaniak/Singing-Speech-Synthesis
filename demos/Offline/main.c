//OSX Compile with:
//gcc *.c ../../lib/*.c


#include <stdio.h>
#include "../../Singer.h"



/*------------------------------------------------------------------------------------*/
int main(int argc, const char** argv)
{
  const int buffer_size = 128;
  const int sample_rate = 44100;
  const int recording_seconds = 10;

  char* wavetable_aiff = "../../audio/wavetables/Cello_41.aiff";
  char* allophone_folder = "../../audio/tenor_allophones/";
  
  Singer* singer = singNew(wavetable_aiff, allophone_folder, sample_rate, buffer_size);
  if(!singer) {perror("unable to create singer object"); exit(-1);}

  MKAiff* aiff = aiffWithDurationInSeconds(1, sample_rate, 16, recording_seconds+1);
  if(!aiff) {perror("unable to create aiff object"); exit(-1);}

  float* buffer = calloc(buffer_size, sizeof(*buffer));
  if(!buffer) {perror("unable to allocate buffer"); exit(-1);}

  char allophone[] = "a-";
  singSetAllophone(singer, allophone);
  singSetPitch(singer, 60 /*MIDI note middle C*/, YES);
  singSetLoudness(singer, 0.5);
  
  int num_buffers = ceil(recording_seconds * sample_rate / (double)buffer_size);
  int i;

  for(i=0; i<num_buffers; i++)
    {
      singFillBuffer(singer, buffer, buffer_size);
      aiffAppendFloatingPointSamples(aiff, buffer, buffer_size, aiffFloatSampleType);
    }
  
  
  aiffSaveWaveWithFilename(aiff, "Test.wav");
  
  singDestroy(singer);
  
  return 0;	
}
