//
//  swap.h
//  org2dat
//
//  Created by Vincent Spader on 6/21/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//  Modified by 20kdc on 6/30/18.
//  (I assume the library README takes precedence
//    over the IDE-generated,
//    and thus not written by the author,
//    copyright notice here. - 20kdc)
//

#include <SDL_endian.h>

#define org_swap_16(x) (((uint16_t)x>>8) | ((uint16_t)x<<8))
#define org_swap_32(x) (((uint32_t)x>>24) | (((uint32_t)x<<8) & 0x00FF0000) | (((uint32_t)x>>8) & 0x0000FF00) | ((uint32_t)x<<24));

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define org_btoh_16(x) org_swap_16(x)
#define org_btoh_32(x) org_swap_32(x)

#define org_ltoh_16(x) x
#define org_ltoh_32(x) x
#else
#define org_btoh_16(x) x
#define org_btoh_32(x) x

#define org_ltoh_16(x) org_swap_16(x)
#define org_ltoh_32(x) org_swap_32(x)
#endif
