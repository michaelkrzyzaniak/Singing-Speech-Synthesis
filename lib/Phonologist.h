#ifndef __PHONOLOGIST__
#define __PHONOLOGIST__ 1

#if defined(__cplusplus)
extern "C"{
#endif   //(__cplusplus)

#include "Allophone.h"
#include "Hashtable.h"

typedef struct  OpaquePhonologistStruct Phonologist;

Phonologist*   phonNew                 (char* pathOfRecordings, FFT* fft, double sampleRate);
Phonologist*   phonDestroy             (Phonologist* self);

Allophone*     phonAllophoneForSymbol  (Phonologist* self, alloSymbol_t symbol);
char*          phonPathToRecordings    (Phonologist* self);
int            phonNumAllophones       (Phonologist* self);
int            phonAllophoneSymbols    (Phonologist* self, alloSymbol_t returnedSymbols[]);
void           phonDebugPrintAllophones(Phonologist* self);

#if defined(__cplusplus)
}
#endif   //(__cplusplus)

#endif   //__PHONOLOGIST__
