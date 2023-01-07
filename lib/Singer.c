/*
 *  Synth.h
 *  Make weird noises
 *
 *  Made by Michael Krzyzaniak at Arizona State University's
 *  School of Arts, Media + Engineering in Spring of 2013
 *  mkrzyzan@asu.edu
 */

#include "../Singer.h"
#include "FFT.h"
#include "fastsin.h"
#include "Util.h"

#define SING_RANDOM(min, max)  (((random() / (long double)RAND_MAX)) * ((max) - (min)) + (min))
#define SING_MIDI2CPS(x)   (440 * pow(2, ((x)-69) / 12.0))
#define SING_MIDI2FREQ(x)  (SING_MIDI2CPS(x) * SIN_TWO_PI / self->sampleRate)
#define SING_CPS2MIDI(x)   (69 + 12.0 * log2((x) / 440.0))
#define SING_FREQ2MIDI(x)  SING_CPS2MIDI(x) / (SIN_TWO_PI / self->sampleRate)

/*not perfect beause assignment will take a few clock cycles, but here*/
/*a case should not arise in which it was locked, unlocked, and locked again by the ohter thread in a few cycles */
#define SING_SPINLOCK(x)   while(x); (x)=1;
#define SING_SPINUNLOCK(x) ((x)=0)

void singAdvaceSynthVariables    (Singer* self);
void singConsonantTimerExpired   (Singer* self);
BOOL singInitWavetableSynthesizer(Singer* self, char* filename);
int  singStringifyAllophones     (Singer* self, alloSymbol_t allos);


/*OpaqueSingStruct------------------------------------------*/
struct OpaqueSingerStruct
{
  double            sampleRate;
  double            twoPiOverSampleRate;
  double            fourFortyOverSampleRate;
  
  FFT*              fft;
  Phonologist*      phonologist;
  List*             allophoneQueue; //holds allophone symbols only
  Allophone*        currentAllophone;
  
  fftSample_t*      targetSpectralEnvelope;
  fftSample_t*      actualSpectralEnvelope;
  float             spectralSmoothing, oneMinusSpectralSmoothing;
  float             origSpectralSmoothing; //sometimes the synth sets the smoothing to 0, so this is to restore it.
  
  int32_t           phase;      //signed to remove DC offset from sawtooth wave. Use unsigned for sine wave
  uint32_t          actualFreq, targetFreq, userFreq;
  float             freqDrift;
  float             freqSmoothing, oneMinusFreqSmoothing;
  float             targetAmp, actualAmp, userAmp;
  float             ampSmoothing, oneMinusAmpSmoothing;
  uint32_t          vibratoPhase;
  uint32_t          vibratoRate, userVibratoRate;
  float             actualVibratoDepth, targetVibratoDepth, userVibratoDepth;
  float             vibratoDepthDrift, vibratoRateDrift;
  float             vibratoDepthSmoothing, oneMinusVibratoDepthSmoothing;
  
  float             targetNoisiness, actualNoisiness; //as analyzed by Allophone
  float             breathiness;
  float             plosiveCrunchiness;
  //float             noisePresence;                    //overall
  
  float             noiseSmoothing, oneMinusNoiseSmoothing;
  float             consonantVolume, vowelVolume;
  int               consonantDuration;
  int               consonantTimer;
  int               shouldTriggerNextAllophone; //counts the number of queued vowels, incase a second is triggered before the first has arrived
  
  float*            wavetable;
  double            wavetableCurrentSample;
  int32_t           wavetableNumSamples;
  uint32_t          wavetableFundamental;
  
  float*            ampEnv;
  int               ampEnvRemaining;
  float             smoothedAmpEnv;
  BOOL              ampEnvIsLocked;
  
  uint32_t          roughnessPhase;  
  uint32_t          roughnessFreq;
  float             roughness, oneMinusRoughness;
  
  float             brightness;
  
  uint8_t           controlRateTimer;
  
  singEventCallback_t allophoneCallback, queueEmptyCallback;
  void *allophoneCallbackArg, *queueEmptyCallbackArg;

  char allophoneSeparatorChar, restChar, holdChar;
};

/*singNew---------------------------------------------------*/
Singer*  singNew(char* pathToWavetableAiff, char* pathToAllophoneAiffs, double sampleRate, int bufferNumSamples)
{
  Singer* self = calloc(sizeof(*self), 1);
  if(self != NULL)
    {
      self->sampleRate = sampleRate;
      self->twoPiOverSampleRate = SIN_TWO_PI / sampleRate;
      self->fourFortyOverSampleRate = 44100.0 / sampleRate;
    
      self->fft = fftNew(bufferNumSamples);
      if(self->fft == NULL) return singDestroy(self);

      self->phonologist = phonNew(pathToAllophoneAiffs, self->fft, sampleRate);
      if(self->phonologist == NULL) return singDestroy(self);

      self->allophoneQueue = listNew();
      if(self->allophoneQueue == NULL) return singDestroy(self); 

      self->actualSpectralEnvelope = (fftSample_t*) calloc(fftN(self->fft) >> 1, sizeof(self->actualSpectralEnvelope[0]));
      if(self->actualSpectralEnvelope == 0)  return singDestroy(self); 

      self->targetSpectralEnvelope = (fftSample_t*) calloc(fftN(self->fft) >> 1, sizeof(self->targetSpectralEnvelope[0]));
      if(self->targetSpectralEnvelope == 0)  return singDestroy(self); 

      if(singInitWavetableSynthesizer(self, pathToWavetableAiff) == NO) return singDestroy(self);

      singSetPitch              (self, 57, NO);
      singSetAllophoneGlideTime (self, 0.6/*0.92*/);
      singSetFreqGlideTime      (self, 0.999/*0.99925*/);
      singSetVibratoRate        (self, 6);
      singSetVibratoDepth       (self, 1);
      singSetVibratoOnsetTime   (self, 0.999992);
      singSetFreqDrift          (self, 0.0015);
      singSetVibratoRateDrift   (self, 0.05/*08*/);
      singSetVibratoDepthDrift  (self, 0.1);
      //singSetNoisePresence      (self, 2.5);
      singSetPlosiveCrunchiness (self, 2.5);
      singSetBreathiness        (self, 4);
      singSetNoiseSmoothing     (self, 0.0);
      singSetConsonantDuration  (self, 100);
      singSetRelativeVowelVolume(self, 0.5);
      singSetLoudness           (self, 1);
      singSetLoudnessGlideTime  (self, 0.999);
      singSetRoughnessFreq      (self, 40.0);
      singSetRoughness          (self, 0.1);
      singSetBrightness         (self, -0.2);
      singSetAllophoneCallback  (self, NULL, NULL);
      singSetQueueEmptyCallback (self, NULL, NULL);
      
      singSetAllophoneSeparatorChar(self, '|');
      singSetRestChar              (self, ' ');
      singSetHoldChar              (self, '-');
    }
  return self;
}

/*singDestroy---------------------------------------------------*/
Singer*  singDestroy                  (Singer* self)
{
  if(self != NULL)
    {
      if(self->fft != NULL)
        fftDestroy(self->fft);
      if(self->phonologist != NULL)
        phonDestroy(self->phonologist);
      if(self->allophoneQueue != NULL)
        listDestroy(self->allophoneQueue, YES);
      if(self->actualSpectralEnvelope != NULL)
        free(self->actualSpectralEnvelope);
      if(self->targetSpectralEnvelope != NULL)
        free(self->targetSpectralEnvelope); 
      if(self->wavetable != NULL)
        free(self->wavetable);
    }
  return (Singer*) NULL;
}

/*--------------------------------------------------------------*/
BOOL singInitWavetableSynthesizer(Singer* self, char* filename)
{
  BOOL success = NO;
  MKAiff* aiff = aiffWithContentsOfFile(filename);
  if(aiff != NULL)
    {
      aiffMakeMono(aiff);
      
      self->wavetableNumSamples  = aiffDurationInSamples(aiff);
      self->wavetableFundamental = (aiffHasInstrumentInfo(aiff)) ? SING_MIDI2FREQ(aiffInstrumentBaseNote(aiff)) 
                                                                 : (SIN_TWO_PI / (double)self->wavetableNumSamples);
      self->wavetable = malloc(self->wavetableNumSamples * sizeof(self->wavetable[0]));
      if(self->wavetable != NULL)
        success = (aiffReadFloatingPointSamplesAtPlayhead(aiff, self->wavetable, self->wavetableNumSamples, aiffYes) == self->wavetableNumSamples);

      aiff = aiffDestroy(aiff);
    }
    
  return success;
}

/*--------------------------------------------------------------*/ 
void   singSetAllophone             (Singer* self, alloSymbol_t symbol)
{
  int N = fftN(self->fft);
  fftSample_t *a, *b;
  BOOL shouldHold = NO;
  
  int len = strlen(symbol);
  if(len > 1)
    if(symbol[len-1] == self->holdChar)
      {
        symbol[len-1] = '\0';
        shouldHold = YES;
      }
  
  Allophone* allo = phonAllophoneForSymbol(self->phonologist, symbol);
      
  if(!shouldHold)
    self->consonantTimer = self->consonantDuration;
  else
    {
      if(self->shouldTriggerNextAllophone > 0)
        self->shouldTriggerNextAllophone --;//= NO;
      if(self->shouldTriggerNextAllophone)
        self->consonantTimer = self->consonantDuration;
      else
        self->consonantTimer = 0;//is this necessary?
    }

  if(allo != NULL)
    { 
        float noise;
        alloPhonemeCategory_t category = alloPhonemeCategory(allo);
        a = self->targetSpectralEnvelope;
        b = alloSpectralEnvelope(allo, NULL);
        memcpy(a, b, (N >> 1) * sizeof(fftSample_t));
        noise = (category == alloVowel) ? self->breathiness : self->plosiveCrunchiness;
        noise *= alloNoisiness(allo);
        if(noise > 1) noise = 1;
        self->targetNoisiness = noise;
        
        if((!shouldHold) /* && (category == alloPlosive) */)
          {
            //printf("cons: ");
            SING_SPINLOCK(self->ampEnvIsLocked);
            self->ampEnv = alloAmplitudeEnvelope(allo, &(self->ampEnvRemaining));
            SING_SPINUNLOCK(self->ampEnvIsLocked);
          }
        else
          {
            SING_SPINLOCK(self->ampEnvIsLocked);
            self->ampEnv = NULL; self->ampEnvRemaining = 0;
            SING_SPINUNLOCK(self->ampEnvIsLocked);          
          }
          
        if(self->allophoneCallback != NULL)
          self->allophoneCallback(self->allophoneCallbackArg, allo);
    }
  else //(allo == NULL)
    if(*symbol == self->restChar)
      singShutYerPieHole(self);
    
  self->currentAllophone = allo;
}

/*--------------------------------------------------------------*/
Allophone*     singCurrentAllophone             (Singer* self)
{
  return self->currentAllophone;
}

/*--------------------------------------------------------------*/ 
void   singEnqueueAllophones        (Singer* self, alloSymbol_t symbols)
{
  int len;
  alloSymbol_t s, nonConstantSymbols;
  
  len = strlen(symbols) + 1;
  nonConstantSymbols = malloc(len * sizeof(*nonConstantSymbols));
  
  if(nonConstantSymbols != NULL)
    {
      memcpy(nonConstantSymbols, symbols, len);
      symbols = nonConstantSymbols;
      
      int numAllophones = singStringifyAllophones(self, symbols);
      
      while(numAllophones-- > 0)
        {
          len = strlen(symbols) + 1;
          s = malloc(len * sizeof(*s));//the list will free this when it is removed
          if(s != NULL)
            {
              memcpy(s, symbols, len);
              listAppendData(self->allophoneQueue, s, (listDataDeallocator_t) free);
            }
          symbols += len;
        }
      free(nonConstantSymbols);
    }
}

/*--------------------------------------------------------------*/ 
int    singStringifyAllophones      (Singer* self, alloSymbol_t symbols)
{
  int numAllophones = 0;
  if(*symbols != '\0')
  {
    while (*symbols != '\0')
      {
        if(*symbols == self->allophoneSeparatorChar)
          {
            *symbols = '\0';
            numAllophones++;
          }
        symbols++;
      }
    if(symbols[-1] != self->allophoneSeparatorChar)
      numAllophones++;
  }
  
  return numAllophones;
}

/*--------------------------------------------------------------*/ 
void   singSetAllophoneGlideTime    (Singer* self, float coefficient)
{
  CONSTRAIN(coefficient, 0.0, 1.0);
  coefficient = pow(coefficient, self->fourFortyOverSampleRate);
  self->spectralSmoothing         =     coefficient;
  self->oneMinusSpectralSmoothing = 1 - coefficient;
  self->origSpectralSmoothing = coefficient;
}

/*--------------------------------------------------------------*/
float  singAllophoneGlideTime       (Singer* self)
{
  return pow(self->origSpectralSmoothing, 1.0/self->fourFortyOverSampleRate);
}
/*--------------------------------------------------------------*/ 
List*  singEnqueuedAllophones       (Singer* self)
{
  return self->allophoneQueue;
}

/*--------------------------------------------------------------*/ 
char** singSupportedAllophones   (Singer* self, int* returnedNumAllophones)
{
  int size = phonNumAllophones(self->phonologist);
  char** allos = malloc(size * sizeof(*allos));
  
  if(allos != NULL)
    size = phonAllophoneSymbols(self->phonologist, allos);
  else size = 0;
  
  *returnedNumAllophones = size;
  
  return allos; /*caller must free*/
}

/*--------------------------------------------------------------*/ 
void       singPrintSupportedAllophones (Singer* self)
{
  int    n;
  char** allos = singSupportedAllophones(self, &n);
  char** a = allos, *c;
  Allophone* allo;
  
  if(a != NULL)
    {
      while(n-- > 0)
        {
          c = *a;
          printf("%s:\t", *a++);
          allo = phonAllophoneForSymbol(self->phonologist, c);
          switch(alloPhonemeCategory(allo))
           {
             case alloUnknown  : printf("%s\t", "unknown  "); break;
             case alloVowel    : printf("%s\t", "vowel    "); break;
             case alloFricative: printf("%s\t", "fricative"); break;
             case alloPlosive  : printf("%s\t", "plosive  "); break; 
             default           : printf("%s\t", "error    "); break;      
           }     
          while(*c != '\0')
            printf("%02X ", (unsigned char)*c++);
          printf("\n\r");
        }
      free(allos);
    }
}

/*--------------------------------------------------------------*/ 
void   singTriggerNextAllophone     (Singer* self)
{
  if(listCount(self->allophoneQueue) > 0)
    {
      alloSymbol_t symbol = listDataAtIndex(self->allophoneQueue, 0);
      singSetAllophone(self, symbol);
      listRemoveDataAtIndex(self->allophoneQueue, 0, YES);
      
      if(self->queueEmptyCallback != NULL)
        if(listCount(self->allophoneQueue) == 0)
          self->queueEmptyCallback(self->queueEmptyCallbackArg, NULL);
    }
  else
    singShutYerPieHole(self);
}

/*--------------------------------------------------------------*/ 
void   singTriggerNextVowel         (Singer* self)
{
  if(self->shouldTriggerNextAllophone < listCount(self->allophoneQueue))
    self->shouldTriggerNextAllophone ++;//= YES;
    
  singTriggerNextAllophone(self);
}

/*--------------------------------------------------------------*/
void   singConsonantTimerExpired(Singer* self)
{
  if(self->shouldTriggerNextAllophone)
    singTriggerNextAllophone(self);
}

/*--------------------------------------------------------------*/ 
void   singSetConsonantDuration     (Singer* self, float millisecs    )
{
  self->consonantDuration = (int) (millisecs * (self->sampleRate / 1000.0));
}

/*--------------------------------------------------------------*/ 
float  singConsonantDuration        (Singer* self                   )
{
  return 1000 * (self->consonantDuration / (double) self->sampleRate);
}

/*--------------------------------------------------------------*/ 
void   singSetFreq                  (Singer* self, float cps, BOOL shouldTriggerVibrato)
{
  self->targetFreq = self->userFreq = (cps * self->twoPiOverSampleRate);
  if(shouldTriggerVibrato)
    singTriggerVibrato(self);
}

/*--------------------------------------------------------------*/
float  singFreq                     (Singer* self               )
{
  return (double)self->userFreq / (double)self->twoPiOverSampleRate;
}
/*--------------------------------------------------------------*/
void   singSetPitch                 (Singer* self, float midiNumber, BOOL shouldTriggerVibrato)
{
  singSetFreq(self, SING_MIDI2CPS(midiNumber), shouldTriggerVibrato);
}

/*--------------------------------------------------------------*/
float  singPitch                    (Singer* self)
{
  float cps = singFreq(self);
  float midi = SING_CPS2MIDI(cps);
  return midi;
}

/*--------------------------------------------------------------*/
void   singSetFreqGlideTime         (Singer* self, float coefficient)
{
  CONSTRAIN(coefficient, 0.0, 1.0);
  coefficient = pow(coefficient, self->fourFortyOverSampleRate);
  self->freqSmoothing = coefficient;
  self->oneMinusFreqSmoothing = 1 - coefficient;
}

/*--------------------------------------------------------------*/
void singSetLoudness(Singer* self, float coefficient)
{
  //CONSTRAIN(coefficient, 0, 1);
  if(coefficient<0)coefficient=0;
  coefficient *= coefficient;
  self->targetAmp = self->userAmp = coefficient;
}

/*--------------------------------------------------------------*/
float singLoudness(Singer* self)
{
  return sqrt(self->targetAmp);
}

/*--------------------------------------------------------------*/
void singSetLoudnessGlideTime (Singer* self, float coefficient)
{
  CONSTRAIN(coefficient, 0.0, 1.0);
  coefficient = pow(coefficient, self->fourFortyOverSampleRate);
  self->ampSmoothing = coefficient;
  self->oneMinusAmpSmoothing = 1 - coefficient;  
}

/*--------------------------------------------------------------*/
float singLoudnessGlideTime(Singer* self)
{
  return pow(self->ampSmoothing, 1.0/self->fourFortyOverSampleRate);
}

/*--------------------------------------------------------------*/
void   singSetVibratoRate           (Singer* self, float cps    )
{
  self->vibratoRate = self->userVibratoRate = (cps * self->twoPiOverSampleRate);
}

/*--------------------------------------------------------------*/
float  singVibratoRate              (Singer* self               )
{
  return (double)self->userVibratoRate / (double)self->twoPiOverSampleRate;
}

/*--------------------------------------------------------------*/
void   singSetVibratoDepth          (Singer* self, float percent)
{
  CONSTRAIN(percent, 0, 100);
  self->targetVibratoDepth = self->userVibratoDepth = percent / 100.0;
}

/*--------------------------------------------------------------*/
float  singVibratoDepth             (Singer* self               )
{
  return self->userVibratoDepth * 100;
}

/*--------------------------------------------------------------*/
void   singSetFreqDrift             (Singer* self, float percent    )
{
  CONSTRAIN(percent, 0, 100);
  self->freqDrift = percent / 100.0;
}

/*--------------------------------------------------------------*/
float  singFreqDrift                (Singer* self                   )
{
  return self->freqDrift * 100;
}

/*--------------------------------------------------------------*/
void   singSetVibratoRateDrift      (Singer* self, float percent    )
{
  CONSTRAIN(percent, 0, 100);
  self->vibratoRateDrift = percent / 100.0;
}

/*--------------------------------------------------------------*/
float  singVibratoRateDrift         (Singer* self                   )
{
  return self->vibratoRateDrift * 100;
}

/*--------------------------------------------------------------*/
void   singSetVibratoDepthDrift     (Singer* self, float percent    )
{
  CONSTRAIN(percent, 0, 100);
  self->vibratoDepthDrift  = percent / 100.0;
}

/*--------------------------------------------------------------*/
float  singVibratoDepthDrift        (Singer* self                   )
{
  return self->vibratoDepthDrift * 100;
}

/*--------------------------------------------------------------*/
void   singSetVibratoOnsetTime      (Singer* self, float coefficient)
{
  CONSTRAIN(coefficient, 0.0, 1.0);
  coefficient = pow(coefficient, self->fourFortyOverSampleRate);
  self->vibratoDepthSmoothing         =     coefficient;
  self->oneMinusVibratoDepthSmoothing = 1 - coefficient;  
}

/*--------------------------------------------------------------*/
float  singVibratoOnsetTime         (Singer* self)
{
  return pow(self->vibratoDepthSmoothing, 1.0/self->fourFortyOverSampleRate);
}

/*--------------------------------------------------------------*/
void   singTriggerVibrato           (Singer* self                   )
{
  self->vibratoRate       = self->userVibratoRate;
  self->targetVibratoDepth = self->userVibratoDepth;
  self->actualVibratoDepth = 0;
}

/*--------------------------------------------------------------*/
/*
void   singSetNoisePresence         (Singer* self, float presence   )
{
  self->noisePresence = presence;
  if(self->currentAllophone != NULL)
    self->targetNoisiness = alloNoisiness(self->currentAllophone) * self->noisePresence;
}
*/

/*--------------------------------------------------------------*/
/*
float  singNoisePresence            (Singer* self                   )
{
  return self->noisePresence;
}
*/

/*--------------------------------------------------------------*/
void       singSetPlosiveCrunchiness    (Singer* self, float coefficient   )
{
  CONSTRAIN(coefficient, 0, 100);
  self->plosiveCrunchiness = coefficient;
}

/*--------------------------------------------------------------*/
float      singPlosiveCrunchiness       (Singer* self                      )
{
  return self->plosiveCrunchiness;
}

/*--------------------------------------------------------------*/
void       singSetBreathiness           (Singer* self, float coefficient   )
{
  CONSTRAIN(coefficient, 0, 100);
  self->breathiness = coefficient;
  Allophone* allo = self->currentAllophone; //lets hope this is atomic
  if(allo != NULL)
    if(alloPhonemeCategory(allo) == alloVowel)
    {
      float noise = alloNoisiness(allo) * self->breathiness;
      if(noise > 1) noise = 1;
      self->targetNoisiness = noise;
    }
}

/*--------------------------------------------------------------*/
float      singBreathiness              (Singer* self                      )
{
  return self->breathiness;
}

/*--------------------------------------------------------------*/
void  singSetNoiseSmoothing        (Singer* self, float coefficient)
{
  CONSTRAIN(coefficient, 0, 1);
  coefficient = pow(coefficient, self->fourFortyOverSampleRate);
  self->noiseSmoothing = coefficient;
  self->oneMinusNoiseSmoothing = 1 - coefficient;
}

/*--------------------------------------------------------------*/
float  singNoiseSmoothing           (Singer* self                   )
{
  return pow(self->noiseSmoothing, 1.0/self->fourFortyOverSampleRate);
}

/*--------------------------------------------------------------*/
void singSetRelativeVowelVolume(Singer* self, float coefficient)
{
  CONSTRAIN(coefficient, 0, 1);
  coefficient = 1 - coefficient;
  
  if(coefficient < 0.5)
    {
      self->consonantVolume = coefficient * 2;
      self->vowelVolume     = 1;
    }
  else
    {
      self->consonantVolume = 1;
      self->vowelVolume     = (1 - coefficient) * 2;
    }
}

/*--------------------------------------------------------------*/
void   singSetRoughness             (Singer* self, float coefficient)
{
  CONSTRAIN(coefficient, 0, 1);
  self->roughness         =     coefficient;
  self->oneMinusRoughness = 1 - coefficient;
}

/*--------------------------------------------------------------*/
float  singRoughness                (Singer* self                   )
{
  return self->roughness;
}

/*--------------------------------------------------------------*/
void   singSetRoughnessFreq         (Singer* self, float cps        )
{
  CONSTRAIN(cps, 20, 150);
  self->roughnessFreq = cps * self->twoPiOverSampleRate;
}

/*--------------------------------------------------------------*/
float  singRoughnessFreq            (Singer* self                   )
{
  return self->roughnessFreq / self->twoPiOverSampleRate;
}

/*--------------------------------------------------------------*/
void     singSetBrightness            (Singer* self, float coefficient   )
{
  CONSTRAIN(coefficient, -1, 1);
  self->brightness = coefficient;
}

/*--------------------------------------------------------------*/
float    singBrightness               (Singer* self)
{
  return self->brightness;
}

/*--------------------------------------------------------------*/
void   singShutYerPieHole           (Singer* self)
{
  int N = fftN(self->fft) >> 1;
  fftSample_t* p = self->targetSpectralEnvelope;
  
  while(N-- > 0)
    *p++ = 0.001;
  self->targetNoisiness = 0;
}

/*--------------------------------------------------------------*/
void     singSetAllophoneCallback     (Singer* self, singEventCallback_t callback, void* callbackArg)
{
  self->allophoneCallback    = callback;
  self->allophoneCallbackArg = callbackArg;
}

/*--------------------------------------------------------------*/
void     singSetQueueEmptyCallback    (Singer* self, singEventCallback_t callback, void* callbackArg)
{
  self->queueEmptyCallback    = callback;
  self->queueEmptyCallbackArg = callbackArg;
}

/*--------------------------------------------------------------*/
void       singSetAllophoneSeparatorChar(Singer* self, char c)
{
  self->allophoneSeparatorChar = c;
}

/*--------------------------------------------------------------*/
char       singAllophoneSeparatorChar   (Singer* self)
{
  return self->allophoneSeparatorChar;
}

/*--------------------------------------------------------------*/
void       singSetRestChar              (Singer* self, char c)
{
  self->restChar = c;
}

/*--------------------------------------------------------------*/
char       singRestChar                 (Singer* self)
{
  return self->restChar;
}

/*--------------------------------------------------------------*/
void       singSetHoldChar              (Singer* self, char c)
{
  self->holdChar = c;
}

/*--------------------------------------------------------------*/
char       singHoldChar                 (Singer* self)
{
  return self->holdChar;
}

/*--------------------------------------------------------------*/
//void singWavetableSynthesizer(void* SELF, float* buffer, int numSamples)
//{
//  Singer* self = (Singer*)SELF;
//  float* wavetable = self->wavetable;
//  float stepSize, mantissa, f1, f2;
//  uint32_t freq;
//  while(numSamples--)
//    {
//      freq = self->actualFreq;
//      freq += (uint32_t)(freq * fastsin(self->vibratoPhase) * self->actualVibratoDepth);
//      stepSize = freq / (double) self->wavetableFundamental;
//      self->wavetableCurrentSample += stepSize;
//      while(self->wavetableCurrentSample >= self->wavetableNumSamples)
//        self->wavetableCurrentSample -= self->wavetableNumSamples;
//
//      /*linear interpolation*/
//      f1 = wavetable[(int)self->wavetableCurrentSample];
//      f2 = wavetable[(int)(self->wavetableCurrentSample + 1) % self->wavetableNumSamples];
//      mantissa =  ((double)self->wavetableCurrentSample) - ((int)self->wavetableCurrentSample);
//      
//      *buffer = mantissa * (f2 - f1) + f1;
//      *buffer++ *= -fastsin(self->vibratoPhase) * (10 * self->actualVibratoDepth) + (1 - (10 * self->actualVibratoDepth));
//  
//      singAdvaceSynthVariables(self);
//    }
//}

/*--------------------------------------------------------------*/
//void singAdvaceSynthVariables(Singer* self)
//{
//  float temp;
//  
//  if(self->consonantTimer > 0)
//    {
//      self->consonantTimer--;
//      if(self->consonantTimer == 0)
//        singConsonantTimerExpired(self);
//    }
//  self->controlRateTimer += 2;
//  if(self->controlRateTimer == 0)
//    {
//      int N = fftN(self->fft) >> 1;
//      fftSample_t *a = self->actualSpectralEnvelope, *b = self->targetSpectralEnvelope;
//      while(N-- > 0)
//        {
//          *a = (*a * self->spectralSmoothing) + (*b * self->oneMinusSpectralSmoothing);
//          a++, b++;
//        }
//      }
//
//  self->actualFreq         = (self->actualFreq         * self->freqSmoothing)         + (self->targetFreq         * self->oneMinusFreqSmoothing);
//  self->actualVibratoDepth = (self->actualVibratoDepth * self->vibratoDepthSmoothing) + (self->targetVibratoDepth * self->oneMinusVibratoDepthSmoothing);
//  self->actualNoisiness    = (self->actualNoisiness    * self->noiseSmoothing)        + (self->targetNoisiness    * self->oneMinusNoiseSmoothing);
//  self->actualAmp          = (self->actualAmp          * self->ampSmoothing)          + (self->targetAmp          * self->oneMinusAmpSmoothing);
//  
//  self->phase += self->actualFreq;
//  self->vibratoPhase += self->vibratoRate;
//  
//  temp = self->freqDrift * self->targetFreq;
//  self->targetFreq          += SING_RANDOM(-temp, temp) + 0.5;
//  
//  temp = self->vibratoRateDrift * self->vibratoRate;
//  self->vibratoRate         += SING_RANDOM(-temp, temp) + 0.5;
//  
//  temp = self->vibratoDepthDrift * self->targetVibratoDepth;
//  self->targetVibratoDepth  += SING_RANDOM(-temp, temp);
//}

/*--------------------------------------------------------------*/
//void     singFillBuffer (Singer* self, float* buffer, int numSamples)
//{
//  int n;
//  fftSample_t* magnitude, *b;
//  singWavetableSynthesizer(self, buffer, numSamples);
//
//  b = buffer; n = numSamples;
//  while(n-- > 0)
//    {
//      *b = (*b * (1 - self->actualNoisiness)) + (SING_RANDOM(-1.0, 1.0) * self->actualNoisiness);
//      b++;
//    }
//
//  fftFilter(self->fft, buffer, NULL, numSamples, &magnitude, NULL, self->actualSpectralEnvelope, numSamples, self->brightness);
//  b = buffer, n = numSamples;
//  memcpy(b, magnitude, n * sizeof(*b));
//
//  while(n-- > 0)
//    {
//      *b *= self->actualAmp;
//      *b *= ((fabs(fastsin(self->roughnessPhase))*self->roughness)+self->oneMinusRoughness);
//      self->roughnessPhase += self->roughnessFreq;
//      
//      if(self->ampEnvRemaining > 0)
//        {
//          SING_SPINLOCK(self->ampEnvIsLocked);
//          *b *= *(self->ampEnv)++ * self->consonantVolume;
//          self->ampEnvRemaining--;
//          SING_SPINUNLOCK(self->ampEnvIsLocked);
//        }
//
//      else if(self->ampEnv != NULL)
//        *b *= 0;
//      else
//        *b *= self->vowelVolume;
//      b++;
//    }
//}


void     singFillBuffer (Singer* self, float* buffer, int numSamples)
{
  /*--wavetable-----------------------------------------------*/
  float* wavetable = self->wavetable;
  float stepSize, mantissa, f1, f2, temp;
  uint32_t freq;
  fftSample_t *b = buffer;
  int n = numSamples;
  
  while(n--)
    {
      freq = self->actualFreq;
      freq += (uint32_t)(freq * fastsin(self->vibratoPhase) * self->actualVibratoDepth);
      stepSize = freq / (double) self->wavetableFundamental;
      self->wavetableCurrentSample += stepSize;
      while(self->wavetableCurrentSample >= self->wavetableNumSamples)
        self->wavetableCurrentSample -= self->wavetableNumSamples;

      /*linear interpolation*/
      f1 = wavetable[(int)self->wavetableCurrentSample];
      f2 = wavetable[(int)(self->wavetableCurrentSample + 1) % self->wavetableNumSamples];
      mantissa =  ((double)self->wavetableCurrentSample) - ((int)self->wavetableCurrentSample);
      
      *b = mantissa * (f2 - f1) + f1; //wavetable
      *b = (*b * (1 - self->actualNoisiness)) + (SING_RANDOM(-1.0, 1.0) * self->actualNoisiness); //noisiness
      *b *= self->actualAmp;          //master volume
      *b *= ((fabs(fastsin(self->roughnessPhase))*self->roughness)+self->oneMinusRoughness); //roughness
      *b *= -fastsin(self->vibratoPhase) * (10 * self->actualVibratoDepth) + (1 - (10 * self->actualVibratoDepth)); //vibrato
      b++;
  
  /*--advance synth variables------------------------------------*/
  
      if(self->consonantTimer > 0)
        {
          self->consonantTimer--;
          if(self->consonantTimer == 0)
            singConsonantTimerExpired(self);
        }
      self->actualFreq         = (self->actualFreq         * self->freqSmoothing)         + (self->targetFreq         * self->oneMinusFreqSmoothing);
      self->actualVibratoDepth = (self->actualVibratoDepth * self->vibratoDepthSmoothing) + (self->targetVibratoDepth * self->oneMinusVibratoDepthSmoothing);
      self->actualNoisiness    = (self->actualNoisiness    * self->noiseSmoothing)        + (self->targetNoisiness    * self->oneMinusNoiseSmoothing);
      self->actualAmp          = (self->actualAmp          * self->ampSmoothing)          + (self->targetAmp          * self->oneMinusAmpSmoothing);
  
      self->phase += self->actualFreq;
      self->vibratoPhase += self->vibratoRate;

      self->roughnessPhase += self->roughnessFreq;
    }
    
    /*control rate variables*/
    int N = fftN(self->fft) >> 1;
    fftSample_t *a = self->actualSpectralEnvelope;
    b = self->targetSpectralEnvelope;
    while(N-- > 0)
      {
        *a = (*a * self->spectralSmoothing) + (*b * self->oneMinusSpectralSmoothing);
        a++, b++;
      }    
    temp = self->freqDrift * self->targetFreq;
    self->targetFreq          += SING_RANDOM(-temp, temp) + 0.5;

    temp = self->vibratoRateDrift * self->vibratoRate;
    self->vibratoRate         += SING_RANDOM(-temp, temp) + 0.5;
  
    temp = self->vibratoDepthDrift * self->targetVibratoDepth;
    self->targetVibratoDepth  += SING_RANDOM(-temp, temp);

  /*--origFillBuffer--------------------------------------------*/
  fftSample_t* magnitude;
  float        targetAmpEnv = 0;

  //b = buffer; n = numSamples;
  fftFilter(self->fft, buffer, NULL, numSamples, &magnitude, NULL, self->actualSpectralEnvelope, numSamples, self->brightness);
  b = buffer, n = numSamples;
  memcpy(b, magnitude, n * sizeof(*b));

  while(n-- > 0)
    {      
      if(self->ampEnvRemaining > 0)
        {
          SING_SPINLOCK(self->ampEnvIsLocked);
          targetAmpEnv = *(self->ampEnv)++ * self->consonantVolume;
          self->ampEnvRemaining--;
          SING_SPINUNLOCK(self->ampEnvIsLocked);
        }

      else if(self->ampEnv != NULL)
        targetAmpEnv = 0;
      else
        targetAmpEnv = self->vowelVolume;
      
      self->smoothedAmpEnv = (self->smoothedAmpEnv * 0.99) + (targetAmpEnv * 0.01);
      *b++ *= self->smoothedAmpEnv;
    }    
    
}
