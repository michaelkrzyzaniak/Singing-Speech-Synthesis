#ifndef __FFT__
#define __FFT__ 1

#if defined(__cplusplus)
extern "C"{
#endif   //(__cplusplus)

#include "Util.h"

typedef struct OpaqueFFTStruct FFT;

typedef float fftSample_t;
typedef void (*fftWindowFunct_t)(float*, int);
extern fftWindowFunct_t fftBlackmanWindow, fftRectWindow, fftHannWindow, fftHammingWindow;

FFT*     fftNew                    (int           numSamples); //may be arbitrary
FFT*     fftDestroy                (FFT*          self)             ;
void     fftTransform              (FFT*          self              ,
                                    fftSample_t*  realIn            ,
                                    fftSample_t*  imagIn            ,
                                    int           numInSamples      ,
                                    fftSample_t** realOut           ,
                                    fftSample_t** imagOut           ,
                                    BOOL          inverse           ,
                                    BOOL          bitReversedOutput ,
                                    BOOL          polarOutput      );
void     fftCepstrum               (FFT*          self              ,
                                    fftSample_t*  realIn            ,
                                    fftSample_t*  imagIn            ,
                                    int           numInSamples      ,
                                    fftSample_t** realOut           ,
                                    fftSample_t** imagOut           ,
                                    BOOL          bitReversedOutput ,
                                    float         coefficient      );
void     fftFilter                 (FFT*          self              ,
                                    fftSample_t*  realIn            ,
                                    fftSample_t*  imagIn            ,
                                    int           numInSamples      ,
                                    fftSample_t** realOut           ,
                                    fftSample_t** imagOut           ,
                                    fftSample_t*  impulseResponse   ,
                                    int           numImpulseSamples ,
                                    float         brightness       );
int      fftHighestBin             (FFT*          self);
void     fftFrequencyOfBin         (FFT*          self, int bin, double sampleRate);
unsigned fftN                      (FFT*          self);
unsigned fftNumSamples             (FFT*          self);
void     fftSetOverlap             (FFT*          self, float overlap); /* 0 ~ 0.99 */
float    fftOverlap                (FFT*          self);
void     fftFlush                  (FFT*          self);
void     fftSetWindowFunct         (FFT*          self, fftWindowFunct_t windowFunct, BOOL analysis); //NO sets the resythesis window
fftWindowFunct_t fftWindowFunction (FFT*          self, BOOL analysis);

#if defined(__cplusplus)
}
#endif   //(__cplusplus)

#endif   // __FFT__
