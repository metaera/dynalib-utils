//
// Created by Ken Kopelson on 14/10/17.
//

#ifndef BITMANIP_H
#define BITMANIP_H

#define SET_BITS(value, bits)       ((value) |= (bits))
#define CLEAR_BITS(value, bits)     ((value) &= ~(bits))
#define IS_ALL_SET(value, bits)     (((value) & (bits)) == (bits))
#define IS_ANY_SET(value, bits)     (((value) & (bits)) != 0)
#define IS_ALL_CLEAR(value, bits)   (((value) & (bits)) == 0)
#define IS_ANY_CLEAR(value, bits)   (!IS_ALL_SET(value, bits) && IS_ANY_SET(value, bits))

#endif //BITMANIP_H
