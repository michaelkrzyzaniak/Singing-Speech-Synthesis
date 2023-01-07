//OSX Compile with:
//gcc *.c ../../lib/*.c -framework AudioToolbox

//#define AVE_SOPRANO
//#define AVE_ALTO
//#define AVE_TENOR
//#define AVE_BASS
#define AMARILLI
//#define SHUSHI
//#define SHUSHI_TENOR
//#define SHUSHI_BASS
//#define SHUSH_END

#include "Synth.h"
#include "AveMaria.h"
#include "Amarilli.h"
#include "Shushi.h"
#include <stdio.h>
#include <pthread.h>

void iHateCanonicalInputProcessingIReallyReallyDo(void);
void makeStdinCannonicalAgain();
void* oscListen(void* SELF);
void allophoneWasSet(void* self, Allophone* allo);
void queueEmptied   (void* self, Allophone* allo);

Singer* sing;

int numPitches = sizeof(pitches) / sizeof(*pitches);
int pitchIndex = -1;

/*---------------------------------------------------------------*/
float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/*------------------------------------------------------------------------------------*/
int main(int argc, const char** argv)
{
  Synth* s = syNew();
  if(s == NULL) {fprintf(stderr, "could not create a synth object, aborting\n"); return 1;}
  sing = syTenor(s);
  singSetAllophoneCallback (sing, allophoneWasSet, NULL);
  singSetQueueEmptyCallback(sing, queueEmptied   , NULL);

  
  singSetVibratoDepth(sing, 0);
  singSetVibratoDepthDrift(sing, 0);

  //////////FOR SHUHSI ONLY!!!!!!!
  //singSetFreqGlideTime(sing, 0.99975);
  //singSetFreqGlideTime(sing, 0.9995);
  singSetRelativeVowelVolume(sing, 0.7);
  //singSetBrightness(sing, 0.7);
  //singSetRoughness(sing, 0.0); 
  //singSetBreathiness(sing, 2);
  
  int MIDI = m_D4;
  
  pthread_t pThread;
  int error = pthread_create(&pThread, NULL, oscListen, NULL /*self*/);
  singEnqueueAllophones(sing, text);
  singSetPitch(sing, pitches[0], NO);
  
  singSetLoudness(sing, 0.5);
  auPlay((Audio*) s);
  
  char c[2];
  c[1] = '\0';
  
  printf("OK\n");
  //return 1; //!!!!!!!!!!!!!!!!!!!For measuring load time!
  
  singPrintSupportedAllophones(sing);
  
  iHateCanonicalInputProcessingIReallyReallyDo();
  
  for(;;)
    {
      *c = getchar();
      if(((*c >= 'a') && (*c <= 'z')) || (*c == ','))
        singSetAllophone(sing, c);
      else
        {
          switch(*c)
            { 
              case '\t': 
              case ']':  singTriggerNextVowel(sing); ++pitchIndex; pitchIndex %= numPitches; singSetPitch(sing, pitches[pitchIndex], YES); break;
              case '[': --pitchIndex; pitchIndex %= numPitches; singSetPitch(sing, pitches[pitchIndex], YES); break;
              
              case '{': singSetPitch(sing, ++MIDI, YES); break;
              case '}': singSetPitch(sing, --MIDI, NO); break;

              case ' ': singShutYerPieHole(sing); break;

              case 3  : goto cleanup;
            }
        }
    }
    
  cleanup:  
  auPause((Audio*) sing);
  usleep(1000);
  makeStdinCannonicalAgain();
  singDestroy(sing);
  
  return 0;	
}

/*---------------------------------------------------------------*/
#define UDP_BUFFER_SIZE 100
char udpBuffer[UDP_BUFFER_SIZE];
#include "Wifi.h"
#include "OSC.h"
void* oscListen(void* SELF)
{
  static float smoothedAccelY = 0, prevSmoothedAccelY = 0;
  static float smoothedAccelZ = 0, prevSmoothedAccelZ = 0;
   
  char *oscAddress, *oscTypeTag;
  oscValue_t *oscValues;
  int numChars;
  wiSetup();
  
  //singSetVibratoDepth(sing, 0); //let the accel do it
  
  for(;;)
    {
      numChars = wiRead(udpBuffer, UDP_BUFFER_SIZE, 12);
      if(numChars > 0)
        {
          int numOscValues = oscParse(udpBuffer, numChars, &oscAddress, &oscTypeTag, &oscValues);
          //printf("%s %s %f\r\n", oscAddress, oscTypeTag, oscValues[0].f);
          //printf("%s : %s : %f : %f : %f\n", oscAddress, oscTypeTag, oscValues[0].f, oscValues[1].f, oscValues[2].f);
          switch(oscHash((unsigned char*)oscAddress))
            {
              //printf("%s %s\n", oscAddress, oscTypeTag);
              case 2138174001: // "/1/fader1"
                singSetBrightness(sing, (oscValues[0].f * 2 - 1));
                break;
              case 2138174002: // "/1/fader2"
                singSetLoudness(sing, oscValues[0].f);
                break;
              case 2138174003: //"/1/fader3"
                singSetRoughness(sing, oscValues[0].f);
                break;
              case 2138174004: //"/1/fader4"
                singSetBreathiness(sing, oscValues[0].f * 20);
                break;
              case 103849527:// /"/3/xy" cascade
              case 1168970551: // "/1/toggle1"
               singTriggerNextVowel(sing);
               ++pitchIndex; 
               pitchIndex %= numPitches; 
               singSetPitch(sing, pitches[pitchIndex], YES); 
               break;
              case 2741611824 : // "/accelxyz"
                prevSmoothedAccelY = smoothedAccelY;
                prevSmoothedAccelZ = smoothedAccelZ;
                smoothedAccelY = (0.9 * smoothedAccelY) + (0.1 * oscValues[1].f);
                smoothedAccelZ = (0.9 * smoothedAccelZ) + (0.1 * oscValues[2].f);
                //singSetLoudness(sing, mapf(smoothedAccelY, -0.25, 1, 0, 1));
                singSetLoudness(sing, mapf(smoothedAccelY, -0.25, 1, 0, 1));
                //singSetRoughness(sing, smoothedAccel * 0.33);
                singSetBreathiness(sing, 10 - (10 * smoothedAccelY));
                singSetBrightness(sing, (smoothedAccelY * 2 - 1));
                if(pitchIndex >= 0)
                singSetPitch(sing, pitches[pitchIndex] + ((smoothedAccelZ - prevSmoothedAccelZ) * 2), NO);
                break;
            }
        }
      usleep(100);
    }
  return 0;
}

/*------------------------------------------------------------------------------------*/
void allophoneWasSet(void* self, Allophone* allo)
{
  fprintf(stderr, "%s\n\r", alloSymbol(allo));
}

/*------------------------------------------------------------------------------------*/
void queueEmptied(void* self, Allophone* allo)
{
  singEnqueueAllophones(sing, text);
}

/*------------------------------------------------------------------------------------*/
//#include <IOKit/serial/ioss.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
struct termios oldTerminalAttributes;

void iHateCanonicalInputProcessingIReallyReallyDo(void)
{
	int error;
	struct termios newTerminalAttributes;
  
	int fd = fcntl(STDIN_FILENO,  F_DUPFD, 0);
  
	error = tcgetattr(fd, &(oldTerminalAttributes));
	if(error == -1) {  fprintf(stderr, "Error getting serial terminal attributes\n"); return;}
	
	newTerminalAttributes = oldTerminalAttributes; 
	
	cfmakeraw(&newTerminalAttributes);
	
	error = tcsetattr(fd, TCSANOW, &newTerminalAttributes);
	if(error == -1) {  fprintf(stderr,  "Error setting serial attributes\n"); return; }
}

/*------------------------------------------------------------------------------------*/

void makeStdinCannonicalAgain()
{
  int fd = fcntl(STDIN_FILENO,  F_DUPFD, 0);
  
	if (tcsetattr(fd, TCSANOW, &oldTerminalAttributes) == -1)
	  fprintf(stderr,  "Error setting serial attributes\n");
}
