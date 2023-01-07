#include "../lib/MKAiff.h"
#include "../lib/Allophone.h"
#include <stdio.h>

char qed();
void help();

/*------------------------------------------------------*/
int main(int argc, char* argv[])
{
  if(argc < 2) {help(); exit(-1);}
    
  --argc, ++argv;
  
  MKAiff* aiff = aiffWithContentsOfFile(*argv);
  if(aiff == NULL)
    printf("%s is not a valid AIFF file\n\r", *argv);
  else
    {
      //char* noisiness;
      //float noiseFloat;
      char* category;
      char  categoryStr[10];
      int   midiNumber;
      int   detune;
      int   needsSave = 0;
      char  c;

      for(;;)
        {
          printf("enter 1 2 s or q:\n\r");
          printf("1) edit MIDI base note\n\r");
          printf("2) edit Phoneme Category\n\r");
          //printf("3) edit Noisiness\n\r");
          printf("s) save \n\r");
          printf("q) quit \n\r");

          scanf("%c", &c);
          switch(c)
            {
              case '1':
                if(aiffHasInstrumentInfo(aiff))
                  printf("current MIDI base note is: %i detune: %i\n\r", (int)aiffInstrumentBaseNote(aiff), (int)aiffInstrumentDetune(aiff));
                else
                  printf("current MIDI base note is: UNDEFINED\n\r");

                switch(qed())
                  {
                    case 'e':
                      needsSave = 1;
                      printf("enter a new MIDI number (0 ~ 127 integer): ");
                      scanf("%i\n\r", &midiNumber);
                      printf("enter a detune amount in cents (-50 ~ 50 integer): ");
                      scanf("%i\n\r", &detune);
                      aiffSetupInstrumentInfo(aiff, midiNumber, detune, 0, 127, 0, 127, 0, AIFF_LOOP_MODE_OFF, 0, 0, AIFF_LOOP_MODE_OFF, 0, 0);
                      break;
                    case 'd':
                      needsSave = 1;
                      aiffRemoveInstrumentInfo(aiff);
                      break;
                    case 'q': /*cascade*/
                    default: 
                      break;
                  }
                break;
              case '2':
                if(aiffNameOfMarkerWithID(aiff, ALLO_CATEGORY_AIFF_MARKER_ID, &category) == aiffYes)
                  printf("current phoneme category is: %i\n\r", atoi(category));
                else
                  printf("current phoneme category is: UNDEFINED\n\r");
                  
                switch(qed())
                  {
                    case 'e':
                      needsSave = 1;
                      printf("All markers removed. Quit without saving if you dont like it\n\r");
                      aiffRemoveAllMarkers(aiff);
                      printf("enter a new category (1=vowel | 2=fricative | 3=plosive): ");
                      scanf("%s\n\r", categoryStr);
                      aiffAddMarkerWithPositionInSamples(aiff, categoryStr, ALLO_CATEGORY_AIFF_MARKER_ID, 0);
                      break;
                    case 'd':
                      needsSave = 1;
                      printf("All markers removed. Exit without saving if you dont like it\n\r");
                      aiffRemoveAllMarkers(aiff);
                      break;
                    case 'q': /*cascade*/
                    default: 
                      break;
                  }
                
                break;
              case '3':
                break;
              case 's':
                aiffSaveWithFilename(aiff, *argv);
                needsSave = 0;
                break;
              case 'q':
                if(needsSave)
                  {
                    printf("There are unsaved changes. Save? (y or n)\n\r");
                    scanf("%c\n\r", &c);
                    if(c == 'y')
                      {
                        aiffSaveWithFilename(aiff, *argv);
                        printf("saved\n\r");
                      }
                    else printf("not saved\n\r");
                  }
                goto cleanup;
                break;
              default:
                break;
            }
        }
      cleanup: aiffDestroy(aiff);
    }

  return 0;
}



/*------------------------------------------------------*/
char qed()
{
  char c;
  printf("enter e d or q:\n\r");
  printf("e) edit\n\r");
  printf("d) delete\n\r");
  printf("q) quit \n\r");
  scanf("%c", &c);
  return c;
}


/*------------------------------------------------------*/
void help()
{
  printf("which AIFF file would you like to edit?\n\r");
}