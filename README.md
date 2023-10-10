# Hamming(8, 4) Encoder and Decoder

This assignment is made up of two programs: a Hamming code encoder and decoder both
operating with the Hamming(8, 4) systematic code. The generator and parity-checker
matrices are represented with bit matrices which are made of underlying bit vectors.
The input for the encoder and decoder is read from stdin by default, or a file if
specified. The output for both is stdout by default, or a file if specified. The
decoder has the option of printing statistics if the user desires. error and entropy
programs are provided by Professor Darrell Long

## Build

        $ make

        or

        $ make all

        (These both have the same effect: building all programs.)

        $ make encode

        (Builds just the encoder)

        $ make decode

        (Builds just the decoder)

        $ make error

        (Builds just the error-injection program)

        $ make entropy

        (Builds just the entropy-measuring program)

## Running

        $ ./encode -[h] -[i <infile>] -[o <outfile>]

        -h Prints a help message explaining program usage.
        -i Allows user to specify an input file path. Default is stdin
        -o Allows user to specify an output file path. Default is stdout

        $ ./decode -[hv] -[i <infile>] -[o <outfile>]

        -h Prints a help message explaining program usage.
        -v Enables printing of decoding statistics.
        -i Allows user to specify an input file path. Default is stdin
        -o Allows user to specify an output file path. Default is stdout

        $ ./error -[h] -[s <seed>] -[e <error_rate>]

        -h Prints a help message explaining program usage.
        -s Allows user to specify random seed.
        -e Allows user to specify error rate.

        $ ./entropy -[h] < [input (reads from stdin)]

        -h Prints a help message explaining program usage.

## Cleaning

        $ make clean
