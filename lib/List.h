/*_--__-_--___-----_-------_------------_-_-----_-----_-----_----------------------------
|  \/  | |/ / |	  (_)_ __ | | _____  __| | |   (_)___| |_  | |__  
| |\/| | ' /| |	  | | '_ \| |/ / _ \/ _` | |   | / __| __| | '_	\ 
| |  | | . \| |___| | |	| |   <	 __/ (_| | |___| \__ \ |_ _| | | |
|_|  |_|_|\_\_____|_|_|	|_|_|\_\___|\__,_|_____|_|___/\__(_)_| |_|

---------------------------------------------------------------------------------------*/
  
#ifndef __MK_LINKED_LIST__
#define __MK_LINKED_LIST__
  
/*---------_---------_-------------------------------------------------------------------
(_)_ _ 	__| |_ 	_ __| |___ ___
| | ' \/ _| | || / _` /	-_|_-<
|_|_||_\__|_|\_,_\__,_\___/__/
---------------------------------------------------------------------------------------*/
  
#include <stdlib.h>

#ifndef bool
#define bool int  
#endif

#define NO  0  
#define YES (!NO)

/*---------------------------------------------------------------------------------------
| |_ _ 	_ _ __ 	___ ___
|  _| || | '_ \/ -_|_-<
 \__|\_, | .__/\___/__/
     |__/|_|   	       
---------------------------------------------------------------------------------------*/
#if defined(__cplusplus)
extern "C"{
#endif   //(__cplusplus)

//---------------------------------------------------------------------------------------
typedef void* listData_t;
typedef void* (*listDataDeallocator_t) (listData_t);

//---------------------------------------------------------------------------------------
typedef struct
{ 
  void*      prev;
  listData_t data;
  void*      next;
  listDataDeallocator_t deallocateData;
}listEntry_t;
  
//---------------------------------------------------------------------------------------
typedef struct
{ 
  int count;
  listEntry_t         *firstEntry;
  listEntry_t         *currentEntry;
  listEntry_t         *lastEntry;
}List;
  
  
  
/*------------_-------_------------------------------------------------------------------
 _ __ _	_ ___| |_ ___| |_ _  _ _ __  ___ ___
| '_ \ '_/ _ \ 	_/ _ \ 	_| || |	'_ \/ -_|_-<
| .__/_| \___/\__\___/\__|\_, |	.__/\___/__/
|_|    	       	       	  |__/|_|      	    
---------------------------------------------------------------------------------------*/
  
  
  
//---------------------------------------------------------------------------------------
/*  create an empty list                                                               */

List*         listNew               ();


//---------------------------------------------------------------------------------------
/*  returns the number of entries in the list                                          */

int           listCount              (List* self);


//---------------------------------------------------------------------------------------
/*  appends data to the end of list                                                    */

void          listAppendData         (List* self, listData_t data, listDataDeallocator_t deallocator);


//---------------------------------------------------------------------------------------
/*  puts data into list at index                                                       */

void          listInsertDataAtIndex  (List* self, listData_t data, int index, listDataDeallocator_t deallocator);


//---------------------------------------------------------------------------------------
/*  remove the data at the specified index                                             */

void          listRemoveDataAtIndex  (List* self, int index, bool shouldDeallocate);


//---------------------------------------------------------------------------------------
/*  returns the data in the list at the given index                                    */

listData_t    listDataAtIndex        (List* self, int index);


//---------------------------------------------------------------------------------------
/*  replaces the data in the list at the given index with data                         */

void          listReplaceDataAtIndex (List* self, listData_t data, int index,  bool shouldDeallocate);


//---------------------------------------------------------------------------------------
/*  removes and destroys all of the entrise in the list                                */

void          listClear              (List* self,  bool shouldDeallocate);


//---------------------------------------------------------------------------------------
/*  clears the list, and frees it and all of its resources                             */

void          listDestroy            (List* self, bool shouldDeallocate);


//---------------------------------------------------------------------------------------
/*  set the list's data deallocator. The data deallocater is called whenever           */
/*  data is removed from the list (ie listClear(), listRemoveEntryAtIndex()            */
/*  listDestroy() listReplaceDataAtIndex() etc...). The default data deallocator       */
/*  simply calls free(listData), so if your data was allocated with a single call to   */
/*  malloc() or calloc() then you do not need to worry about this. If, however, you are*/
/*  using a more complicated data type, then you need to write a function that returns */ 
/*  void, takes listData_t as an argument, and destroys the listData. You must then    */
/*  use this method to tell the list what function to call when it wants to free data  */

//void          listSetDataDeallocator (List* list, listDataDeallocator_t deallocator); 


//---------------------------------------------------------------------------------------
/*  resets the iterator. returns 1 if there is data, otherwise 0. see                  */
/*  listAdvanceIterator() for a discussion of proper use.                              */

int           listResetIterator      (List* self); 


//---------------------------------------------------------------------------------------
/*  advance the iterator. returns 1 until there is no more data, then returns 0        */
/*  correct usage is as follows:                                                       */
/*  if(listResetIterator(list))                                                        */
/*    {                                                                                */
/*       do{                                                                           */
/*            //do something to the current data, which can be obtained with           */
/*            //listCurrentData(list);                                                 */
/*            //or set with                                                            */
/*            //listSetCurrentData(list, data)                                         */
/*         }while(listAdvanceIterator(list));                                          */
/*    }                                                                                */
/*                                                                                     */
/*  This is much, much more efficient than calling listDataAtIndex() for each          */
/*  entry in the list.                                                                 */

int           listAdvanceIterator    (List* self); 


//---------------------------------------------------------------------------------------
/*  get the data that is currently pointed to by the iterator. see                     */
/*  listAdvanceIterator() for a discussion of proper use.                              */
 
listData_t    listCurrentData        (List* self);


//---------------------------------------------------------------------------------------
/*  set the data that is currently pointed to by the iterator. see                     */
/*  listAdvanceIterator() for a discussion of proper use.                              */

//void          listSetCurrentData     (List* self, listData_t data);


//---------------------------------------------------------------------------------------
#if defined(__cplusplus)
}
#endif   //(__cplusplus)

#endif   //MK_LINKED_LIST

