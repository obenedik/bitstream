/* ************************************************************************** */
/* *  Original filename : data_types.h                                      * */
/* *  Create date       : 13-may-2022 08:26:28                              * */
/* *  Contributors      : Ondrej Benedik  <ondrej.benedik@gmail.com>        * */
/* *  Project           : Bitstream Processor                               * */
/* ************************************************************************** */

#ifndef DATA_TYPES_H_
#define DATA_TYPES_H_

/* prefix for objects of the defined type */
typedef unsigned char           UNSIGNED8;      /* u8 */
typedef unsigned short          UNSIGNED16;     /* u16 */
typedef unsigned long           UNSIGNED32;     /* u32 */
typedef unsigned long long      UNSIGNED64;     /* u64 */
typedef signed char             SIGNED8;        /* s8 */
typedef signed short            SIGNED16;       /* s16 */
typedef signed long             SIGNED32;       /* s32 */
typedef long long               SIGNED64;       /* s64 */

typedef enum
{
  enFALSE = 0x00,
  enTRUE = 0xFF
}
BOOL_T;                                         /* b */

#endif /* DATA_TYPES_H_ */
