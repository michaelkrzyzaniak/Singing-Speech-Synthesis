/*!
 * @header MIDI.h
 * @abstract define MIDI note numbers using the
 *  American system for naming notes, in which C4 
 *  represents middle C.
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

#ifndef  __MK_MIDI__     
#define  __MK_MIDI__ 1 


#define m_CNEG1   0x00
#define m_CsNEG1  0x01
#define m_DbNEG1  0x01
#define m_DNEG1   0x02
#define m_DsNEG1  0x03
#define m_EbNEG1  0x03
#define m_ENEG1   0x04
#define m_FNEG1   0x05
#define m_FsNEG1  0x06
#define m_GbNEG1  0x06
#define m_GNEG1   0x07
#define m_GsNEG1  0x08
#define m_AbNEG1  0x08
#define m_ANEG1   0x09
#define m_AsNEG1  0x0A
#define m_BbNEG1  0x0A
#define m_BNEG1   0x0B

#define m_C0   0x0C
#define m_Cs0  0x0D
#define m_Db0  0x0D
#define m_D0   0x0E
#define m_Ds0  0x0F
#define m_Eb0  0x0F
#define m_E0   0x10
#define m_F0   0x11
#define m_Fs0  0x12
#define m_Gb0  0x12
#define m_G0   0x13
#define m_Gs0  0x14
#define m_Ab0  0x14
#define m_A0   0x15
#define m_As0  0x16
#define m_Bb0  0x16
#define m_B0   0x17

#define m_C1   0x18
#define m_Cs1  0x19
#define m_Db1  0x19
#define m_D1   0x1A
#define m_Ds1  0x1B
#define m_Eb1  0x1B
#define m_E1   0x1C
#define m_F1   0x1D
#define m_Fs1  0x1E
#define m_Gb1  0x1E
#define m_G1   0x1F
#define m_Gs1  0x20
#define m_Ab1  0x20
#define m_A1   0x21
#define m_As1  0x22
#define m_Bb1  0x22
#define m_B1   0x23    

#define m_C2   0x24
#define m_Cs2  0x25
#define m_Db2  0x25
#define m_D2   0x26
#define m_Ds2  0x27
#define m_Eb2  0x27
#define m_E2   0x28
#define m_F2   0x29
#define m_Fs2  0x2A
#define m_Gb2  0x2A
#define m_G2   0x2B
#define m_Gs2  0x2C
#define m_Ab2  0x2C
#define m_A2   0x2D
#define m_As2  0x2E
#define m_Bb2  0x2E
#define m_B2   0x2F

#define m_C3   0x30
#define m_Cs3  0x31
#define m_Db3  0x31
#define m_D3   0x32
#define m_Ds3  0x33
#define m_Eb3  0x33
#define m_E3   0x34
#define m_F3   0x35
#define m_Fs3  0x36
#define m_Gb3  0x36
#define m_G3   0x37
#define m_Gs3  0x38
#define m_Ab3  0x38
#define m_A3   0x39
#define m_As3  0x3A
#define m_Bb3  0x3A
#define m_B3   0x3B

#define m_C4   0x3C
#define m_Cs4  0x3D
#define m_Db4  0x3D
#define m_D4   0x3E
#define m_Ds4  0x3F
#define m_Eb4  0x3F
#define m_E4   0x40
#define m_F4   0x41
#define m_Fs4  0x42
#define m_Gb4  0x42
#define m_G4   0x43
#define m_Gs4  0x44
#define m_Ab4  0x44
#define m_A4   0x45
#define m_As4  0x46
#define m_Bb4  0x46
#define m_B4   0x47

#define m_C5   0x48
#define m_Cs5  0x49
#define m_Db5  0x49
#define m_D5   0x4A
#define m_Ds5  0x4B
#define m_Eb5  0x4B
#define m_E5   0x4C
#define m_F5   0x4D
#define m_Fs5  0x4E
#define m_Gb5  0x4E
#define m_G5   0x4F
#define m_Gs5  0x50
#define m_Ab5  0x50
#define m_A5   0x51
#define m_As5  0x52
#define m_Bb5  0x52
#define m_B5   0x53

#define m_C6   0x54
#define m_Cs6  0x55
#define m_Db6  0x55
#define m_D6   0x56
#define m_Ds6  0x57
#define m_Eb6  0x57
#define m_E6   0x58
#define m_F6   0x59
#define m_Fs6  0x5A
#define m_Gb6  0x5A
#define m_G6   0x5B
#define m_Gs6  0x5C
#define m_Ab6  0x5C
#define m_A6   0x5D
#define m_As6  0x5E
#define m_Bb6  0x5E
#define m_B6   0x5F

#define m_C7   0x60
#define m_Cs7  0x61
#define m_Db7  0x61
#define m_D7   0x62
#define m_Ds7  0x63
#define m_Eb7  0x63
#define m_E7   0x64
#define m_F7   0x65
#define m_Fs7  0x66
#define m_Gb7  0x66
#define m_G7   0x67
#define m_Gs7  0x68
#define m_Ab7  0x68
#define m_A7   0x69
#define m_As7  0x6A
#define m_Bb7  0x6A
#define m_B7   0x6B

#define m_C8   0x6C
#define m_Cs8  0x6D
#define m_Db8  0x6D
#define m_D8   0x6E
#define m_Ds8  0x6F
#define m_Eb8  0x6F
#define m_E8   0x70
#define m_F8   0x71
#define m_Fs8  0x72
#define m_Gb8  0x72
#define m_G8   0x73
#define m_Gs8  0x74
#define m_Ab8  0x74
#define m_A8   0x75
#define m_As8  0x76
#define m_Bb8  0x76
#define m_B8   0x77

#define m_C9   0x78
#define m_Cs9  0x79
#define m_Db9  0x79
#define m_D9   0x7A
#define m_Ds9  0x7B
#define m_Eb9  0x7B
#define m_E9   0x7C
#define m_F9   0x7D
#define m_Fs9  0x7E
#define m_Gb9  0x7E
#define m_G9   0x7F

#endif //__m_K_m_IDI__     