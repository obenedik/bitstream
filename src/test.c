/* ************************************************************************** */
/* *  Original filename : test.c                                            * */
/* *  Create date       : 13-may-2022 08:26:42                              * */
/* *  Contributors      : Ondrej Benedik  <ondrej.benedik@gmail.com>        * */
/* *  Project           : Bitstream Processor                               * */
/* ************************************************************************** */

#include <stdio.h>
#include <string.h>
#include "data_types.h"

#define UNUSED( x )                                                  (void)( x )

#define U8SZ                                                 sizeof( UNSIGNED8 )
#define U16SZ                                               sizeof( UNSIGNED16 )
#define U32SZ                                               sizeof( UNSIGNED32 )

#define BYTE_TO_BINARY_PATTERN                               "%c%c%c%c %c%c%c%c"
#define BYTE_TO_BINARY( byte )                                                 \
  ( byte & 0x80 ? '1' : '0' ),                                                 \
  ( byte & 0x40 ? '1' : '0' ),                                                 \
  ( byte & 0x20 ? '1' : '0' ),                                                 \
  ( byte & 0x10 ? '1' : '0' ),                                                 \
  ( byte & 0x08 ? '1' : '0' ),                                                 \
  ( byte & 0x04 ? '1' : '0' ),                                                 \
  ( byte & 0x02 ? '1' : '0' ),                                                 \
  ( byte & 0x01 ? '1' : '0' ) 

#define TEST_DATA_FILL( arr, sz )                                              \
  &arr[ 0 ], sz, sizeof( arr ) / sz

void vGetBits( void * pvInput,                  // Start of input buffer
               UNSIGNED8 uc8InputItemSize,      // Input items size in bytes
               UNSIGNED16 u16InputItemsCnt,     // Input items count
               void * pvOutput,                 // Start of output buffer
               UNSIGNED8 uc8OutputItemSize,     // Output items size in bytes
               UNSIGNED16 u16OuputMaxLen,       // Output buffer length in bytes
               UNSIGNED16 * pu16OutputLen,      // Output length in bytes
               SIGNED16 s16StartBit,            // 0 - 32767
               SIGNED16 s16BitLen );            // 1 - 32767

typedef struct
{
  void * pvInput;
  UNSIGNED8 uc8InputItemSize;
  UNSIGNED16 u16InputItemsCnt;
  void * pvOutput;
  UNSIGNED8 uc8OutputItemSize;
  UNSIGNED16 u16OuputLen;
  SIGNED16 s16StartBit;
  SIGNED16 s16BitLen;
}
test_data_t;

UNSIGNED8 u8Output[ ( 32768 / 8 ) + 1 ];
UNSIGNED16 u16OuputLen = 0;

UNSIGNED16 u16I1[] = { 0xBA98 };
UNSIGNED8  u8O1[] =  { 0x13 };
UNSIGNED16 u16I2[] = { 0x9876, 0xDCBA };
UNSIGNED32 u32O2[] = { 0x1CBA9876 };
UNSIGNED32 u32I3[] = { 0xFEDCBA98 };
UNSIGNED8  u8O3[] =  { 0xB7, 0x0F };
UNSIGNED8  u8I4[] =  { 0x98, 0xBA, 0xDC };
UNSIGNED16 u16O4[] = { 0x5D4C, 0xE };

test_data_t testdata[] =
{
  { TEST_DATA_FILL( u16I1, U16SZ ), TEST_DATA_FILL( u8O1,  U8SZ  ), 3,  5  },
  { TEST_DATA_FILL( u16I2, U16SZ ), TEST_DATA_FILL( u32O2, U32SZ ), 0,  30 },
  { TEST_DATA_FILL( u32I3, U32SZ ), TEST_DATA_FILL( u8O3,  U8SZ  ), 18, 12 },
  { TEST_DATA_FILL( u8I4,  U8SZ  ), TEST_DATA_FILL( u16O4, U16SZ ), 1,  20 },
  { NULL },
};

static void printf_dat( void * data, UNSIGNED8 u8ItemSize,
                                     UNSIGNED16 u16ItemCount )
{
  UNSIGNED16 u16Idx = 0;

  for(u16Idx = 0 ; u16Idx < u16ItemCount ; u16Idx += 1 )
  {
    printf( "%s", ( u16Idx == 0 ) ? "" : ", " );

    switch( u8ItemSize )
    {
      case U8SZ:
        printf( "0x%02X",  ( (UNSIGNED8 *) data )[ u16Idx ] );
        break;
      case U16SZ:
        printf( "0x%04X",  ( (UNSIGNED16 *) data )[ u16Idx ] );
        break;
      case U32SZ:
        printf( "0x%08lX", ( (UNSIGNED32 *) data )[ u16Idx ] );
        break;
    }
  }
}

static void printf_stream( test_data_t * data )
{
  BOOL_T bFirstRun = enTRUE;
  UNSIGNED16 u16Idx = 0;
  UNSIGNED32 u32Idx = data->u16InputItemsCnt * data->uc8InputItemSize * 8;
  UNSIGNED32 u32IdxMin = data->s16StartBit + 1;
  UNSIGNED32 u32IdxMax = data->s16StartBit + data->s16BitLen + 1;

  for( u16Idx = data->u16InputItemsCnt ; u16Idx > 0 ; u16Idx -= 1 )
  {
    printf( "%s", ( u16Idx == data->u16InputItemsCnt ) ? "" : " " );

    switch( data->uc8InputItemSize )
    {
      case U8SZ:
      {
        UNSIGNED8 u8data = ( (UNSIGNED8 *) data->pvInput )[ u16Idx - 1 ];
        printf( BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY ( u8data ) );
        break;
      }
      case U16SZ:
      {
        UNSIGNED16 u16data = ( (UNSIGNED16 *) data->pvInput )[ u16Idx - 1 ];
        printf( BYTE_TO_BINARY_PATTERN " " BYTE_TO_BINARY_PATTERN,
                BYTE_TO_BINARY( u16data / 0x100 ),
                BYTE_TO_BINARY( u16data % 0x100 ) );
        break;
      }
      case U32SZ:
      {
        UNSIGNED32 u32data = ( (UNSIGNED32 *) data->pvInput )[ u16Idx - 1 ];
        printf( BYTE_TO_BINARY_PATTERN " "
                BYTE_TO_BINARY_PATTERN " "
                BYTE_TO_BINARY_PATTERN " "
                BYTE_TO_BINARY_PATTERN,
                BYTE_TO_BINARY( u32data / 0x1000000 ),
                BYTE_TO_BINARY( u32data / 0x10000 ),
                BYTE_TO_BINARY( u32data / 0x100 ),
                BYTE_TO_BINARY( u32data % 0x100 ) );
        break;
      }
    }
  }
  printf( "\n" );

  for( ; u32Idx > 0 ; u32Idx -= 1 )
  {
    if( ( u32Idx % 4 ) == 0 && bFirstRun == enFALSE )
      printf( " " );
    bFirstRun = enFALSE;
    printf( ( u32Idx < u32IdxMax && u32Idx >= u32IdxMin ) ? "=" : " " );
  }
  printf("\n");
}

int main( int argc, char ** argv )
{
  UNSIGNED32 u32Idx = 0;

  UNUSED( argc );
  UNUSED( argv );

  while( testdata[ u32Idx ].pvInput != NULL )
  {
    UNSIGNED16 u16ExpectedOutputItemCount =
      testdata[ u32Idx ].uc8OutputItemSize * 8 - 1;
    u16ExpectedOutputItemCount += testdata[ u32Idx ].s16BitLen;
    u16ExpectedOutputItemCount /= 8;

    printf( "\nTest %ld :\n", u32Idx + 1 );
    printf( "Startbit = %d; Length = %d\n", testdata[ u32Idx ].s16StartBit,
                                            testdata[ u32Idx ].s16BitLen );
    printf( "Input =  { " );
    printf_dat( testdata[ u32Idx ].pvInput,
                testdata[ u32Idx ].uc8InputItemSize,
                testdata[ u32Idx ].u16InputItemsCnt );
    printf( " }\n" );

    printf_stream( &testdata[ u32Idx ] );

    vGetBits( testdata[ u32Idx ].pvInput,
              testdata[ u32Idx ].uc8InputItemSize,
              testdata[ u32Idx ].u16InputItemsCnt,
              (void *) &u8Output[ 0 ],
              testdata[ u32Idx ].uc8OutputItemSize,
              sizeof( u8Output ),
              &u16OuputLen,
              testdata[ u32Idx ].s16StartBit,
              testdata[ u32Idx ].s16BitLen );

    printf( "Expected Output = { " );
    printf_dat( testdata[ u32Idx ].pvOutput,
                testdata[ u32Idx ].uc8OutputItemSize,
                testdata[ u32Idx ].u16OuputLen );
    printf( " }\n" );

    printf( "Computed Output Length = %d\n", u16OuputLen );
    printf( "Computed Output = { " );
    printf_dat( (void *) &u8Output[ 0 ],
                testdata[ u32Idx ].uc8OutputItemSize,
                u16OuputLen );
    printf(" }\n");

    do
    {
      UNSIGNED8 * pu8Test = (UNSIGNED8 *) testdata[ u32Idx ].pvOutput;
      UNSIGNED32 u32TestLen = u16OuputLen *
                              testdata[ u32Idx ].uc8OutputItemSize;

      if( testdata[ u32Idx ].u16OuputLen != u16OuputLen )
      {
        printf( "!!! Different output length\n" );
        break;
      }
      if( memcmp( &u8Output[ 0 ], pu8Test, u32TestLen ) != 0 )
      {
        printf( "!!! Different output content\n" );
        break;
      }
      printf( "OK\n" );
    }
    while( 0 );

    u32Idx += 1;
  };

  printf( "\n" );

  return 0;
}
