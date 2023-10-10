#include "bm.h"

#include "bv.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// BitMatrix ADT from Professor Darrell Long via asgn5.pdf
struct BitMatrix {
    uint32_t rows;
    uint32_t cols;
    BitVector *vector;
};

// Returns index in underlying BitVector of the given coordinate.
// (desired row * total columns) + desired column
uint32_t static inline bm_index(BitMatrix *m, uint32_t row, uint32_t col) {
    return (row * m->cols) + col;
}

BitMatrix *bm_create(uint32_t rows, uint32_t cols) {
    if (rows < 1 || cols < 1) {
        return NULL;
    }

    BitMatrix *m = (BitMatrix *) malloc(sizeof(BitMatrix));

    if (m) {
        m->rows = rows;
        m->cols = cols;

        // rows * cols = total elements in a matrix
        m->vector = bv_create(rows * cols);
    }

    return m;
}

void bm_delete(BitMatrix **m) {
    if (*m && (*m)->vector) {
        bv_delete(&(*m)->vector);
        free(*m);
        *m = NULL;
    }

    return;
}

uint32_t bm_rows(BitMatrix *m) {
    return m->rows;
}

uint32_t bm_cols(BitMatrix *m) {
    return m->cols;
}

void bm_set_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    bv_set_bit(m->vector, bm_index(m, r, c));
}

void bm_clr_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    bv_clr_bit(m->vector, bm_index(m, r, c));
}

uint8_t bm_get_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    return bv_get_bit(m->vector, bm_index(m, r, c));
}

BitMatrix *bm_from_data(uint8_t byte, uint32_t length) {
    BitMatrix *m = (BitMatrix *) malloc(sizeof(BitMatrix));

    if (m) {
        m->rows = 1;
        m->cols = length;
        m->vector = bv_create(length);

        for (uint32_t i = 0; i < length; i += 1) {

            // Starting with 0th bit, shift right until desired bit is at the front,
            // then AND with 00000001 to mask unneeded bits on the left of it.
            // If this ith bit is 1, then set. If not, then we don't need to do anything.
            if (((byte >> i) & 1) == 1) {
                bv_set_bit(m->vector, i);
            }
        }
    }

    return m;
}

uint8_t bm_to_data(BitMatrix *m) {
    uint8_t data = 0;

    for (uint32_t i = 0; i < 8; i += 1) {

        // Go through the first 8 bits of the BitMatrix, each time we do this shift the data
        // byte right by 1 to effectively move over 1 index in this byte as well. If the bit
        // in the BitMatrix is 1, then add 128 (equal to 10000000 in binary) to data to match.
        // Once we have reached the final index, don't shift the data anymore.
        if (bm_get_bit(m, 0, i) == 1) {
            data += 128;
        }

        if (i < 7) {
            data >>= 1;
        }
    }

    return data;
}

BitMatrix *bm_multiply(BitMatrix *A, BitMatrix *B) {
    BitMatrix *C = (BitMatrix *) malloc(sizeof(BitMatrix));

    if (C) {
        C->rows = A->rows;
        C->cols = B->cols;
        C->vector = bv_create(A->rows * B->cols);

        // The two operands that will be multiplied
        uint32_t A_operand;
        uint32_t B_operand;

        for (uint32_t A_r = 0; A_r < A->rows; A_r += 1) {
            for (uint32_t B_c = 0; B_c < B->cols; B_c += 1) {
                uint8_t product = 0;

                for (uint32_t A_c = 0; A_c < A->cols; A_c += 1) {

                    // XOR equivalent to addition mod 2, AND equivalent to multiplication
                    // mod 2.
                    A_operand = bm_index(A, A_r, A_c);
                    B_operand = bm_index(B, A_c, B_c);
                    product ^= bv_get_bit(A->vector, A_operand) & bv_get_bit(B->vector, B_operand);
                }

                // Add product to corresponding index in product matrix
                bv_xor_bit(C->vector, bm_index(C, A_r, B_c), product);
            }
        }
    }

    return C;
}

void bm_print(BitMatrix *m) {
    for (uint32_t r = 0; r < bm_rows(m); r += 1) {
        for (uint32_t c = 0; c < bm_cols(m); c += 1) {
            printf("%u ", bm_get_bit(m, r, c));
        }

        printf("\n");
    }

    return;
}
