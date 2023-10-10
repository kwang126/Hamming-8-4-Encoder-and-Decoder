#include "bm.h"
#include "hamming.h"

#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "hi:o:"

// Value an index in the parity array is set to if the corresponding parity nibble hasn't
// been calculated before.
#define NA 99

// statbuf struct declaration given by Professor Darrell Long via asgn5.pdf
struct stat statbuf;

// lower_nibble, upper_nibble, and pack_byte helper functions given by
// Professor Darrell Long via asgn5.pdf
uint8_t lower_nibble(uint8_t val) {
    return val & 0xF;
}

uint8_t upper_nibble(uint8_t val) {
    return val >> 4;
}

uint8_t pack_byte(uint8_t upper, uint8_t lower) {
    return (upper << 4) | (lower & 0xF);
}

// Prints help message explaining program usage.
static inline void help(void) {
    printf("SYNOPSIS\n  A Hamming(8, 4) systematic code generator.\n\n");
    printf("USAGE\n  ./encode [-h] [-i infile] [-o outfile]\n\n");
    printf("OPTIONS\n  %-15sProgram usage and help.\n", "-h");
    printf("  %-15sInput data to encode.\n", "-i infile");
    printf("  %-15sOutput of encoded data.\n", "-o outfile");

    return;
}

// Creates the generator matrix for the Hamming(8, 4) code
static BitMatrix *generator_bm(void) {
    BitMatrix *G = bm_create(4, 8);

    for (uint32_t r = 0; r < bm_rows(G); r += 1) {
        for (uint32_t c = 0; c < bm_cols(G); c += 1) {

            // If we are in the first half columns, the bit at coordinate row = column
            // is the only bit set. In the second half columns, the bit at the coordinate
            // row = column is the only bit that is not set.
            if (c < 4) {
                if (r == c) {
                    bm_set_bit(G, r, c);
                }
            } else {
                if (r != (c - 4)) {
                    bm_set_bit(G, r, c);
                }
            }
        }
    }

    return G;
}

int main(int argc, char **argv) {
    int8_t opt = 0;
    FILE *infile = stdin;
    FILE *outfile = stdout;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h': help(); return 0;

        case 'i':
            infile = fopen(optarg, "rb");

            // If the FILE is NULL, then the file failed to open.
            if (infile == NULL) {
                printf("Error: failed to open infile.\n");
                return 1;
            }

            break;

        case 'o':
            outfile = fopen(optarg, "wb");

            if (outfile == NULL) {
                printf("Error: failed to open outfile.\n");
                return 1;
            }

            break;

        default: break;
        }
    }

    // Making outfile's permissions the same as infile's.
    // Given by Professor Darrell Long via asgn5.pdf
    fstat(fileno(infile), &statbuf);
    fchmod(fileno(outfile), statbuf.st_mode);

    BitMatrix *G = generator_bm();
    int8_t byte; // Byte read from infile
    uint8_t upper; // Upper nibble of byte
    uint8_t lower; // Lower nibble of byte

    // Array of parity nibbles. Starts initialized to value NA (not available),
    // and when a new parity is calculated it is stored in the array for the future.
    uint8_t parity[16] = { NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA };

    // Read a byte in and split it into an upper and lower nibble. Use the look-up table
    // to get the parity nibble for each data nibble and
    while ((byte = fgetc(infile)) != EOF) {
        upper = upper_nibble(byte);
        lower = lower_nibble(byte);

        if (parity[lower] == NA) {
            lower = ham_encode(G, lower);
            parity[lower & 15] = lower >> 4; // Get the parity bits and save them for the future
        } else {
            lower = pack_byte(parity[lower], lower);
        }

        fputc(lower, outfile);

        if (parity[upper] == NA) {
            upper = ham_encode(G, upper);
            parity[upper & 15] = upper >> 4;
        } else {
            upper = pack_byte(parity[upper], upper);
        }

        fputc(upper, outfile);
    }

    fclose(infile);
    fclose(outfile);
    bm_delete(&G);
    return 0;
}
