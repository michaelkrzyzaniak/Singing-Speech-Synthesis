#ifndef __ALLOPHONE__
#define __ALLOPHONE__ 1

#if defined(__cplusplus)
extern "C"{
#endif   //(__cplusplus)

#include "MKAiff.h"
#include "FFT.h"

#define ALLO_NOISINESS_AIFF_MARKER_ID 1234
#define ALLO_CATEGORY_AIFF_MARKER_ID  5678

typedef enum
{
  alloUnknown,
  alloVowel,
  alloFricative,
  alloPlosive,
}alloPhonemeCategory_t;


typedef struct  OpaqueAllophoneStruct Allophone;
typedef char*   alloSymbol_t;

Allophone*   alloNew                   (alloSymbol_t symbol, MKAiff* aiff, FFT* fft);
Allophone*   alloDestroy               (Allophone* self);

alloSymbol_t alloSymbol                (Allophone* self);
fftSample_t* alloSpectralEnvelope      (Allophone* self, int* numSamples);
float        alloNoisiness             (Allophone* self);
float*       alloAmplitudeEnvelope     (Allophone* self, int* numSamples);
alloPhonemeCategory_t alloPhonemeCategory(Allophone* self);
float        alloAverageAmplitude      (Allophone* self);

//plosive, fricative, vowel;

#if defined(__cplusplus)
}
#endif   //(__cplusplus)

#endif   // __ALLOPHONE__
