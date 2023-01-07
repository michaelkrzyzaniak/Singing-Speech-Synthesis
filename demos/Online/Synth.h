/*
 *  Synth.h
 *  Make weird noises
 *
 *  Made by Michael Krzyzaniak at Arizona State University's
 *  School of Arts, Media + Engineering in Spring of 2013
 *  mkrzyzan@asu.edu
 */

#ifndef __SYNTH__
#define __SYNTH__ 1

#if defined(__cplusplus)
extern "C"{
#endif   //(__cplusplus)

#include "AudioSuperclass.h"
#include "../../Singer.h"

typedef struct OpaqueSynthStruct Synth;

Synth*  syNew                   (           );
Synth*  syDestroy               (Synth* self);
Singer* syTenor                 (Synth* self);

#if defined(__cplusplus)
}
#endif   //(__cplusplus)

#endif   // __Synth__
