# Huffman Compression and AES128 Encryption Project

## Overview
This repository contains the source code and resources for the "Huffman Compression and AES128 Encryption Project" - a comprehensive study focused on efficient data management through the implementation of Huffman compression and AES128 encryption algorithms, specifically applied to text file processing.

## Methodology

### Compression Program
- **Huffman.c**: Implements the Huffman algorithm for compressing text files.
- **unHuffman.c**: Reverses the Huffman compression, restoring the data to its original state.

### Encryption Program
- **panthercrypt.c**: Utilizes the gcrypt library in C for AES128 encryption of compressed data.
- **pantherdec.c**: Decrypts data encrypted by panthercrypt.c, restoring it to its compressed form.

### File Generation and Processing
- A Bash script generates a set of large text files with varying sizes, simulating real-world data.
- The `encode_encrypt.sh` script automates the process of compressing and encrypting these files using the Huffman and AES128 algorithms.
- The `decrypt_decode.sh` script reverses this process, decrypting and decompressing the files.

### Independent Operation
- Each component (compression, encryption, decompression, and decryption) is designed to function both independently and as part of an integrated system.

## Usage

### Running the Programs
Commands to run each program independently from the root of the project folder:

- **Compression (Encode)**: `./compression/encode <path to file>`
- **Decompression (Decode)**: `echo <path to file> | ./compression/decode`
- **Encryption**: `echo <path to file> | ./encryption/encrypt [password]`
- **Decryption**: `./encryption/decrypt <path to file> [password]`

## Repository Contents

- `/compression`: Contains Huffman compression and decompression source files.
- `/encryption`: Contains AES128 encryption and decryption source files.
- `/scripts`: Contains Bash scripts for file generation, compression, encryption, and their reversal.

## Getting Started

To get started with this project, clone the repository and navigate to the respective directories to compile and run the programs as per the usage instructions.

## Contributing

Contributions to this project are welcome. Please refer to the contribution guidelines for more information.

## License

This project is licensed under the [MIT License](LICENSE).

## Contact

For any queries or contributions, please contact [Your Contact Information].
