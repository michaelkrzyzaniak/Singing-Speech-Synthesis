#include "FFT.h"
#include <math.h>
#include "fastsin.h"

void     fftRadix2DIT              (FFT* self, int isInverse);
void     fftApplyWindow            (FFT* self, BOOL analysis);
void     fftShiftSlidingOutput     (FFT* self);
void     fftSetAndShiftSlidingInput(FFT* self, fftSample_t* real, fftSample_t* imag /*or NULL*/, int numValidSamples);
void     fftBitReverseIndices      (FFT* self);
void     fftRectangularToPolar     (FFT* self);
void     fftScaleMagnitudes        (FFT* self, fftSample_t min, fftSample_t max); //assumes polar coordinates
unsigned fftSmallestPowerOfTwoGreaterThanOrEqualTo(unsigned n);

/*OpaqueFFTStruct-----------------------------------------------*/
struct OpaqueFFTStruct
{
  unsigned int     numSamples; //may be arbitrary
  unsigned int     N;          //lowest power of 2 greater or equal to numSamples
  unsigned int     overlap;    //numSamples
  fftSample_t*     realSlidingInput;
  fftSample_t*     imagSlidingInput;
  fftSample_t*     realSlidingOutput;
  fftSample_t*     imagSlidingOutput;
  fftSample_t*     real;
  fftSample_t*     imag;
  float*           analysisWindowBuffer;
  fftWindowFunct_t analysisWindowFunct;
  float*           synthesisWindowBuffer;
  fftWindowFunct_t synthesisWindowFunct;
};


/*fftNew--------------------------------------------------------*/
FFT* fftNew(int numSamples)
{
  FFT* self = (FFT*) calloc(sizeof(*self), 1);
  if(self != NULL)
    {
      self->numSamples = numSamples;
      self->N = fftSmallestPowerOfTwoGreaterThanOrEqualTo(numSamples);
      self->realSlidingInput      = (fftSample_t*) calloc(self->numSamples * 2, sizeof(self->realSlidingInput [0]));
      if(self->realSlidingInput     == NULL) return fftDestroy(self);
      self->imagSlidingInput      = (fftSample_t*) calloc(self->numSamples * 2, sizeof(self->imagSlidingInput [0]));
      if(self->imagSlidingInput     == NULL) return fftDestroy(self);
      self->realSlidingOutput     = (fftSample_t*) calloc(self->numSamples * 2, sizeof(self->realSlidingOutput[0]));
      if(self->realSlidingOutput    == NULL) return fftDestroy(self);
      self->imagSlidingOutput     = (fftSample_t*) calloc(self->numSamples * 2, sizeof(self->imagSlidingOutput[0]));
      if(self->imagSlidingOutput    == NULL) return fftDestroy(self);
      self->real                  = (fftSample_t*) calloc(self->N, sizeof(self->real  [0]));
      if(self->real                 == NULL) return fftDestroy(self);
      self->imag                  = (fftSample_t*) calloc(self->N, sizeof(self->imag  [0]));
      if(self->imag                 == NULL) return fftDestroy(self);
      self->analysisWindowBuffer  = (float*) calloc(self->N, sizeof(self->analysisWindowBuffer[0]));
      if(self->analysisWindowBuffer == NULL) return fftDestroy(self);
      self->synthesisWindowBuffer = (float*) calloc(self->N, sizeof(self->synthesisWindowBuffer[0]));
      if(self->synthesisWindowBuffer == NULL) return fftDestroy(self);
            
      fftSetWindowFunct (self, fftHannWindow, YES);  //analysis
      fftSetWindowFunct (self, fftHannWindow, NO ); //resynthesis
      fftSetOverlap(self, 3.0/4.0);
    }
  return self;
}

/*fftDestroy----------------------------------------------------*/
FFT* fftDestroy(FFT* self)
{
  if(self != NULL)
    {
      if(self->real != NULL)
        free(self->real);
      if(self->imag != NULL)
        free(self->imag);

      if(self->realSlidingInput != NULL)
        free(self->realSlidingInput);
      if(self->imagSlidingInput != NULL)
        free(self->imagSlidingInput);
        
      if(self->realSlidingOutput != NULL)
        free(self->realSlidingOutput);
      if(self->imagSlidingOutput != NULL)
        free(self->imagSlidingOutput);

      if(self->analysisWindowBuffer != NULL)
        free(self->analysisWindowBuffer);
      if(self->synthesisWindowBuffer != NULL)
        free(self->synthesisWindowBuffer);
        
      free(self);
    }
  return (FFT*) NULL;
}

/*-----------------------------------------------------------------------*/
/*
void     fftTransform(FFT* self, fftSample_t*  realIn_arg , fftSample_t*   imagIn_arg, int numInSamples, 
                                 fftSample_t** realOut_arg, fftSample_t**  imagOut_arg, 
                                 BOOL inverse, BOOL bitReversedOutput, BOOL polarOutput)
{
  fftSample_t *realIn  = self->realSlidingInput , *imagIn  = self->imagSlidingInput ;
  fftSample_t *realOut = self->realSlidingOutput, *imagOut = self->imagSlidingOutput;
  fftSample_t *real, *imag;
  int overlap = self->overlap, numFFTs = self->numSamples / overlap, n;
  int N = self->N;
  
  fftSetAndShiftSlidingInput(self, realIn_arg, imagIn_arg, numInSamples);
  fftShiftSlidingOutput     (self);
  
  while(numFFTs-- > 0)
    {
      n = self->numSamples;
      real = self->real;
      imag = self->imag;
      realIn += overlap;
      imagIn += overlap;
      memcpy(real, realIn, n * sizeof(fftSample_t));
      memcpy(imag, imagIn, n * sizeof(fftSample_t));
      real += n;
      imag += n;
      while(n++ < N)
        *real++ = *imag++ = 0;
      
      fftApplyWindow(self, YES);
      if(!bitReversedOutput)
        fftBitReverseIndices(self);
      fftRadix2DIT(self, 0);      
      
      real = self->real;
      imag = self->imag;
      realOut = self->realSlidingOutput,
      imagOut = self->imagSlidingOutput;
      n = self->numSamples;
      while(n-- > 0)
        {
          *realOut++ += *real++;
          *imagOut++ += *imag++;
        }
    }
    
  if(polarOutput)
    fftRectangularToPolar(self);
  fftScaleMagnitudes(self, 0, 1);
  
  if(realOut_arg != NULL)
    *realOut_arg = self->realSlidingOutput;
  if(imagOut_arg != NULL)
    *imagOut_arg = self->imagSlidingOutput;
}
*/
void     fftTransform(FFT* self, fftSample_t*  realIn, fftSample_t*   imagIn, int numInSamples, 
                                 fftSample_t** realOut, fftSample_t**  imagOut, 
                                 BOOL inverse, BOOL bitReversedOutput, BOOL polarOutput)
{
  fftSetAndShiftSlidingInput(self, realIn, imagIn, numInSamples);
  memcpy(self->real, self->realSlidingInput + self->numSamples, self->numSamples * sizeof(fftSample_t));
  memcpy(self->imag, self->imagSlidingInput + self->numSamples, self->numSamples * sizeof(fftSample_t));
  fftApplyWindow(self, YES);
  if(!bitReversedOutput)
    fftBitReverseIndices(self);
  fftRadix2DIT(self, inverse);
  if(polarOutput)
    fftRectangularToPolar(self);
  fftScaleMagnitudes(self, 0, 1);
  memcpy(self->realSlidingOutput, self->real, self->N * sizeof(fftSample_t));
  memcpy(self->imagSlidingOutput, self->imag, self->N * sizeof(fftSample_t));
  if(realOut != NULL)
    *realOut = self->realSlidingOutput;
  if(imagOut != NULL)
    *imagOut = self->imagSlidingOutput;
}

/*-----------------------------------------------------------------------*/
void     fftCepstrum               (FFT*          self              ,
                                    fftSample_t*  realIn            ,
                                    fftSample_t*  imagIn            ,
                                    int           numInSamples      ,
                                    fftSample_t** realOut           ,
                                    fftSample_t** imagOut           ,
                                    BOOL          bitReversedOutput ,
                                    float         coefficient      )
{
  CONSTRAIN(coefficient, 0.0, 1.0);
  int N = fftN(self);
  int cutoff;
  fftSample_t *r, *s, *i, *j;
  fftSetAndShiftSlidingInput(self, realIn, imagIn, numInSamples);
  memcpy(self->real, self->realSlidingInput + self->numSamples, self->numSamples * sizeof(fftSample_t));
  memcpy(self->imag, self->imagSlidingInput + self->numSamples, self->numSamples * sizeof(fftSample_t));
  fftApplyWindow(self, YES);
  //if(!bitReversedOutput) //reversing them here causes normal order output
  fftBitReverseIndices(self);
  fftRadix2DIT(self, NO);
  fftRectangularToPolar(self);
  //fftScaleMagnitudes(self, 0, 1);

  N = fftN(self);  
  r = self->real;
  while(N-- > 0)
    {
      *r = log10(*r + 1);
      r++;
    }

  fftSetAndShiftSlidingInput(self, self->real, NULL, self->N);
  memcpy(self->real, self->realSlidingInput + self->numSamples, self->numSamples * sizeof(fftSample_t));
  memcpy(self->imag, self->imagSlidingInput + self->numSamples, self->numSamples * sizeof(fftSample_t));
  fftBitReverseIndices(self);
  fftRadix2DIT(self, YES);

  
  N = fftN(self);
  cutoff = (1-coefficient) * N / 2;
  r = self->real + cutoff;
  s = self->real + N - (1 + cutoff);
  i = self->imag + cutoff;
  j = self->imag + N - (1 + cutoff);  
  N = N/2 - cutoff;
  while(N-- > 0)
    *r++ = *s-- = *i++ = *j-- = 0;

  fftBitReverseIndices(self);
  fftRadix2DIT(self, YES);  
  fftRectangularToPolar(self);
  fftScaleMagnitudes(self, 0, 1.0);
  
  memcpy(self->realSlidingOutput, self->real, self->N * sizeof(fftSample_t));
  memcpy(self->imagSlidingOutput, self->imag, self->N * sizeof(fftSample_t));
  if(realOut != NULL)
    *realOut = self->realSlidingOutput;
  if(imagOut != NULL)
    *imagOut = self->imagSlidingOutput;
}

/*-----------------------------------------------------------------------*/
void     fftFilter(FFT* self, fftSample_t*  realIn_arg , fftSample_t* imagIn_arg, int numInSamples, 
                              fftSample_t** realOut_arg, fftSample_t** imagOut_arg, 
                              fftSample_t* impulseResponse, int numImpulseSamples, float brightness)
{
  CONSTRAIN(numImpulseSamples, 0, self->N);
  
  fftSample_t *realIn  = self->realSlidingInput , *imagIn  = self->imagSlidingInput ;
  fftSample_t *realOut = self->realSlidingOutput, *imagOut = self->imagSlidingOutput, *r, *i;
  fftSample_t *real, *imag, *realBackwards, *imagBackwards;
  fftSample_t* spectrum;
  int overlap = self->overlap, numFFTs = self->numSamples / overlap, n;
  int N = self->N, j;
  fftSample_t nextBrightness;
  
  brightness /= (N / 16);
  
  fftSetAndShiftSlidingInput(self, realIn_arg, imagIn_arg, numInSamples);
  fftShiftSlidingOutput     (self);
  
  while(numFFTs-- > 0)
    {
      n = self->numSamples;
      real = self->real;
      imag = self->imag;
      realIn += overlap;
      imagIn += overlap;
      memcpy(real, realIn, n * sizeof(fftSample_t));
      memcpy(imag, imagIn, n * sizeof(fftSample_t));
      real += n;
      imag += n;
      while(n++ < N)
        *real++ = *imag++ = 0;
      
      fftApplyWindow(self, YES);
      fftBitReverseIndices(self);
      fftRadix2DIT(self, 0);      

      real = self->real;
      imag = self->imag;
      spectrum = impulseResponse;
      
      
      realBackwards = real + self->numSamples - 1;
      imagBackwards = imag + self->numSamples - 1;
      n = numImpulseSamples / 2;
      for(j=0; j<n; j++)//while(n-- > 0)
        {
          nextBrightness    = brightness * j + 1;
          if(nextBrightness < 0) nextBrightness = 0;
          nextBrightness   *= *spectrum;
          *real++          *= nextBrightness;
          *imag++          *= nextBrightness;
          *realBackwards-- *= nextBrightness;
          *imagBackwards-- *= nextBrightness;
          spectrum++;
        }
      n = (N - numImpulseSamples) / 2;
      while(n-- > 0)
        *real++ = *imag++ = *realBackwards-- = *imagBackwards-- = 0;

      fftBitReverseIndices  (self);
      fftRadix2DIT          (self, 1);
      fftApplyWindow        (self, NO);
      
      real = self->real;
      imag = self->imag;
      r = realOut;
      i = imagOut;
      n = self->numSamples;
      while(n-- > 0)
        {
          *r++ += *real++;
          *i++ += *imag++;
        }
      realOut += overlap;
      imagOut += overlap;
    }
  if(realOut_arg != NULL)
    *realOut_arg = self->realSlidingOutput;
  if(imagOut_arg != NULL)
    *imagOut_arg = self->imagSlidingOutput;
}

/*-----------------------------------------------------------------------*/
void     fftSetAndShiftSlidingInput(FFT* self, fftSample_t* real, fftSample_t* imag /*or NULL*/, int numValidSamples)
{
  CONSTRAIN(numValidSamples, 0, self->numSamples);
  
  int n;
  fftSample_t *source, *dest;
  
  n = self->numSamples * sizeof(fftSample_t);
  dest   = self->realSlidingInput;
  source = dest + self->numSamples;
  memcpy(dest, source, n);
  
  n = numValidSamples * sizeof(fftSample_t);
  dest = source;
  source = real;
  memcpy(dest, source, n);
  n = numValidSamples;
  dest += n;
  while(n++ < self->numSamples)
    *dest++ = 0;

  n = self->numSamples * sizeof(fftSample_t);
  dest   = self->imagSlidingInput;
  source = dest + self->numSamples;
  memcpy(dest, source, n);

  dest = source;
  if(imag != NULL)
    {  
      n = numValidSamples * sizeof(fftSample_t);
      source = imag;
      memcpy(dest, source, n);
      n = numValidSamples;
      dest += n;  
    }
  else n = 0;
    
  while(n++ < self->numSamples)
    *dest++ = 0;
}

/*-----------------------------------------------------------------------*/
void fftShiftSlidingOutput(FFT* self)
{
  int n = self->numSamples;
  fftSample_t *r = self->realSlidingOutput;
  fftSample_t *i = self->imagSlidingOutput;
  memcpy(r, r+n, n * sizeof(fftSample_t));
  memcpy(i, i+n, n * sizeof(fftSample_t));
  r += n;
  i += n;
  while(n-- > 0)
    *r++ = *i++ = 0;
}

/*-----------------------------------------------------------------------*/
void   fftFlush                 (FFT* self)
{
  int n = self->numSamples;
  fftSample_t *a, *b, *c, *d;
  //don't worry about the lower half of the buffers, 
  //they will be flushed on the next call to fftSetData()
  a = self->realSlidingInput  + n;
  b = self->imagSlidingInput  + n;
  c = self->realSlidingOutput + n;
  d = self->imagSlidingOutput + n;
  
  while(n-- > 0)
    *a++ = *b++ = *c++ = *d++ = 0;
}

/*-----------------------------------------------------------------------*/
void fftSetWindowFunct (FFT* self, fftWindowFunct_t windowFunct, BOOL analysis)
{
  int i;
  float* buffer           = (analysis) ? self->analysisWindowBuffer   : self->synthesisWindowBuffer;
  fftWindowFunct_t* funct = (analysis) ? &(self->analysisWindowFunct) : &(self->synthesisWindowFunct);
  *funct = windowFunct;
    
  if(windowFunct != NULL)
    {
      (*funct)(buffer, self->numSamples);
      for(i=self->numSamples; i<self->N; i++)
        buffer[i] = 0;
    }
}

/*-----------------------------------------------------------------------*/
fftWindowFunct_t fftWindowFunction(FFT* self, BOOL analysis)
{
  return (analysis) ? self->analysisWindowFunct : self->synthesisWindowFunct;
}

/*-----------------------------------------------------------------------*/
void fftRadix2DIT(FFT* self, int isInverse)
{
  int N = self->N;
  fftSample_t *real = self->real, *imag = self->imag;
  //double twoPiOverN = TWO_PI / N, omegaTwoPiOverN;
  uint32_t twoPiOverN = SIN_TWO_PI / N, omegaTwoPiOverN;
  int subTransform, butterfly;
  
  int numSubTransforms = N, numButterflies = 1, omega;
  fftSample_t wr, wi, *r, *i, tempr, tempi;
  
  while((numSubTransforms >>= 1) > 0)
    {
      r = real, i = imag;
      for(subTransform=0; subTransform<numSubTransforms; subTransform++)
        {
          omega = 0;
          for(butterfly=0; butterfly<numButterflies; butterfly++)
            {
              omegaTwoPiOverN = omega * twoPiOverN;
              //tempr =  cos(omegaTwoPiOverN); 
              //tempi = -sin(omegaTwoPiOverN);
              tempr   =  fastcos(omegaTwoPiOverN);
              tempi   = -fastsin(omegaTwoPiOverN);
              if(isInverse)
                tempi = -tempi;
              wr = (r[numButterflies] * tempr) - (i[numButterflies] * tempi);
              wi = (r[numButterflies] * tempi) + (i[numButterflies] * tempr);
              tempr = *r; tempi = *i;
              *r += wr;
              *i += wi;
              r[numButterflies] = tempr - wr;
              i[numButterflies] = tempi - wi;              
              omega += numSubTransforms;
              r++, i++;
            }
            r += numButterflies, i += numButterflies;
        }
      numButterflies <<= 1;
    }
    omega = N; //just reuse some damn variable
    if(isInverse)
      while(omega-- > 0)
        {
          *real++ /= N;
          *imag++ /= N;
        }
}

/*-----------------------------------------------------------------------*/
void fftBitReverseIndices(FFT* self)
{
  fftSample_t *real = self->real, *imag = self->imag;
  int N = self->N;
  int NMinus1 = N-1;
  int highestBit = N >> 1;
  int n, nextBit, nReversed=0;
  fftSample_t temp;
  
  for(n=1; n<N; n++)
    {
      nextBit = highestBit;
      while((nextBit + nReversed) > NMinus1) nextBit >>= 1;  //find highest unpopulated bit
      nReversed &= nextBit - 1;                              //clear all higher bits
      nReversed |= nextBit;                                  //set new bit
      
      if(nReversed > n)
        {
          temp            = real[n]        ;
          real[n]         = real[nReversed];
          real[nReversed] = temp           ;
          temp            = imag[n]        ;
          imag[n]         = imag[nReversed];
          imag[nReversed] = temp           ;          
        }
    }
}

/*-----------------------------------------------------------------------*/
void fftRectangularToPolar(FFT* self)
{
  int i;
  fftSample_t temp;
  unsigned N = self->N;
  fftSample_t* real = self->real;
  fftSample_t* imag = self->imag;
  
  for(i=0; i<N; i++)
    {
      temp = *real;
      *real = hypot(*real, *imag);
      *imag = atan2(*imag, temp);
      real++, imag++;    
    }
}

/*-----------------------------------------------------------------------*/
void fftScaleMagnitudes(FFT* self, fftSample_t min, fftSample_t max)
{
  fftSample_t minSample = 1000000/*FLT_MAX*/, maxSample = 0;
  fftSample_t* r = self->real; 
  unsigned N = self->N;
  
  
  while(N--)
    {
      if(*r < minSample)
        minSample = *r;
      if(*r > maxSample)
        maxSample = *r;
      r++;
    }
    
  N = self->N;
  r = self->real;
  
  while(N-- > 0)
    {
      *r = scalef(*r, minSample, maxSample, min, max);
      r++;
    }
}

/*-----------------------------------------------------------------------*/
void   fftSetOverlap            (FFT* self, float overlap)
{
  unsigned temp = self->numSamples * overlap;
  temp = self->numSamples - temp;
  CONSTRAIN(temp, 1, self->numSamples);
  self->overlap = temp;
}

/*-----------------------------------------------------------------------*/
float  fftOverlap               (FFT* self)
{
  //is this even right?
  return (self->N - self->overlap) / self->numSamples;
}

/*-----------------------------------------------------------------------*/
void fftApplyWindow(FFT* self, BOOL analysis)
{
  fftWindowFunct_t funct = (analysis) ? self->analysisWindowFunct : self->synthesisWindowFunct;
  if(funct != NULL)
    {
      fftSample_t *real = self->real, *imag = self->imag;
      float* windowBuffer = (analysis) ? self->analysisWindowBuffer : self->synthesisWindowBuffer;
      unsigned N = self->N;
  
      while(N-- > 0)
        {
          *real++ *= *windowBuffer;
          *imag++ *= *windowBuffer;
          windowBuffer++;
        }
    }
}

/*-----------------------------------------------------------------------*/
/*
void _fftRectWindow(float* windowBuffer, int bufferSize)
{
  int i;
  for(i=0; i<bufferSize; i++)
    *windowBuffer++ = 1;
}
*/
fftWindowFunct_t fftRectWindow = NULL;

/*-----------------------------------------------------------------------*/
void _fftHannWindow(float* windowBuffer, int bufferSize)
{
  int i;
  long double phase = 0;
  long double phaseIncrement = TWO_PI / (long double)bufferSize;
  for(i=0; i<bufferSize; i++)
    {
      *windowBuffer++ = 0.5 * (1-cos(phase));
      phase += phaseIncrement;
    }  
}
fftWindowFunct_t fftHannWindow = _fftHannWindow;

/*-----------------------------------------------------------------------*/
void _fftHammingWindow(float* windowBuffer, int bufferSize)
{
  int i;
  long double phase = 0;
  long double phaseIncrement = TWO_PI / (long double)bufferSize;
  for(i=0; i<bufferSize; i++)
    {
      *windowBuffer++ = 0.54 - 0.46 * cos(phase);
      phase += phaseIncrement;
    }  
}
fftWindowFunct_t fftHammingWindow = _fftHannWindow;

/*-----------------------------------------------------------------------*/
void _fftBlackmanWindow(float* windowBuffer, int bufferSize)
{
  int i;
  long double phase = 0;
  long double phaseIncrement = TWO_PI / (long double)bufferSize;
  double a = 0;
  double a0 = (1-a)/2.0;
  double a1 = 1/2.0;
  double a2 = a/2.0;
  
  for(i=0; i<bufferSize; i++)
    {
      *windowBuffer++ = a0 - a1*cos(phase) + a2*cos(2*phase);
      phase += phaseIncrement;
    }  
}
fftWindowFunct_t fftBlackmanWindow = _fftBlackmanWindow;

/*-----------------------------------------------------------------------*/
unsigned fftN(FFT* self)
{
  return self->N;
}

/*-----------------------------------------------------------------------*/
unsigned fftNumSamples          (FFT* self)
{
  return self->numSamples;
}

/*-----------------------------------------------------------------------*/
unsigned fftSmallestPowerOfTwoGreaterThanOrEqualTo(unsigned n)
{
  int result = 1;
  while(result < n)
    result <<= 1;
  return result;
}









