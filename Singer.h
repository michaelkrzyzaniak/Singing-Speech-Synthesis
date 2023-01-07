/*
 *  Synth.h
 *  Make weird noises
 *
 *  Made by Michael Krzyzaniak at Arizona State University's
 *  School of Arts, Media + Engineering in Spring of 2013
 *  mkrzyzan@asu.edu
 */

#ifndef __SINGER__
#define __SINGER__ 1

#if defined(__cplusplus)
extern "C"{
#endif   //(__cplusplus)

#include "lib/Phonologist.h"
#include "lib/List.h"
#include "lib/MKAiff.h"

typedef struct OpaqueSingerStruct Singer;

/*singCurrentAllophone() refers to the previous allophone, allo to the target allophone*/
typedef void (*singEventCallback_t)     (void* userData, Allophone* allo);

Singer*    singNew                      (char* pathToWavetableAiff, char* pathToAllophoneAiffs, double sampleRate, int bufferNumSamples);
Singer*    singDestroy                  (Singer* self                      );

void       singFillBuffer               (Singer* self, float* buffer, int numSamples);

void       singSetAllophone             (Singer* self, alloSymbol_t symbol );
Allophone* singCurrentAllophone         (Singer* self                      );
void       singEnqueueAllophones        (Singer* self, alloSymbol_t symbols);
void       singSetAllophoneGlideTime    (Singer* self, float coefficient   );
float      singAllophoneGlideTime       (Singer* self                      );
List*      singEnqueuedAllophones       (Singer* self                      );
char**     singSupportedAllophones      (Singer* self, int* returnedNumAllophones); /*must free retunred array when done (but not the strings therein)*/
void       singPrintSupportedAllophones (Singer* self                      );
void       singTriggerNextAllophone     (Singer* self                      );
void       singTriggerNextVowel         (Singer* self                      );
void       singSetConsonantDuration     (Singer* self, float millisecs     );
float      singConsonantDuration        (Singer* self                      );
void       singSetFreq                  (Singer* self, float cps, BOOL shouldTriggerVibrato);
float      singFreq                     (Singer* self                      );
void       singSetFreqDrift             (Singer* self, float percent       );
float      singFreqDrift                (Singer* self                      );
void       singSetPitch                 (Singer* self, float midiNumber, BOOL shouldTriggerVibrato);
float      singPitch                    (Singer* self                      );
void       singSetFreqGlideTime         (Singer* self, float coefficient   );
void       singSetLoudness              (Singer* self, float amp           );
float      singLoudness                 (Singer* self                      );
void       singSetLoudnessGlideTime     (Singer* self, float coefficient   );
float      singLoudnessGlideTime        (Singer* self                      );
void       singSetVibratoRate           (Singer* self, float cps           );
float      singVibratoRate              (Singer* self                      );
void       singSetVibratoDepth          (Singer* self, float percent       );
float      singVibratoDepth             (Singer* self                      );
void       singSetVibratoRateDrift      (Singer* self, float percent       );
float      singVibratoRateDrift         (Singer* self                      );
void       singSetVibratoDepthDrift     (Singer* self, float percent       );
float      singVibratoDepthDrift        (Singer* self                      );
void       singSetVibratoOnsetTime      (Singer* self, float coefficient   );
float      singVibratoOnsetTime         (Singer* self                      );
void       singTriggerVibrato           (Singer* self                      );
//void       singSetNoisePresence         (Singer* self, float presence      ); //deprecated
//float      singNoisePresence            (Singer* self                      ); //deprecated
void       singSetPlosiveCrunchiness    (Singer* self, float crunchiness   );
float      singPlosiveCrunchiness       (Singer* self                      );
void       singSetBreathiness           (Singer* self, float breathiness   );
float      singBreathiness              (Singer* self                      );
void       singSetNoiseSmoothing        (Singer* self, float coefficient   );
float      singNoiseSmoothing           (Singer* self                      );
void       singSetRelativeVowelVolume   (Singer* self, float coefficient   );
void       singSetRoughness             (Singer* self, float coefficient   );
float      singRoughness                (Singer* self                      );
void       singSetRoughnessFreq         (Singer* self, float cps           );
float      singRoughnessFreq            (Singer* self                      );
void       singSetBrightness            (Singer* self, float coefficient   ); //-1 ~ 1, 0 is unaltered, neg is darker
float      singBrightness               (Singer* self                      );
void       singShutYerPieHole           (Singer* self                      ); 
void       singSetAllophoneCallback     (Singer* self, singEventCallback_t callback, void* callbackArg);
void       singSetQueueEmptyCallback    (Singer* self, singEventCallback_t callback, void* callbackArg);
void       singSetAllophoneSeparatorChar(Singer* self, char c              );
char       singAllophoneSeparatorChar   (Singer* self                      );
void       singSetRestChar              (Singer* self, char c              );
char       singRestChar                 (Singer* self                      );
void       singSetHoldChar              (Singer* self, char c              );
char       singHoldChar                 (Singer* self                      );

#if defined(__cplusplus)
}
#endif   //(__cplusplus)

#endif   // __SINGER__

