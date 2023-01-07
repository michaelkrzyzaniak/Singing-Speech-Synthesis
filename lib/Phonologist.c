#include "Phonologist.h"
#include "glob.h"
#include "Util.h"

//a singleton associated with each path to allophone data
List* PhonologistClassSingletons = NULL;

Phonologist*   phonPrivateDestroy(Phonologist* self);


/*OpaquePronunciationStruct--------------------------------------*/
struct OpaquePhonologistStruct
{
  Hashtable* hashtable;
  char* pathToRecordings;
  int   referenceCount;
};

/*-----------------------------------------------------------*/
Phonologist*   phonNew(char* pathOfRecordings, FFT* fft, double sampleRate)
{
  Phonologist* self = NULL;
  glob_t g;
  char path [strlen(pathOfRecordings) + 7];
  strcpy(path, pathOfRecordings);
  strcat(path, "*.aif*");

  if(PhonologistClassSingletons == NULL)
    {
      PhonologistClassSingletons = listNew();
      if(PhonologistClassSingletons == NULL)
        return phonPrivateDestroy(self);
    }
  else
    {
      Phonologist* singleton = NULL;
      if(listResetIterator(PhonologistClassSingletons))
        do{
            singleton = listCurrentData(PhonologistClassSingletons);
            if(strcmp(phonPathToRecordings(singleton), pathOfRecordings) == 0)
              {
                self = singleton;
                self->referenceCount++;
                break;
              }
          }while(listAdvanceIterator(PhonologistClassSingletons));
    }
  if(self != NULL)
    return self;
  
  else if(glob(path, 0, NULL, &g) == 0)
    {
      self = (Phonologist*) calloc(1, sizeof(*self));
      if(self != NULL)
        {
          int i;
          MKAiff *aiff;
          Allophone* allo;
          self->referenceCount = 1;
          self->hashtable = hashNew(g.gl_pathc * 1.3);
          if(self->hashtable == NULL)
            return phonPrivateDestroy(self);
            
          i = strlen(pathOfRecordings) + 1;
          self->pathToRecordings = malloc(i);
          
          if(self->pathToRecordings == NULL)
            return phonPrivateDestroy(self);
          memcpy(self->pathToRecordings, pathOfRecordings, i);
          
          for(i=0; i<g.gl_pathc; i++)
	          {
              aiff = aiffWithContentsOfFile(g.gl_pathv[i]);
              if(aiff != NULL)
                {
                  aiffMakeMono(aiff);
                  if(aiffResample(aiff, sampleRate, aiffInterpLinear) == aiffNo)
                    return phonPrivateDestroy(self);

                  allo = alloNew(basename(g.gl_pathv[i]), aiff, fft);

                  if(allo != NULL)
                    hashStoreValueForKey(self->hashtable, alloSymbol(allo), allo, (hashValueDeallocator_t) alloDestroy);

                  aiff = aiffDestroy(aiff);
                  
                  listAppendData(PhonologistClassSingletons, self, (listDataDeallocator_t)phonPrivateDestroy);
                }
	          }
        }
      else return (phonPrivateDestroy(self));
    }
    
  return self;
}

/*-----------------------------------------------------------*/
Phonologist*   phonDestroy        (Phonologist* self)
{
  if(self != NULL)
    {
      if(--(self->referenceCount) <= 0)
        {
          Phonologist* singleton = NULL;
          if(listResetIterator(PhonologistClassSingletons))
            {
              int i = 0;
              do{
                  singleton = listCurrentData(PhonologistClassSingletons);
                  if(self == singleton)
                    {
                      listRemoveDataAtIndex(PhonologistClassSingletons, i, YES);
                      if(listCount(PhonologistClassSingletons) == 0)
                        listDestroy(PhonologistClassSingletons, YES);
                      break;
                    }
                  i++;
                }while(listAdvanceIterator(PhonologistClassSingletons));
            }
        }
    }
    return (Phonologist*) NULL;
}

/*proDestroy-------------------------------------------------*/
Phonologist*   phonPrivateDestroy(Phonologist* self)
{
  if(self != NULL)
    {
      if(--(self->referenceCount) <= 0)
        {
          if(self->hashtable != NULL)
            free(self->hashtable);
          if(self->pathToRecordings != NULL)
            free(self->pathToRecordings);
          
          free(self);
        }
    }
  return (Phonologist*)NULL;
}

/*-----------------------------------------------------------*/
int          phonNumAllophones         (Phonologist* self)
{
  return hashCount(self->hashtable);
}

/*proAllophoneForSymbol----------------------------------------*/
Allophone*       phonAllophoneForSymbol   (Phonologist* self, alloSymbol_t symbol)
{
  return (Allophone*)hashGetValueForKey(self->hashtable, symbol);
}

/*-----------------------------------------------------------*/
char*       phonPathToRecordings   (Phonologist* self)
{
  return self->pathToRecordings;
}

/*-----------------------------------------------------------*/
int          phonAllophoneSymbols      (Phonologist* self, alloSymbol_t returnedSymbols[])
{
  return hashGetAllKeys(self->hashtable, returnedSymbols);
}

/*-----------------------------------------------------------*/
void         phonDebugPrintAllophones  (Phonologist* self)
{
  int numSymbols = hashCount(self->hashtable);
  char* symbols[numSymbols];
  char** s = symbols;  
  phonAllophoneSymbols(self, symbols);
  while(numSymbols--)
    printf("%s\n", *s++);
}







