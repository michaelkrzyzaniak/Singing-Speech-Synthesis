#include "MIDINotes.h"


/*--------------------------------------------------------------------------*/
#ifdef AMARILLI
//prendi questo mio strale
char* text = "a-|m|a-|r|i-|l|i-|m|i|a-| |b|e-|l|a-| -|n|n|a-|n|n|k|r|e-|d|i|o-|d|e-|l|m|i-|k|o-|r| -|"
             "n|d|o-|l|c|e-|d|e-|z|i-|o-| -|d|e-|s|e-|r|t|u-| -|l|a-|m|o-|r|m|i-|o-| -|"
             "k|r|e-|d|i-|l|o-|p|u-|r| -|e-|s|e-|t|i-|m|o-|r|t|a-|s|a-|a-|l|e-| -|"
             "p|r|e-|n|d|i-|k|u|e-|s|t|o-|m|i|o-|s|t|r|a-|a-|l|e-| -|a-|p|r|i-|m|i-|l|p|e-|t|o-| -|"
             "e-|v|e-|d|r|a-|i|s|k|r|i-|t|o|i-|n|k|o-|r|e-| -|"
             "a-|m|a-|r|i-|i-|i-|i-|l|i-| -|"
             "a-|m|a-|r|i-|i-|i-|i-|i-|i-|i-|l|i-| -|"
             "a-|m|a-|r|i-|l|i-| -|e-|l|m|i|a-|m|o-|o-|r|e-| -|"
             "k|r|e-|d|i-|l|o-|p|u-|r| -|e-|s|e-|t|i-|m|o-|r|t|a-|s|a-|a-|l|e-| -|"
             "p|r|e-|n|d|i-|k|u|e-|s|t|o-|m|i|o-|s|t|r|a-|a-|l|e-| -|a-|p|r|i-|m|i-|l|p|e-|t|o-| -|"
             "e-|v|e-|d|r|a-|i|s|k|r|i-|t|o|i-|n|k|o-|r|e-| -|"
             "a-|m|a-|r|i-|i-|i-|i-|l|i-| -|"
             "a-|m|a-|r|i-|i-|i-|i-|i-|i-|i-|l|i-| -|"
             "a-|m|a-|r|i-|l|i-| -|e-|l|m|i|a-|m|o-|o-|r|e-| -|"
             "a-|m|a-|r|i-|i-|i-|i-|i-|l|i-| -|"
             "e-|l|m|i|a-|m|o-|o-|o-|o-|o-|o-|o-|o-|o-|o-|o-|o-|o-|o-|o-|r|e-| -|";

int pitches[] = {/*amarilli*/m_D4 , m_A3 , m_Bb3, m_Fs3, m_G3 , m_A3 , m_A3 , m_A3 , m_D4 , m_C4 , m_A3, m_C4 , m_Bb3, m_A3, m_A3,
                 /**dolce*/  m_D4 , m_C4 , m_Bb3, m_C4 , m_Bb3, m_Bb3, m_D4 , m_C4 , m_Bb3, m_Bb3, m_A3, m_A3 , m_A3 , m_G3, m_G3,
                 /*credilo*/ m_Bb3, m_A3 , m_G3 , m_A3 , m_Fs3, m_Fs3, m_Fs3, m_G3 , m_A3 , m_Fs3, m_G3, m_A3 , m_A3 , m_A3,
                 /*prendi*/  m_Bb3, m_G3 , m_A3 , m_G3 , m_Fs3, m_G3 , m_A3 , m_A3 , m_A3 , m_D4 , m_C4, m_Bb3, m_A3 , m_A3, m_Fs3, 
                 /*e vedrai*/m_Fs3, m_Fs3, m_G3 , m_A3 , m_F3 , m_E3 , m_D3 , m_D3 ,
                 /*amarilli*/m_Fs3, m_Fs3, m_G3 , m_Fs3, m_G3 , m_A3 , m_Fs3, m_Fs3, 
                 /*amarilli*/m_A3 , m_A3 , m_B3 , m_C4 , m_B3 , m_C4 , m_B3 , m_A3 , m_B3 , m_C4 , m_C4, 
                 /*amarilli*/m_Cs4, m_Cs4, m_D4 , m_D4 , m_Bb3, m_Bb3, m_G3 , m_A3 , m_G3 , m_G3 , m_G3,
                 /*credilo*/ m_Bb3, m_A3 , m_G3 , m_A3 , m_Fs3, m_Fs3, m_Fs3, m_G3 , m_A3 , m_Fs3, m_G3, m_A3 , m_A3 , m_A3,
                 /*prendi*/  m_Bb3, m_G3 , m_A3 , m_G3 , m_Fs3, m_G3 , m_A3 , m_A3 , m_A3 , m_D4 , m_C4, m_Bb3, m_A3 , m_A3, m_Fs3, 
                 /*e vedrai*/m_Fs3, m_Fs3, m_G3 , m_A3 , m_F3 , m_E3 , m_D3 , m_D3 ,
                 /*amarilli*/m_Fs3, m_Fs3, m_G3 , m_Fs3, m_G3 , m_A3 , m_Fs3, m_Fs3, 
                 /*amarilli*/m_A3 , m_A3 , m_B3 , m_C4 , m_B3 , m_C4 , m_B3 , m_A3 , m_B3 , m_C4 , m_C4, 
                 /*amarilli*/m_Cs4, m_Cs4, m_D4 , m_D4 , m_Bb3, m_Bb3, m_G3 , m_A3 , m_G3 , m_G3 , m_G3,
                 /*coda*/    m_D4 , m_D4 , m_E4 , m_D4 , m_C4 , m_B3 , m_C4 , m_D4 , m_C4 , 
                 /*eilmio*/  m_C4 , m_B3, m_B3, m_C4, m_B3, m_C4, m_D4, m_B3, m_C4, m_A3, m_C4, m_B3, m_A3, m_B3, m_G3, m_A3, m_G3, m_G3, m_G3
                 };

/*
int pitches[] = {m_E3, m_Gs3, m_A3, m_B3, m_E4, m_Gs4, m_A4, m_B4, m_E4};
char* text = "wisnaaitisaaarson";
*/

  
#endif //AMARILLI

