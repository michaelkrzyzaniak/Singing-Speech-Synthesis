# Singing-Speech-Synthesis
Singing-Speech-Synthesis is an ANSI C library that sings in real time. It sings vowels and consonants. Just tell it what notes and allophones you want it to sing. Then feed empty buffers into it, and it will fill them with audio, with which you can do as you please. It requires no external libraries or packages, and has no platform-dependent code. It was designed to run on ebmedded linux computers in musical robots, and It should run on anything.

A demo of Singing-Speech-Synthesis is on Youtube.

To get started, Singing-Speech-Synthesis needs a wavetable that it will use as the basis of the sound it makes. There are a few demo wavetables in audio/wavetables. Then Singing-Speech-Synthesis also needs a recording of each allophone you want it to be able to sing. There is a demo set of allophones in audio/tenor_allophones/, which I crappily recorded into my laptop while sitting in my parent's living room several years ago. Singing-Speech-Synthesis will figure out whether each allophone is a vowel, plosive or fricative by analyzing the recordings. Then you can tell it what pitches and allophone to sing. 

## Getting Started Code Snipit
```c
//Compile with
//gcc *.c lib/*.c

#include "Singer.h"

int main(void)
{
  const int buffer_size = 128;

  char* wavetable_aiff = "audio/wavetables/Cello_41.aiff";
  char* allophone_folder = "audio/tenor_allophones/";
  
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

  return 0;
}
```

Alternatively, a sequence of allophones can be enqueued and then triggered in succession. This allows a song to be programmed in advance. This is done using a string in the folowing form: "a-|m|a-|r|i-|l|i-|m|i|a-| |b|e-|l|a-| -"; By default the pipe charcter | separates allophones, the space character indicates a rest, and the dash character - indicates a sustain. The enqueued alophones can either be triggered one by one, or it can sing through the consonants and pause on the vowels. In this latter mode, whenever the next allophone is triggered, Singing-Speech-Synthesis will sing the next enqueued allophones in rapid succeession until it encounters a dash, then it will sustain and wait to be riggered again. Note that a rest may or may note be sustained. If a plosive is 'sustained', the singer will sing the plosive and then sustain silence. 

## Getting Started Code Snipit With Enqueued Allophones
```c
//gcc *.c ../../lib/*.c

#include <stdio.h>
#include "../../Singer.h"

int main(int argc, const char** argv)
{
  const int buffer_size = 128;
  const int sample_rate = 44100;

  char* wavetable_aiff = "audio/wavetables/Cello_41.aiff";
  char* allophone_folder = "audio/tenor_allophones/";
  
  Singer* singer = singNew(wavetable_aiff, allophone_folder, sample_rate, buffer_size);
  
  float buffer[buffer_size];

  char allophones[] = "a-|m|a-|r|i-|l|i-|m|i|a-| |b|e-|l|a-| -";
  singEnqueueAllophones(singer, allophones);
  singSetLoudness(singer, 0.5);
  
  //1 sec per allophone
  int buffers_per_allophone = ceil(sample_rate / (double)buffer_size);
  int pitch = 60; //MIDI note middle c
  int i;
  
  while(listCount(singEnqueuedAllophones(singer)) > 0)
    {
      singSetPitch(singer, pitch++, YES);
      singTriggerNextVowel(singer);       //go to the next hold symbol and wait
      //singTriggerNextAllophone(singer); //go to the next allophone and wait
      
      for(i=0; i<buffers_per_allophone; i++)
        {
          singFillBuffer(singer, buffer, buffer_size);
          //buffer is full of audio, do something with it here.
        }
      }
  
  return 0;	
}

```

Complete versions of these code snipits are in /demos/

## Constructor, Destructor, Processing Audio
##### Overview

##### New
```c
Singer*    singNew(char* pathToWavetableAiff, char* pathToAllophoneAiffs, double sampleRate, int bufferNumSamples);
```
Create a new Singer object.
Args: 
* pathToWavetableAiff: path of an aiff or wave file containing a single cycle of a waveform that will be used as the basis of wavetable synthesis. Try "/audio/wavetables/Cello_41.aiff".
* pathToAllophoneAiffs: path of a folder containing allophone recordings. "audio/tenor_allophones/". Silence should be trimmed off the begining and end of the recording. In principal the software could be made to do this in the future and if you send me some cookies I might do that. The file extension should be .aif or .aiff, but wave files should work just so long as they have the (wrong) .aif extension.
* sampleRate your desired audio sample rate, perhaps 44100 would be a good choice.
* bufferNumSamples this controls the size of the underlying FFTs. It could be anythhing but maybe try a power of 2 somewhere between 128 and 4096.


##### Destroy
```c
Singer*    singDestroy                  (Singer* self);
```
Free the Singer object and deallocate all of its internal resources. Returns (Singer\*) NULL; It is good practice to assign to result to the object you are destroying, e.g.
singer = singDestroy(singer);

##### Fill Buffer
```c
void       singFillBuffer               (Singer* self, float* buffer, int numSamples);
```
Fill an empty buffer with an audio waveform containing the next bit of the beautiful melodious sounds of robot singing voice.
Args:
* self: the singer object that you previously created with singNew();
* buffer: and empty array where you want audio samples to be written
* numSamples: the size of the audio buffer, I don't exactly remember the rationale, but it should probably be the same size as the undrelying ffts, which is controlled by the bufferNumSamples argument to singNew(); So whatever value you used there maybe consider using the same value here.


## Allophones
##### SetAllophone, CurrentAllophone
```c
void       singSetAllophone             (Singer* self, alloSymbol_t symbol );
Allophone* singCurrentAllophone         (Singer* self                      );
```
Set or get the current allophone. Setting the allophone causes the singer to sing that sound. Getting it tells you what sound the singer is currently singing.
Args:
* self: the singer object that you previously created with singNew();
* symbol: This should be a null terrminated character array (not a constant string) that represents the filename of the corresponding allophone recording. For example
```c
Singer* singer = singNew("/audio/wavetables/Cello_41.aiff", "audio/tenor_allophones/", 44100, 512);
char[] allo = "foo"; //sing and stop
//char[] allo = "foo-"; //sing and sustain
singSetAllophone(singer, allo);
```
causes the algorithm to look up audio/tenor_allophones/foo.aif\* and use that to synthesize sound. The singer will sing a bit of sound and then stop. If you want it to sustain the sound indefinitely, append the hold character to the end of the allophone symbol. By default the hold character is dash "-" but that can be changed with singSetHoldChar(); Note that plosives are sustained with silence, and the algorithm automatically determines which allophones are plosives. If you want the algorithm to sing a reset, you can use the rest character which by default is a space, e.g. char[] allo = " "; That can be changed with singSetRestChar(). Or  you can reest by calling singShutYerPieHole(); 

Return:
singCurrentAllophone returns an Allophone object. You can get the string representing the allophone by calling alloSymbol() on the retuned object. Other things that can be done with the object are described in lib/allophone.h.

##### EnqueueAllophones, EnqueuedAllophones, TriggerNextAllophone, TriggerNextVowel.
```c
void       singEnqueueAllophones        (Singer* self, alloSymbol_t symbols);
List*      singEnqueuedAllophones       (Singer* self                      );
void       singTriggerNextAllophone     (Singer* self                      );
void       singTriggerNextVowel         (Singer* self                      );

void       singSetAllophoneSeparatorChar(Singer* self, char c              );
char       singAllophoneSeparatorChar   (Singer* self                      );
void       singSetRestChar              (Singer* self, char c              );
char       singRestChar                 (Singer* self                      );
void       singSetHoldChar              (Singer* self, char c              );
char       singHoldChar                 (Singer* self                      );

typedef void (*singEventCallback_t)     (void* userData, Allophone* allo);
void       singSetAllophoneCallback     (Singer* self, singEventCallback_t callback, void* callbackArg);
void       singSetQueueEmptyCallback    (Singer* self, singEventCallback_t callback, void* callbackArg);

```
Instead of setting allophones one at a time, you can program in a whole song by enqueuing a list of allophones. See the code snipit at the top of this document for more information on how this works. 

Args:
* symbols: A character array (not a string constant) containing the allophones. It should be of the form: "a-|m|a-|r|i-|l|i-|m|i|a-| |b|e-|l|a-| -"; By default the pipe charcter | separates allophones, the space character indicates a rest, and the dash character - indicates a hold (sustain). These can be changed with singSetAllophoneSeparatorChar(), singSetRestChar(), and singSetHoldChar(); The enqueued allophones can either be triggered one by one using singTriggerNextAllophone(), in which case the singer will simply sing the next allophone and then wait to be retriggered. Or it can sing through the consonants and pause on the next sustained note (usually a vowel) by using singTriggerNextVowel(). In this latter mode, whenever the next allophone is triggered, the singer will sing the next enqueued allophones in rapid succeession until it encounters a dash, then it will sustain and wait to be riggered again. Note that a rest may or may note be sustained. If a plosive is 'sustained', the singer will sing the plosive and then sustain silence. 

You can get notifications when each allophone is triggered or when the queue is empty, using the respective callbacks. 
* callbackArg can be anything that will be passed back to you as userData when the callback is called. 
* allo will be the allophone that is going to be sung next, whereas calling singCurrentAllophone() from the callback will get the allophone that has just finished. Moe information about what can be done with the passed Allophone object is in lib/Allophone.h. Notably you can get the string representation of the allophone with alloSymbol(allo);

##### Allophone Glide Time
```c
void       singSetAllophoneGlideTime    (Singer* self, float coefficient   );
float      singAllophoneGlideTime       (Singer* self                      );
```
The algorithm will quickly interpolate between successive allophones. The coefficient should be betweeen 0 and 1, with higher values indicating a longer transition time. Default is 0.6.

##### Supported Allophones
```c
char**     singSupportedAllophones      (Singer* self, int* returnedNumAllophones); /*caller must free retunred array when done (but not the strings therein)*/
void       singPrintSupportedAllophones (Singer* self                      );
```
Get or print a list of supported allophones. Printing the list will also tell you if the algorithm considers each allophone to be a plosive, fricative, or vowel. It also prints the bytes representting the allophone symbols, which can be useful if the allophone has a filename such as ð.aiff and there are char eencoding issues. 

##### Consonant Duration
```c
void       singSetConsonantDuration     (Singer* self, float millisecs     );
float      singConsonantDuration        (Singer* self                      );
```
Controls how long a consonant should last when it is not held. Default is 100 milliseconds.

## Pitch Loudness and Timbre
##### Set Freq, Set Pitch
```c
void       singSetFreq                  (Singer* self, float cps, BOOL shouldTriggerVibrato);
float      singFreq                     (Singer* self                      );
void       singSetPitch                 (Singer* self, float midiNumber, BOOL shouldTriggerVibrato);
float      singPitch                    (Singer* self                      );
void       singSetFreqGlideTime         (Singer* self, float coefficient   );
```
Get or set the pitch that the singer sings. By default the singer will glide to the new pitch and optionally re-start its vibrato.
Args:
* cps: the desired frequency in cycles per second where 440 represeents A above middle C.
*midiNumber: the desired frequency expressed as a MIDI note number where 69 represents A above middle C, and incrementing or decrementing by one always raises or lowers by a half step. Note that non-integer MIDI note numbers are also possible for that sweet sweet microtonal music.
* shouldTriggerVibrato: should be YES or NO (as a constant, not a string). By default the singer starts singing straight and then gradually adds vibrato which drifts in amplitude and frequency. If the singer is already singing vibrato, then passing NO here makes the singer continue its vibrato through the change of notes. Passing YES causes it to quit its vibrato and sing the begining  of the note straight before gradually adding vibrato again. There are a separate set of functions that allow you to change the vibrato behaviour or turn it off.
* coefficient: controls how quickly the singer glides to the nwe note. Should be betweeen 0 and 1, where 0 means no glide (jumps to the new note) and higher values indicate a longer glide. Default is 0.999. 


##### Set Freq Drift
```c
void       singSetFreqDrift             (Singer* self, float percent       );
float      singFreqDrift                (Singer* self                      );
```
By default the singer will gradually drift in pitch when it sings a sustained note. 
Args:
* percent: Should be betweeen 0 and 100, with 0 indicating no drift, and higher values indicating more drift. Default is 0.0015. Note that the drift is unbounded over time.



##### Vibrato
```c
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
```
Control the singer's vibrato. Default rate is 6 Hz. Default depth is 1, indicating plus or minus 1 percent of current pitch. Both the rate and depth drift over time to make it sound more natural. Default rate drift coeficient is 0.05, default depth drift coefficient is 0.1. Setting either coefficient to 0 turns it off, with higher values meaning more drift. Note that the drift is unbounded over time. By default the singer will start singing straight and gradually increase the vibrato depth. Thee vibrato onset time controls how long it takes to achieve full vibrato depth. Default is 0.999992, with 0  indicating immediate onset and higher values (less than 1) indicating longer onseet time. By default the vibrato will just keep going forever, but calling singTriggerVibrato() resets all of the drift values and makes the singer sing straight, gradually bringing in the vibrato again. Triggering the vibrrato can also be done by passing YES to the shouldTriggerVibrato argument of singSetFreq() or singSetPitch();

##### Loudness, Loudness Glide Time 
```c
void       singSetLoudness              (Singer* self, float amp           );
float      singLoudness                 (Singer* self                      );
void       singSetLoudnessGlideTime     (Singer* self, float coefficient   );
float      singLoudnessGlideTime        (Singer* self                      );
```

Get or set the amplitude (not perceptual loudness) of the output. In principal it should be between 0 and 1, default is 1. Lower it if the output is clipping. By default the algorithm will quickly interpolate to the new loudness to avoid clicks in the audio stream. The glide coefficient should be between 0 and 1 with 0 meaning no interpolation and higher values indicating longer time to reach tthe specified amplitude. Default is 0.999. Consider increasing brightness when you increase loudness, see note at singSetBrightness() below.




##### Plosive Crunchiness
```c
void       singSetPlosiveCrunchiness    (Singer* self, float crunchiness   );
float      singPlosiveCrunchiness       (Singer* self                      );
```
This controls the amplitude of noise in plosives and fricatives. Default is 2.5.

##### Breathiness
```c
void       singSetBreathiness           (Singer* self, float breathiness   );
float      singBreathiness              (Singer* self                      );
```
This controls the amplitude of noise in vowels. Default is 4. RRange is 0 to 100.

##### Noise Smoothing
```c
void       singSetNoiseSmoothing        (Singer* self, float coefficient   );
float      singNoiseSmoothing           (Singer* self                      );
```
When the singer transitions from one allophone to another, the amount of noisiness depends on 1) the intrinsic noisiness of the allophone recording, and 2) the settings of singSetBreathiness() or singSetPlosiveCrunchiness(). To smooth out the transition, thee singer can interpolate between the noise levels. By default, coefficient is 0 meaning there is no interpolation and the transition is immediate. Larger coefficient meeans longer transition. Must be less than one.

##### Relative Vowel Volume
```c
void       singSetRelativeVowelVolume   (Singer* self, float coefficient   );
```
Seet the amplitude of vowels relative to consonants. Should be 0 to 1 inclusive. Higher values make louder vowels. If coefficient is less than 0.5 this is accomplished by raising the amplitude of consonants, otherwise it raises the amplitude of vowels. Default is 0.5. I don't know why there is no getter method.  

##### Roughness
```c
void       singSetRoughness             (Singer* self, float coefficient   );
float      singRoughness                (Singer* self                      );
void       singSetRoughnessFreq         (Singer* self, float cps           );
float      singRoughnessFreq            (Singer* self                      );
```
You can add roughness to the singer's voice. If I recall corrrectly this uses ampitude modulation. The coefficent controlls how rough the sound is, and should range from 0 to 1, with the default being 0.1. The roughness frrequency is the frequency of the modulating signal and can range from 20 to 150 Hz. Default is 40.

##### Brightness
```c
void       singSetBrightness            (Singer* self, float coefficient   );
float      singBrightness               (Singer* self                      );
```
This controlls a high-pass or low-pass filter that can make the singer's voice brighter or darker. The coefficient can range from -1 to 1 where 0 is unaltered, and negative is darker. Default is -0.2. Brightness plays an inportant role in pereceived loudness for the human voice, as nattural human singing voices increase in brightness as they get louder. Therefore it might be good practice to use this function together with singSetLoudness(); 

##### Shut Up
```c
void       singShutYerPieHole           (Singer* self                      ); 
```
Shut it.
