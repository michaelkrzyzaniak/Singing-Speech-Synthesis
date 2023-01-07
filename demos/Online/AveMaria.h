#include "MIDINotes.h"

//#define AVE_SOPRANO
//#define AVE_ALTO
//#define AVE_TENOR
//#define AVE_BASS

/*--------------------------------------------------------------------------*/
#ifdef AVE_SOPRANO
char* text = "avemariia,"
             "graatsiapleeeeeena,"
             "daminusteekuuumteeeeeeekum,"
             "virgosereena,seereena,";

int pitches[] = 
  {
  /*ave   */ m_G4, m_C5, m_C5, m_D5, m_E5, m_C5, 0,
  /*gratia*/ m_C5, m_B4, m_A4, m_G4, m_A4, m_G4, m_C5, m_B4, m_A4, m_B4, m_C5, 0, 
  /*dominu*/ m_C5, m_C5, m_C5, m_D5, m_E5, m_C5, m_B4, m_A4, m_G4, m_F4, m_C5, m_B4, m_A4, m_G4, m_F4, m_E4, 0,
  /*virgo */ m_C5, m_A4, m_G4, m_C5,  m_D5, m_E5, /*rest*/0,  m_E5,  m_D5,  m_C5,  m_B4, m_C5, 0,
  };
  
#endif //AVE_SOPRANO

/*--------------------------------------------------------------------------*/
#ifdef AVE_ALTO
char* text = "avemariia,"
             "graatsiapleeeeeena,"
             "daminusteekum,"
             "virgosereeeeeeeeeeena,seerena,";
             
int pitches[] = 
  {
  /*ave*/    m_G4, m_C5, m_C5, m_D5, m_E5, m_C5, 0,
  /*gratia*/ m_C5, m_B4, m_A4, m_G4, m_A4, m_G4, m_C5, m_B4, m_A4, m_B4, m_C5, 0,
  /*dominu*/ m_C5, m_C5, m_C5, m_D5, m_E5, m_C5, 0,
  /*virgo */ m_C5, m_A4, m_G4, m_A4, m_B4, m_C5, m_D5, m_E5, m_D5, m_C5, m_G5, m_F5, m_E5, m_D5, m_C5, /*rest*/0, m_E5, m_F5, m_G5, m_E5, 0,


  };
#endif //AVE_ALTO

/*--------------------------------------------------------------------------*/
#ifdef AVE_TENOR
char* text = "avemariia,"
             "graatsiapleeeeeena,"
             "daminusteeeeeeekum,"
             "virgosereeeeeeeena,";
             
int pitches[] = 
  {
  /*ave*/    m_G3, m_C4, m_C4, m_D4, m_E4, m_C4, 0,
  /*gratia*/ m_C4, m_B3, m_A3, m_G3, m_A3, m_G3, m_C4, m_B3, m_A3, m_B3, m_C4, 0,
  /*dominu*/ m_C4, m_C4, m_C4, m_D4, m_E4, m_C4, m_E4, m_D4, m_G4, m_Fs4, m_G4, 0,
  /*virgo */ m_C4, m_A3, m_G3, m_G4, m_F4, m_E4, m_D4, m_C4, m_B3, m_C4, m_D4, m_C4, 0,

  };

#endif //AVE_TENOR

/*--------------------------------------------------------------------------*/
#ifdef AVE_BASS
char* text = "avemariia,"
             "graatsiapleena,"
             "daminusteekum,"
             "virgosereena,";
             
int pitches[] = 
  {
  /*ave*/    m_G2, m_C3, m_C3, m_D3, m_E3, m_C3, 0,
  /*gratia*/ m_C3, m_B2, m_A2, m_G2, m_A2, m_B2, m_C3, 0,
  /*dominu*/ m_C3, m_C3, m_C3, m_D3, m_E3, m_C3, 0,
  /*virgo */ m_C3, m_A2, m_G2, m_A2, m_B2, m_C3, 0,
  };
#endif //AVE_BASS
