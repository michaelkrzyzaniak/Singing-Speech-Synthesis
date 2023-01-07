/*_--__-_--___-----_-------_------------_-_-----_-----_----------------------------------
|  \/  | |/ / |	  (_)_ __ | | _____  __| | |   (_)___| |_   ___	
| |\/| | ' /| |	  | | '_ \| |/ / _ \/ _` | |   | / __| __| / __|
| |  | | . \| |___| | |	| |   <	 __/ (_| | |___| \__ \ |_ | (__	
|_|  |_|_|\_\_____|_|_|	|_|_|\_\___|\__,_|_____|_|___/\__(_)___|

---------------------------------------------------------------------------------------*/
  
#include "List.h"
  
/*--------_----------_------------_--------_---------------_---_-------------------------
 _ __ _	_(_)_ ____ _| |_ ___   __| |___	__| |__	_ _ _ __ _| |_(_)___ _ _  ___
| '_ \ '_| \ V / _` |  _/ -_) /	_` / -_) _| / _` | '_/ _` |  _|	/ _ \ '	\(_-<
| .__/_| |_|\_/\__,_|\__\___| \__,_\___\__|_\__,_|_| \__,_|\__|_\___/_||_/__/
|_|    	       	       	       	       	       	       	       	       	     
---------------------------------------------------------------------------------------*/
listEntry_t*      listNewEntry                ();
listEntry_t*      listEntryAtIndex            (List* self, int index);
void              listRemoveEntryAtIndex      (List* self, int index, bool shouldDeallocate);
void              listCleanEntry              (List* self, listEntry_t* entry);
//void              listDefaultDataDeallocator(listData_t data);

/*------------_---_----------------------------------------------------------------------
 _ _ ___ _  _| |_(_)_ _	 ___ ___
| '_/ _	\ || | 	_| | ' \/ -_|_-<
|_| \___/\_,_|\__|_|_||_\___/__/
---------------------------------------------------------------------------------------*/

listEntry_t* listNewEntry()
{
  return (listEntry_t*)malloc(sizeof(listEntry_t));
}      

//entryAtIndex---------------------------------------------------------------------------
listEntry_t* listEntryAtIndex(List* self, int index)
{
  listEntry_t* thisEntry = self->firstEntry;
  if(thisEntry != NULL)
    {
      int maxIndex = listCount(self)-1;
      if(index > maxIndex) index = maxIndex;
   
      while(index-- > 0)
        thisEntry = thisEntry->next;
    }
  return thisEntry;
}

//cleanEntry-----------------------------------------------------------------------------
void listCleanEntry(List* self, listEntry_t* entry)
{
  if(entry->deallocateData != NULL)
    entry->deallocateData(entry->data);
  //free(entry->data) if it was allocated with malloc;
}

//removeEntryAtIndex---------------------------------------------------------------------
void listRemoveEntryAtIndex(List* self, int index, bool shouldDeallocate)
{
  if(index < listCount(self))
    {
      listEntry_t* thisEntry = listEntryAtIndex(self, index);
      if (shouldDeallocate) listCleanEntry(self, thisEntry);
      if(thisEntry->prev != NULL)
        ((listEntry_t*)thisEntry->prev)->next = thisEntry->next;
      else
        self->firstEntry = (listEntry_t*)thisEntry->next;
      if(thisEntry->next != NULL)
        ((listEntry_t*)thisEntry->next)->prev = thisEntry->prev;
      else self->lastEntry = thisEntry->prev;
      free(thisEntry);
      self->count--;
    }
}

//listCreate-----------------------------------------------------------------------------
List* listNew()
{
  List* self = malloc(sizeof(*self));
  if(self != NULL)
    {
      self->firstEntry = NULL;
      self->currentEntry = NULL;
      self->lastEntry = NULL;
      self->count = 0;
     //listSetDataDeallocator(list, listDefaultDataDeallocator);
    }
  return self;
}

//count----------------------------------------------------------------------------------
int listCount(List* self)
{
  return self->count;
}

//appendData-----------------------------------------------------------------------------
void listAppendData(List* self, listData_t data, listDataDeallocator_t deallocator)
{
  listEntry_t* thisEntry = listNewEntry();
  if(thisEntry != NULL)
    {
      if(self->firstEntry == NULL) self->firstEntry = thisEntry; 
      listEntry_t* prevEntry = self->lastEntry;
      thisEntry->prev = prevEntry;
      if(prevEntry != NULL)prevEntry->next = thisEntry;
      self->lastEntry = thisEntry;
    
      thisEntry->next = NULL;
      thisEntry->data = data;
      thisEntry->deallocateData = deallocator;
      self->count++;
    }
}

//insertDataAtIndex----------------------------------------------------------------------
void listInsertDataAtIndex(List* self, listData_t data, int index, listDataDeallocator_t deallocator)
{
  if(index >= listCount(self)) 
    listAppendData(self, data, deallocator);
  else
    {
      listEntry_t* thisEntry = listNewEntry();
      if(thisEntry != NULL)
        {
          listEntry_t* nextEntry = listEntryAtIndex(self, index);
          listEntry_t* prevEntry = nextEntry->prev;
          
          thisEntry->next     = nextEntry;
          thisEntry->prev = prevEntry;
          
          if(prevEntry != NULL)
            prevEntry->next = thisEntry;
          if(nextEntry != NULL)
	          nextEntry->prev = thisEntry;
          if(index == 0) 
	          self->firstEntry = thisEntry;
            
          thisEntry->data = data;
          thisEntry->deallocateData = deallocator;
          self->count++;
        }
    }
}

//removeDataAtIndex----------------------------------------------------------------------
void listRemoveDataAtIndex(List* self, int index, bool shouldDeallocate)
{
  listRemoveEntryAtIndex(self, index, shouldDeallocate);
}

//dataAtIndex----------------------------------------------------------------------------
listData_t listDataAtIndex(List* self, int index)
{
  return listEntryAtIndex(self, index)->data;
}

//replaceDataAtIndex---------------------------------------------------------------------
void listReplaceDataAtIndex(List* self, listData_t data, int index, bool shouldDeallocate)
{
  if(index<listCount(self))
    {
      listEntry_t* thisEntry = listEntryAtIndex(self, index);
      if(shouldDeallocate)
        listCleanEntry(self, thisEntry);
      thisEntry->data = data;
    }
}

//resetIterator--------------------------------------------------------------------------
int listResetIterator(List* self)
{
  self->currentEntry = self->firstEntry;
  return (self->currentEntry == NULL) ? 0 : 1;
}

//advanceIterator------------------------------------------------------------------------
int listAdvanceIterator(List* self)
{
  self->currentEntry = (listEntry_t*)(self->currentEntry->next);
  return (self->currentEntry == NULL) ? 0 : 1;
}

//currentData----------------------------------------------------------------------------
listData_t listCurrentData(List* self)
{
  return self->currentEntry->data;
}

//setCurrentData-------------------------------------------------------------------------
/*
void listSetCurrentData(List* self, listData_t data, boolShouldDeallocate)
{
  if(shouldDeallocate)
    listCleanEntry(self, self->currentEntry);
  self->currentEntry->data = data;
}
*/

//clear----------------------------------------------------------------------------------
void listClear(List* self, bool shouldDeallocate)
{
  while(listCount(self))
      listRemoveEntryAtIndex(self, 0, shouldDeallocate);
}

//defaultDataDeallocator-----------------------------------------------------------------
void listDefaultDataDeallocator(listData_t data)
{
  if(data != NULL)
    free(data);
}

//setDataDeallocator---------------------------------------------------------------------
/*
void listSetDataDeallocator (List* self, listDataDeallocator_t deallocator)
{
  self->deallocateData = deallocator;
}
*/
//listDestroy----------------------------------------------------------------------------
void listDestroy(List* self, bool shouldDeallocate)
{
  if(self != NULL)
  {
    listClear(self, shouldDeallocate);
    free(self);
  }
}

