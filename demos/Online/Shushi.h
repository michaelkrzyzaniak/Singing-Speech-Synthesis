#include "MIDINotes.h"


/*--------------------------------------------------------------------------*/
#ifdef SHUSHI
char* text = "v|e-|r|a-|g|a-|r|z|a-|a-|i|t| -|sh|u-|u-|sh|i-| -|" // taraitsi, nadereda, vazanchetzals tzang nana.
             "ih-|m|p|a-|p|e-|r|i-|i-|d|o-|sh|u-|u-|sh|i-| -|"
             "t|a-|r|a-|a-|l|a-|a-|ch|i-|i-|i-|i-| -|"
             "n|a-|d|e-|r|e-|d|a-| -|"
             "v|a-|z|a-|n|c|h|e-|t|z|a-|l|s| -|z|a-|ŋ| -|ə|n|a-|n|a-| -|"
             "t|a-|r|a-|a-|l|a-|a-|ch|i-|i-|i-|i-| -|"
             "n|a-|d|e-|r|e-|d|a-| -|"
             "v|a-|z|a-|n|c|h|e-|t|z|a-|l|s| -|z|a-|ŋ| -|ə|n|a-|n|a-| -|"
             ;
             
int pitches[] = 
{
  m_D3 , m_A3 , m_A3 , m_A3 , m_Bb3, m_C4 , m_Bb3, m_A3 , m_A3 , m_A3/*r*/ , 
  m_A3 , m_D4 , m_A3 , m_D4 , m_E4 , m_F4 , m_E4 , m_D4 , m_D4 , m_D4/*r*/ ,
  m_D4 , m_D4 , m_C4 , m_C4 , m_Bb3, m_Bb3, m_A3 , m_Bb3, m_C4 , m_C4/*r*/ ,
  m_C4 , m_Bb3, m_A3, m_A3 , m_D3 ,
  m_D3 , m_E3 , m_F3, m_F3 , m_F3/*r*/ , m_E3, m_E3/*r*/, m_Cs3, m_D3, m_D3/*r*/,
  m_D4 , m_D4 , m_C4 , m_C4 , m_Bb3, m_Bb3, m_A3 , m_Bb3, m_C4 , m_C4/*r*/ ,
  m_C4 , m_Bb3, m_A3, m_A3 , m_D3 ,
  m_D3 , m_E3 , m_F3, m_F3 , m_F3/*r*/ , m_E3, m_E3/*r*/, m_Cs3, m_D3, m_D3/*r*/
};

  
#endif //SHUSHI


#ifdef SHUSHI_TENOR
char* text = "sh|u-|u-|sh|i-| -|p|a-|p|e-|r|i-|d|o-|sh|u-|sh|i-| -|"
             "t|a-|r|a-|l|a-|ch|i-|"
             "n|a-|d|e-|r|e-|d|a-|a-|a-| -|"
             "v|a-|z|a-|n|c|h|e-|t|z|a-|l|z|a-|ŋ|ə|n|a-|n|a-| -|"
             "t|a-|r|a-|l|a-|ch|i-|"
             "n|a-|d|e-|r|e-|d|a-|a-|a-| -|"
             "c|h|e-|t|z|a-|l|z|a-|ŋ|ə|n|a-|n|a-| -|"
             ;

             
int pitches[] = 
{
  m_F4, m_E4, m_D4 , m_D4 /*r*/, 
  m_F4, m_A3, m_Bb3, m_Bb3, m_A3, m_F4, m_F4 /*r*/, 
  m_D4, m_E4, m_D4, m_Cs4, m_Cs4, m_Bb3, m_A3, m_A3, m_Bb3, m_C4, m_C4/*r*/,
  m_D4, m_D4, m_Cs4, m_Bb3, m_A3, m_G3, m_F3, m_F3/*r*/, 
  m_D4, m_E4, m_D4, m_Cs4, m_Cs4, m_Bb3, m_A3, m_A3, m_Bb3, m_C4, m_C4/*r*/,
  m_D4, m_Cs4, m_Bb3, m_A3, m_A3, m_A3/*r*/,
};

#endif //SHUSHI_TENOR


#ifdef SHUSHI_BASS
char* text = "sh|u-|u-|u-|u-|u-|sh|i-| -|"
             "p|a-|p|e-|r|i-|d|o-|sh|u-|sh|i-| -|"
             "t|a-|r|a-|a-|l|a-|a-|n|a-|d|e-|r|e-|d|a-|a-|a-|a-|a-| |"
             "v|a-|z|a-|n|c|h|e-|t|z|a-|l|z|a-|ŋ|n|a-|n|a-| -|"
             "t|a-|r|a-|a-|l|a-|a-|n|a-|d|e-|r|e-|d|a-|a-|a-|a-|a-| |"
             "v|a-|z|a-|n|c|h|e-|t|z|a-|l|z|a-|ŋ|n|a-|n|a-| -|"
             ;

             
int pitches[] = 
{
  m_D3, m_C3, m_Bb2, m_A2, m_E3, m_F3, m_Bb2/*r*/, 
  m_Bb2, m_D3, m_E3, m_F3, m_D3, m_D3, m_D3 /*r*/, 
  m_G3, m_Bb3, m_G3, m_G3,
  m_A2, m_G3 , m_F3, m_E3, m_D3, m_C3, m_D3,  m_E3, m_F3, m_F3, m_G3, m_A3, m_D3, m_A2, m_E3, m_Bb2, m_Bb2/*r*/, 
  m_G3, m_Bb3, m_G3, m_G3,
  m_A2, m_G3 , m_F3, m_E3, m_D3, m_C3, m_D3,  m_E3, m_F3, m_F3, m_G3, m_A3, m_A3, m_A2, m_A2, m_D3, m_D3/*r*/, 
};

#endif //SHUSHI_BASS

#ifdef SHUSH_END
char* text = "i|o-|o-|r|h|a-|a-|r|t| -|b|i-|i-|i-|t| -|"
             ;

             
int pitches[] = 
{
  m_D3 , m_E3 , m_F3, m_G3, m_E3,/*r*/ m_E3, m_F3, m_D3, m_D3/*r*/
};


#endif //SHUSH_END











