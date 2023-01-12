//Compile with
//gcc *.c ../../lib/*.c

#include "../../Singer.h"

int main(void)
{
  const int buffer_size = 1024;

  char* wavetable_aiff = "../../audio/wavetables/Cello_41.aiff";
  char* allophone_folder = "../../audio/tenor_allophones/";
  
  Singer* singer = singNew(wavetable_aiff, allophone_folder, 44100, buffer_size);

  float buffer[buffer_size];

  //char allophone[] = "a"; //sing 'a' then stop
  char allophone[] = "a-";  //sing 'a' and sustain
  singSetAllophone(singer, allophone);
  singSetPitch(singer, 60 /*MIDI note middle C*/, YES);
  
  while(1)
    {
      singFillBuffer(singer, buffer, buffer_size);
      /* buffer now contains audio */
      /* do something with it here */
    }
  
  singDestroy(singer);
  return 0;
}
