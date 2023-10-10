#include "bm.h"
#include "hamming.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define OPTIONS "hi:o:v"

// statbuf struct declaration given by Professor Darrell Long via asgn5.pdf
struct stat statbuf;

// pack_byte helper function given by Professor Darrell Long via asgn5.pdf
uint8_t pack_byte(uint8_t upper, uint8_t lower) {
    return (upper << 4) | (lower & 0xF);
}

static inline void help(void) {
    printf("SYNOPSIS\n  A Hamming(8, 4) systematic code decoder.\n\n");
    printf("USAGE\n  ./decode [-h] [-v] [-i infile] [-o outfile]\n\n");
    printf("OPTIONS\n  %-15sProgram usage and help.\n", "-h");
    printf("  %-15sPrint statistics of decoding to stderr.\n", "-v");
    printf("  %-15sInput data to decode.\n", "-i infile");
    printf("  %-15sOutput of decoded data.\n", "-o outfile");

    return;
}

// Creates the parity-checker matrix for the Hamming(8, 4) code
static BitMatrix *checker_bm(void) {
    BitMatrix *Ht = bm_create(8, 4);

    for (uint32_t r = 0; r < bm_rows(Ht); r += 1) {
        for (uint32_t c = 0; c < bm_cols(Ht); c += 1) {

            // If we are in the first half rows, the bit at coordinate row = column
            // is the only bit not set. In the second half rows, the bit at the coordinate
            // row = column is the only bit that is set.
            if (r < 4) {
                if (r != c) {
                    bm_set_bit(Ht, r, c);
                }
            } else {
                if (c == (r - 4)) {
                    bm_set_bit(Ht, r, c);
                }
            }
        }
    }

    return Ht;
}

int main(int argc, char **argv) {
    bool verbose = false;
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

        case 'v': verbose = true; break;

        default: break;
        }
    }

    // Making outfile's permissions the same as infile's.
    // Given by Professor Darrell Long via asgn5.pdf
    fstat(fileno(infile), &statbuf);
    fchmod(fileno(outfile), statbuf.st_mode);

    BitMatrix *Ht = checker_bm();
    int8_t lower; // Lower byte
    int8_t upper; // Upper byte
    uint8_t lower_nibble; // Lower decoded nibble
    uint8_t upper_nibble; // Upper decoded nibble
    uint8_t byte; // Decoded byte

    // Statistic counters
    uint64_t byte_counter = 0; // Bytes processed
    uint64_t err_counter = 0; // Errors uncorrected
    uint64_t corr_counter = 0; // Errors corrected
    HAM_STATUS status;

    while ((lower = fgetc(infile)), !feof(infile)) {
        byte_counter += 1;
        status = ham_decode(Ht, lower, &lower_nibble);

        if (status == HAM_CORRECT) {
            corr_counter += 1;
        } else if (status == HAM_ERR) {
            err_counter += 1;
        }

        if ((upper = fgetc(infile)), !feof(infile)) {
            byte_counter += 1;
            status = ham_decode(Ht, upper, &upper_nibble);

            if (status == HAM_CORRECT) {
                corr_counter += 1;
            } else if (status == HAM_ERR) {
                err_counter += 1;
            }

            byte = pack_byte(upper_nibble, lower_nibble);
            fputc(byte, outfile);
        } else {
            break;
        }
    }

    fclose(infile);
    fclose(outfile);
    bm_delete(&Ht);

    if (verbose) {
        // Ratio of uncorrected errors to total bytes processed
        double err_rate = (double) err_counter / (double) byte_counter;
        fprintf(stderr, "Total bytes processed: %ld\n", byte_counter);
        fprintf(stderr, "Uncorrected errors: %ld\n", err_counter);
        fprintf(stderr, "Corrected errors: %ld\n", corr_counter);
        fprintf(stderr, "Error rate: %1.6f\n", err_rate);
    }

    return 0;
}
