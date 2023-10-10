#include "hamming.h"

#include "bm.h"

#include <stdint.h>
#include <stdio.h>

// Look-up table. Index is error syndrome, corresponds with which digit has been erroneously
// flipped. HAM_ERR means more than 1 error has occurred and is uncorrectable.
static const int8_t error[16] = { HAM_OK, 7, 6, HAM_ERR, 5, HAM_ERR, HAM_ERR, 0, 4, HAM_ERR,
    HAM_ERR, 1, HAM_ERR, 2, 3, HAM_ERR };

uint8_t ham_encode(BitMatrix *G, uint8_t msg) {

    // Make BitMatrix from lower nibble of msg
    BitMatrix *lower = bm_from_data(msg, 4);
    BitMatrix *encode = bm_multiply(lower, G);
    uint8_t encoded = bm_to_data(encode);
    bm_delete(&lower);
    bm_delete(&encode);

    return encoded;
}

HAM_STATUS ham_decode(BitMatrix *Ht, uint8_t code, uint8_t *msg) {
    BitMatrix *encoded = bm_from_data(code, 8);
    BitMatrix *e = bm_multiply(encoded, Ht);

    int8_t lookup = error[bm_to_data(e)];

    // Bitwise & the decoded byte by 00001111 (15 in decimal)
    // to mask out the parity bits.
    if (lookup == HAM_OK) {
        *msg = bm_to_data(encoded) & 15;

        bm_delete(&encoded);
        bm_delete(&e);
        return HAM_OK;
    } else if (lookup == HAM_ERR) {
        *msg = bm_to_data(encoded) & 15;

        bm_delete(&encoded);
        bm_delete(&e);
        return HAM_ERR;
    } else {

        // Correct error by flipping the error bit
        if (bm_get_bit(encoded, 0, lookup) == 1) {
            bm_clr_bit(encoded, 0, lookup);
        } else {
            bm_set_bit(encoded, 0, lookup);
        }

        *msg = bm_to_data(encoded) & 15;

        bm_delete(&encoded);
        bm_delete(&e);
        return HAM_CORRECT;
    }
}
