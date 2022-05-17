/* ************************************************************************** */
/* *  Original filename : getbits.c                                         * */
/* *  Create date       : 13-may-2022 08:26:35                              * */
/* *  Contributors      : Ondrej Benedik  <ondrej.benedik@gmail.com>        * */
/* *  Project           : Bitstream Processor                               * */
/* ************************************************************************** */

#include <stdio.h>
#include "data_types.h"

#define ARRAY_IDX( IsBigEndian, Index, Size )                                 \
          ( IsBigEndian == enFALSE ) ? (Index) :                              \
          ( (Index) / (Size) * (Size) + (Size) - 1 - ( (Index) % (Size) ) )

void vGetBits( void * pvInput,                  // Start of input buffer
               UNSIGNED8 uc8InputItemSize,      // Input items size in bytes
               UNSIGNED16 u16InputItemsCnt,     // Input items count
               void * pvOutput,                 // Start of output buffer
               UNSIGNED8 uc8OutputItemSize,     // Output items size in bytes
               UNSIGNED16 u16OuputMaxLen,       // Output buffer length in bytes
               UNSIGNED16 * pu16OutputLen,      // Output length in bytes
               SIGNED16 s16StartBit,            // 0 - 32767
               SIGNED16 s16BitLen )             // 1 - 32767
{
  UNSIGNED8 * pu8Input = (UNSIGNED8 *) pvInput;
  UNSIGNED8 * pu8Output = (UNSIGNED8 *) pvOutput;
  UNSIGNED16 u16InPtr = s16StartBit / 8;;
  UNSIGNED16 u16OutPtr = 0;

  UNSIGNED32 u32InputMinLen = ( (s16StartBit + s16BitLen + 7 ) / 8 );
  UNSIGNED32 u32InputLen = u16InputItemsCnt * uc8InputItemSize;
  UNSIGNED32 u32OutputMinLen = ( (s16BitLen + 7 ) / 8 );

  UNSIGNED8 u8PosAlign = ( s16StartBit % 8 );
  UNSIGNED8 u8NextPosAlign = 8 - u8PosAlign;
  UNSIGNED8 u8NextPosMask = ( ( 1 << u8PosAlign ) - 1 );
  UNSIGNED8 u8PosMask = ~u8NextPosMask;
  UNSIGNED8 u8LastPosMask = ( 1 << ( ( s16StartBit + s16BitLen ) % 8 ) );

  BOOL_T bIsBigEndian = enTRUE;
  SIGNED32 s32BigEndianMarker = 1;

  do
  {
    if( pvInput == NULL || pvOutput == NULL )
      break;    // No input or no output data
    if( s16StartBit < 0 || s16BitLen < 1 )
      break;    // Bit boundary out of range
    if( u32InputLen < u32InputMinLen )
      break;    // Small input buffer
    if( u16OuputMaxLen < u32OutputMinLen )
      break;    // Small output buffer

    if( *(SIGNED8 *) &s32BigEndianMarker == 1 )
      bIsBigEndian = enFALSE;
    u8LastPosMask = ( u8LastPosMask == 0 ) ? 0 : ( u8LastPosMask - 1 );

    while( u32OutputMinLen > 0 )
    {
      UNSIGNED16 u16Right = ARRAY_IDX( bIsBigEndian,
                                       u16InPtr,
                                       uc8InputItemSize );
      UNSIGNED16 u16Left  = ARRAY_IDX( bIsBigEndian,
                                       u16InPtr + 1,
                                       uc8InputItemSize );
      UNSIGNED16 u16Out   = ARRAY_IDX( bIsBigEndian,
                                       u16OutPtr,
                                       uc8OutputItemSize );
      UNSIGNED8 u8RightPart = pu8Input[ u16Right ];
      UNSIGNED8 u8LeftPart = 0;

      if( u32OutputMinLen == 1 && u8LastPosMask != 0 )
        u8RightPart &= u8LastPosMask;

      u8RightPart &= u8PosMask;
      u8RightPart >>= u8PosAlign;
      if( u16Left < u32InputLen && u32OutputMinLen > 1 )
        u8LeftPart = pu8Input[ u16Left ];
      u8LeftPart &= u8NextPosMask;
      u8LeftPart <<= u8NextPosAlign;

      pu8Output[ u16Out ] = u8RightPart | u8LeftPart;

      u32OutputMinLen -= 1;
      u16InPtr += 1;
      u16OutPtr += 1;
    }

    if( pu16OutputLen != NULL )
      *pu16OutputLen =
        ( u16OutPtr + ( uc8OutputItemSize - 1 ) ) / uc8OutputItemSize;

    while( ( u16OutPtr % uc8OutputItemSize ) != 0 )
    {
      pu8Output[ u16OutPtr ] = 0;
      u16OutPtr += 1;
    }
  }
  while( 0 );
}
