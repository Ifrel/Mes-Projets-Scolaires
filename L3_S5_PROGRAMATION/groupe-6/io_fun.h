#ifndef __IO_FUN_H__
#define __IO_FUN_H__

#include <stdio.h>
#include <stdint.h>
#include "status.h"

ERR_CODE get_u32(FILE *stream, uint32_t *buffer);
ERR_CODE get_u16(FILE *stream, uint16_t *buffer);
ERR_CODE get_u8(FILE *stream, uint8_t *buffer);

ERR_CODE set_u32(FILE *dest, uint32_t buffer);
ERR_CODE set_u16(FILE *dest, uint16_t buffer);
ERR_CODE set_u8(FILE *dest, uint8_t buffer);


#endif
