/*!
 * @header Wifly
 * @abstract Interface with the wireless communication
 *  hardware
 *
 * @author
 *  Written by Michael Krzyzaniak at Arizona State 
 *  University's School of Arts, Media + Engineering
 *  in Spring of 2013.
 *
 *  mkrzyzan@asu.edu
 *
 * @unsorted
 */
/*-----------------------------------------------------*/

#ifndef  __MK_WIFLY__     
#define  __MK_WIFLY__ 1 

#ifdef __cplusplus            
extern "C" {                
#endif 


/*-----------------------------------------------------*/
/*! 
 * @function wiSetup
 * @abstract 
 */ 
void wiSetup();

/*-----------------------------------------------------*/
/*! 
 * @function wiWrite
 * @abstract 
 */
int wiWrite(char* buffer, int bufferSize);

/*-----------------------------------------------------*/
/*! 
 * @function wiRead
 * @abstract 
 */
int wiRead(char* buffer, int bufferSize, int minBytes);


#ifdef __cplusplus            
}                             
#endif

#endif//__MK_WIFLY__
