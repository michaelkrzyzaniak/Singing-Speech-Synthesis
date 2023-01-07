#ifndef __HASHTABLE__
#define __HASHTABLE__ 1

#if defined(__cplusplus)
extern "C"{
#endif   //(__cplusplus)

#include "List.h"
#include "stdint.h"
#include "stdlib.h"

typedef struct  OpaqueHashtableStruct Hashtable;
typedef char*                 hashKey_t;
typedef listData_t            hashValue_t;
typedef listDataDeallocator_t hashValueDeallocator_t;

Hashtable*  hashNew                   (int    capacity);
Hashtable*  hashDestroy               (Hashtable* self, bool shouldDeallocate);
void        hashStoreValueForKey      (Hashtable* self, hashKey_t key, hashValue_t value, hashValueDeallocator_t valueDeallocator);
hashValue_t hashGetValueForKey        (Hashtable* self, hashKey_t key);
hashValue_t hashRemoveValueForKey     (Hashtable* self, hashKey_t key, bool shouldDeallocate);
int         hashCount                 (Hashtable* self);
int         hashGetAllKeys            (Hashtable* self, char* returnedKeys[]);

void        hashPrintCollisionData    (Hashtable* self);

#if defined(__cplusplus)
}
#endif   //(__cplusplus)

#endif   // __HASHTABLE__
