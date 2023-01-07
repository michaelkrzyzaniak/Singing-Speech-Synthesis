/*
 *  Synth.h
 *  Make weird noises
 *
 *  Made by Michael Krzyzaniak at Arizona State University's
 *  School of Arts, Media + Engineering in Spring of 2013
 *  mkrzyzan@asu.edu
 */

#include "Synth.h"
#include "stdio.h"

int syAudioCallback(void* SELF, auSample_t* buffer, int numFrames, int numChannels);

/*OpaqueSynthStruct----------------------------------------*/
struct OpaqueSynthStruct
{
  AUDIO_GUTS        ;
  Singer*      tenor;
};

/*syNew---------------------------------------------------*/
Synth* syNew()
{
  Synth* self = (Synth*) auAlloc(sizeof(*self), syAudioCallback, YES, 1);
  if(self != NULL)
    {
      self->tenor = singNew("../../audio/wavetables/Cello_41.aiff", "../../audio/tenor_allophones/", AU_SAMPLE_RATE, AU_BUFFER_NUM_FRAMES);
      if(self->tenor == NULL) 
        return syDestroy(self);
    }
  return self;
}

/*syDestroy-----------------------------------------------*/
Synth* syDestroy(Synth* self)
{
  if(self != NULL)
    {
      if(self->tenor != NULL)
        singDestroy(self->tenor);
    }
  return (Synth*)auDestroy((Audio*)self);
}

/*--------------------------------------------------------*/
Singer* syTenor(Synth* self)
{
  return self->tenor;
}

/*syAudioCallback-----------------------------------------*/
int syAudioCallback(void* SELF, auSample_t* buffer, int numFrames, int numChannels)
{
  Synth* self = (Synth*)SELF;
  singFillBuffer(self->tenor, buffer, numFrames);
  return numFrames;
}
