#include "bv.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Finds the location of a byte given a bit in a bit vector.
#define BYTE(i) (uint32_t)(i / 8)

// Finds the location of a specified bit in a bit vector.
#define BIT(i) (uint32_t)(i % 8)

// BitVector ADT from Professor Darrell Long via asgn5.pdf
struct BitVector {
    uint32_t length;
    uint8_t *vector;
};

BitVector *bv_create(uint32_t length) {
    if (length < 1) {
        return NULL;
    }

    BitVector *v = (BitVector *) malloc(sizeof(BitVector));

    if (v) {
        v->length = length;
        uint32_t bytes; // Amount of bytes the vector is made up of.

        // Do this so we don't accidentally allocate 1 extra unnecessary byte if the
        // amount of bits fits perfectly.
        if ((length % 8) == 0) {
            bytes = length / 8;
            v->vector = (uint8_t *) calloc(bytes, sizeof(uint8_t));
        } else {
            bytes = (length / 8) + 1;
            v->vector = (uint8_t *) calloc(bytes, sizeof(uint8_t));
        }

        if (!v->vector) {
            free(v);
            v = NULL;
        } else {

            // Initialize all digits to 0
            for (uint32_t i = 0; i < bytes; i += 1) {
                v->vector[i] = 0;
            }
        }
    }

    return v;
}

void bv_delete(BitVector **v) {
    if (*v && (*v)->vector) {
        free((*v)->vector);
        free(*v);
        *v = NULL;
    }

    return;
}

uint32_t bv_length(BitVector *v) {
    return v->length;
}

void bv_set_bit(BitVector *v, uint32_t i) {

    // 128 (decimal) in binary is 10000000
    // Start with a 1 at the MSB, move right to the index of the bit we want to set.
    uint8_t set = 128 >> BIT(i);
    v->vector[BYTE(i)] |= set;
    return;
}

void bv_clr_bit(BitVector *v, uint32_t i) {

    // Do the same thing as bv_set_bit but then NOT it so that all bits but the one we want
    // to clear are 1s.
    uint8_t clr = ~(128 >> BIT(i));
    v->vector[BYTE(i)] &= clr;
    return;
}

void bv_xor_bit(BitVector *v, uint32_t i, uint8_t bit) {

    // Bit starts at LSB, so move it to MSB first and then move it right to the desired index.
    bit <<= 7;
    bit >>= BIT(i);
    v->vector[BYTE(i)] ^= bit;
    return;
}

uint8_t bv_get_bit(BitVector *v, uint32_t i) {

    // Shift 1 right to desired bit's index. Since all indices but the desired one are 0s, we
    // can simply do a bitwise AND, which masks all bits but the desired one. Then shift it
    // right to the LSB.
    uint8_t bit = 128 >> BIT(i);
    bit = v->vector[BYTE(i)] & bit;
    bit = bit >> (7 - BIT(i));
    return bit;
}

void bv_print(BitVector *v) {
    for (uint32_t i = 0; i < v->length; i += 1) {
        uint8_t bit = v->vector[BYTE(i)] << BIT(i);
        bit = bit >> 7;
        printf("%u ", bit);
    }

    printf("\n");
    return;
}
