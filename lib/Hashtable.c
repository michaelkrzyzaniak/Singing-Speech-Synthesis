#include "Hashtable.h"
#include <stdio.h> //for debugging only
#include <string.h>

uint32_t hash_hash(char* str);


/*HashEntryStruct--------------------------------------------*/
typedef struct opaque_hash_entry_struct
{
  hashKey_t   key;
  hashValue_t value;
}hashEntry_t;

/*hashNewEntry-----------------------------------------------*/
hashEntry_t* hashNewEntry(hashKey_t key, hashValue_t value)
{
  hashEntry_t* entry = malloc(sizeof(*entry));
  if(entry != NULL)
    {
      entry->key   = key;
      entry->value = value;
    }
  return entry;
}

/*hashNewEntry-----------------------------------------------*/
//entry->value will already have been destroyed if it was supposed to be
void* hashDestroyEntry(hashEntry_t* entry)
{
  if(entry != NULL)
    free(entry);
  return NULL;
}

/*OpaqueHashtableStruct--------------------------------------*/
struct OpaqueHashtableStruct
{
  List** entries;
  int capacity;
  int count;
};

/*hashNew----------------------------------------------------*/
Hashtable* hashNew(int capacity)
{
  
  Hashtable* self = (Hashtable*) calloc(sizeof(*self), 1);
  if(self != NULL)
    {
      self->capacity = capacity;
      self->entries = (List**) calloc(capacity, sizeof(List*));
      if(self->entries == NULL) self = hashDestroy(self, NO);
    }
  return self;
}

/*hashDestroy------------------------------------------------*/
Hashtable* hashDestroy(Hashtable* self, bool shouldDeallocate)
{
  if(self != NULL)
    {
      if(self->entries != NULL)
        {
          int i;
          for(i=0; i<self->capacity; i++)
            if(self->entries[i] != NULL)
              listDestroy(self->entries[i], shouldDeallocate);
          free(self->entries);
        }
        
      free(self);
    }
  return (Hashtable*)NULL;
}

/*hashStoreValueForKey---------------------------------------*/
void hashStoreValueForKey (Hashtable* self, hashKey_t key, hashValue_t value, hashValueDeallocator_t valueDeallocator)
{
  uint32_t h = hash_hash(key) % self->capacity;
  if(self->entries[h] == NULL)
    {
      self->entries[h] = listNew();
      if(self->entries[h] == NULL) return; //error, could not create list
    }
    
  hashEntry_t* entry = hashNewEntry(key, value);
  if(entry == NULL) return; //error, could not create entry
  
  listAppendData(self->entries[h], entry, (listDataDeallocator_t)hashDestroyEntry);
  self->count++;
}

/*hashGetValueForKey-----------------------------------------*/
hashValue_t hashGetValueForKey (Hashtable* self, hashKey_t key)
{
  uint32_t h = hash_hash(key) % self->capacity;
  hashValue_t value = NULL;
  List* list = self->entries[h];
  hashEntry_t* entry;
  
  if(list != NULL)
    {
      if(listResetIterator(list))
        {
         do{
             entry = (hashEntry_t*) listCurrentData(list);

             if(strcmp(key, entry->key) == 0)
               {
                 value = entry->value;
                 break;
               }
           }while(listAdvanceIterator(list));
      }
    }
  return value;
}

/*hashRemoveValueForKey--------------------------------------*/
hashValue_t hashRemoveValueForKey (Hashtable* self, hashKey_t key, bool shouldDeallocate)
{
  /*not important at the moment*/
  return 0;
}

/*hashPrintCollisionData-------------------------------------*/
int         hashCount                 (Hashtable* self)
{
  return self->count;
}

/*hashPrintCollisionData-------------------------------------*/
int        hashGetAllKeys               (Hashtable* self, char* returnedKeys[])
{
  int i, n=0;
  hashEntry_t* entry;
  List* list;
  
  for(i=0; i<self->capacity; i++)
    {
      list = self->entries[i];
      if(list != NULL)
        {
          if(listResetIterator(list))
            {
             do{
                entry = (hashEntry_t*) listCurrentData(list);
                *returnedKeys++ = entry->key;
                n++;
               }while(listAdvanceIterator(list));
            }
        }
    } 
  return n;
}

/*hashPrintCollisionData-------------------------------------*/
void hashPrintCollisionData(Hashtable* self)
{
  int i, capacity = self->capacity;
  int numFilledEntries = 0;
  int numVacantEntries = 0;
  int numCollisions    = 0;
  
  for(i=0; i<capacity; i++)
    {
      if(self->entries[i] == NULL)
        numVacantEntries++;
      else
        {
          numFilledEntries++;
          numCollisions += listCount(self->entries[i]) - 1;
        }
    }
  printf("capacity: %i, numFilledEntries: %i, numVacantEntries: %i, numCollisions: %i\n", capacity, numFilledEntries, numVacantEntries, numCollisions);
}

/*hash-------------------------------------------------------*/
uint32_t hash_hash(char* str)
{
	uint32_t hash = 5381;
	while (*str != '\0')
		hash = hash * 33 ^ (unsigned int)*str++;
	return hash;
}
