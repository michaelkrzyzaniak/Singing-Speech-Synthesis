#include "Allophone.h"
#include "Util.h"

void         alloComputeNoisiness(Allophone* self, MKAiff* monoAiff);
fftSample_t* alloAllocateSpectralEnvelope(Allophone* self, MKAiff* monoAiff, FFT* fft);
float*       alloAllocateAmplitudeEnvelope(Allophone* self, MKAiff* monoAiff);
void         alloHilbertTransform(float* in, float* out, int numSamples);
void         alloComputeAverageAmplitude (Allophone* self);
void         alloComputePhonemeCategory  (Allophone* self);

/*OpaqueAllophoneStruct--------------------------------------*/
struct OpaqueAllophoneStruct
{
  char*  symbol;
  float* amplitudeEnv;
  int    amplitudeSize;
  float* spectralEnv;
  int    spectralSize;
  float  noisiness;
  
  float averageAmplitude;
  alloPhonemeCategory_t category;
};

/*alloNew----------------------------------------------------*/
Allophone* alloNew(alloSymbol_t symbol, MKAiff* monoAiff, FFT* fft)
{  
  Allophone* self = (Allophone*) calloc(1, sizeof(*self));
  if(self != NULL)
    {
      int len = strlen(symbol) + 1;
      self->symbol = malloc(len);
      if(self->symbol == NULL) 
        return alloDestroy(self);
      memcpy(self->symbol, symbol, len);
      
      //assignment is unnecessary but it looks pretty and helps with error handling
      self->spectralEnv = alloAllocateSpectralEnvelope (self, monoAiff, fft);
      if(self->spectralEnv == NULL) return alloDestroy(self);
      
      self->amplitudeEnv = alloAllocateAmplitudeEnvelope(self, monoAiff);
      if(self->amplitudeEnv == NULL) return alloDestroy(self);
      
      alloComputeAverageAmplitude(self);
      
      alloComputeNoisiness(self, monoAiff);
      
      alloComputePhonemeCategory(self);
    }
  return self;
}

/*alloDestroy------------------------------------------------*/
Allophone* alloDestroy(Allophone* self)
{
  if(self != NULL)
    {
      if(self->symbol != NULL)
        free(self->symbol);
      if(self->amplitudeEnv != NULL)
        free(self->amplitudeEnv);
      if(self->spectralEnv != NULL)
        free(self->spectralEnv);
      free(self);
    }
  return (Allophone*)NULL;
}

/*alloSymbol------------------------------------------------*/
alloSymbol_t alloSymbol(Allophone* self)
{
  return self->symbol;
}

/*computeNoisiness------------------------------------------*/
void alloComputeNoisiness(Allophone* self, MKAiff* monoAiff)
{
  unsigned int numZeroCrossings = 0;
  BOOL isPositive, firstTime = YES;
  int bufferSize = 100;
  float buffer[bufferSize], *b;
  double noisiness;
  int numSamplesRead;
  aiffRewindPlayheadToBeginning(monoAiff);
  while((numSamplesRead = aiffReadFloatingPointSamplesAtPlayhead(monoAiff, buffer, bufferSize, aiffYes)) > 0)
    {
      if(firstTime) 
        isPositive = (buffer[0] > 0);
      
      b = buffer;
      while(numSamplesRead--)
        if((*b++ > 0) != isPositive)
          {
            isPositive = !isPositive;
            numZeroCrossings++;
          }
    }
    
  noisiness = (long double)numZeroCrossings / (long double)aiffDurationInSamples(monoAiff);
  self->noisiness = (float)noisiness;
}

/*computeSpectralEnvelope----------------------------------*/
fftSample_t* alloAllocateSpectralEnvelope(Allophone* self, MKAiff* monoAiff, FFT* fft)
{
  //keep extra space at end to make fft happy
  self->spectralEnv = (float*) calloc(fftN(fft) >> 1, sizeof(self->spectralEnv[0]));
  if(self->spectralEnv != NULL)
    {        
      self->spectralSize = fftNumSamples(fft);
        
      int numSamplesRead, numFFTsTaken=0, i;
      float buffer[self->spectralSize];
      fftSample_t *magnitudes;
      aiffRewindPlayheadToBeginning(monoAiff);
  
      while((numSamplesRead = aiffReadFloatingPointSamplesAtPlayhead(monoAiff, buffer, self->spectralSize, aiffYes)) > 0)
        {
          numFFTsTaken++;
          fftCepstrum (fft, buffer, NULL, numSamplesRead, &magnitudes, NULL, NO, 0.5);
          //fftTransform(fft, buffer, NULL, numSamplesRead, &magnitudes, NULL, NO, NO, YES);
          for(i=0; i<self->spectralSize >> 1; i++)
            self->spectralEnv[i] += magnitudes[i];
        }
    
      scaleb(self->spectralEnv, fftN(fft) >> 1, 0, 1);
      //for(i=0; i<self->spectralSize; i++)
        //self->spectralEnv[i] /= (float)numFFTsTaken;
      fftFlush(fft); //be polite;
      
      //MKAiff* testAiff = aiffWithDurationInFrames(1, aiffSampleRate(monoAiff), aiffBitsPerSample(monoAiff), fftN(fft));
      //aiffAppendFloatingPointSamples(testAiff, self->spectralEnv, fftN(fft), aiffFloatSampleType);
      //aiffSaveWithFilename(testAiff, /*alloSymbol(self)*/ "___test.aiff");
      //aiffDestroy(testAiff);
    }
  
  return self->spectralEnv;
}

/*computeAmplitudeEnvelope---------------------------------*/
float* alloAllocateAmplitudeEnvelope(Allophone* self, MKAiff* monoAiff)
{
  int i;
  float *a, *b, *hilbert, max = 0, min = 10000;
  self->amplitudeSize = aiffDurationInFrames(monoAiff);
  self->amplitudeEnv = (float*) calloc(self->amplitudeSize, sizeof(self->amplitudeEnv[0]));
  if(self->amplitudeEnv != NULL)
    {
      hilbert = (float*) calloc(self->amplitudeSize, sizeof(self->amplitudeEnv[0]));
      if(hilbert != NULL)
        {
          aiffRewindPlayheadToBeginning(monoAiff);
          aiffReadFloatingPointSamplesAtPlayhead(monoAiff, self->amplitudeEnv, self->amplitudeSize, aiffYes);
          alloHilbertTransform(self->amplitudeEnv, hilbert, self->amplitudeSize);
  
          a = self->amplitudeEnv, b = hilbert;
          i = self->amplitudeSize;
          while(i-- > 0)
            {
              *a = fabs(*a) + fabs(*b);
              a++; b++;
            }

          i = self->amplitudeSize - 1;
          a = self->amplitudeEnv + 1;
          while(i-- > 0)
            {  
              *a = (/* 0.005 * */ *a) + (0.995 * a[-1]);
              if(*a > max) max = *a;
              if(*a < min) min = *a;
              a++;
            }
          if(max >= 1) //dont scale if the file is silent
            {          //that will make the amp env everywhere 1
              a = self->amplitudeEnv;
              i = self->amplitudeSize;
              while(i-- > 0)
                {
                  *a = scalef(*a, min, max, 0, 1);
                  a++;
                }
            }
          /*
          MKAiff* testAiff = aiffWithDurationInFrames(aiffNumChannels(monoAiff), aiffSampleRate(monoAiff), aiffBitsPerSample(monoAiff), aiffDurationInFrames(monoAiff));
          aiffAppendFloatingPointSamples(testAiff, self->amplitudeEnv, self->amplitudeSize, aiffFloatSampleType);
          aiffSaveWithFilename(testAiff, alloSymbol(self));
          aiffDestroy(testAiff);
          */
          free(hilbert);
        }
    }
    return self->amplitudeEnv;
}

/*---------------------------------------------------------*/
void alloHilbertTransform(float* in, float* out, int numSamples)
{
   /* Adapted from Joe Mietus */
    int i, l;
    float yt;
    int numCoeffs       = 128; //must be even
    int numCoeffsOver2  = numCoeffs >> 1;
    double coeffs[numCoeffs+1];
    
    /* generate filter coeffecients */
    //this sucks doing it here, I know...
    //these are constants, but there is no better place
    //for this at the moment... mkae global with isInitialized flag later;
    for (i=1; i<=numCoeffs; i++)
      coeffs[i] = 1 / ((i - numCoeffsOver2) - 0.5) / M_PI;

    for (l=1; l<=numSamples-numCoeffs+1; l++) 
      {
        yt = 0.0;
        for (i=1; i<=numCoeffs; i++) 
            yt = yt + in[l+i-1] * coeffs[numCoeffs+1-i];
        out[l] = yt;
      }

    /* shifting lfilt/1+1/2 points */
    for (i=1; i<=numSamples-numCoeffs; i++) 
        out[i] = 0.5 * (out[i] + out[i+1]);
        
    for (i=numSamples-numCoeffs; i>=1; i--)
        out[i + numCoeffsOver2] = out[i];

    /* writing zeros */
    for (i=1; i<=numCoeffsOver2; i++) 
      {
        out[i]              = 0.0;
        out[numSamples-i] = 0.0;
      }
}

/*---------------------------------------------------------*/
void alloComputeAverageAmplitude(Allophone* self)
{
  int n = self->amplitudeSize;
  float* p = self->amplitudeEnv;
  long double avg = 0;
  
  while(n-- > 0)
    avg += *p++;
  
  avg /= (long double) self->amplitudeSize;
  self->averageAmplitude = avg;
}

/*---------------------------------------------------------*/
fftSample_t* alloSpectralEnvelope      (Allophone* self, int* numSamples)
{
  if(numSamples != NULL)
    *numSamples = self->spectralSize;
  return self->spectralEnv;
}

/*---------------------------------------------------------*/
float        alloNoisiness             (Allophone* self)
{
  return self->noisiness;
}

/*---------------------------------------------------------*/
float*       alloAmplitudeEnvelope     (Allophone* self, int* numSamples)
{
  if(numSamples != NULL)
    *numSamples = self->amplitudeSize;
  return self->amplitudeEnv;
}

/*---------------------------------------------------------*/
alloPhonemeCategory_t alloPhonemeCategory(Allophone* self)
{
  return self->category;
}

/*---------------------------------------------------------*/
void alloComputePhonemeCategory(Allophone* self)
{
  alloPhonemeCategory_t result = alloUnknown;
  
  if(self->averageAmplitude < 0.5)
    result = alloPlosive;
  else
    {
      if(self->noisiness > 0.2)
        result = alloFricative;
      else
        result = alloVowel;
    }
  
  self->category = result;
}

/*---------------------------------------------------------*/
float        alloAverageAmplitude      (Allophone* self)
{
  return self->averageAmplitude;
}




