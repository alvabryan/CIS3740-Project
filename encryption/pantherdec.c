#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gcrypt.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <gcrypt.h>

#define BUFFER_SIZE 1024
#define PORT 8080   // Default port for demonstration, should be replaced by argv input
#define SALT_LEN 16 // Adjust according to your encryption
#define IV_LEN 16   // AES block size for AES-128, AES-192, AES-256
#define KEY_LEN 32  // AES-256 Key Length
#define HMAC_LEN 64 // Length of SHA512 HMAC
#define HASH_ALGO GCRY_MD_SHA512
#define KDF_ITERATIONS 10000

void DecryptFile(char *file_in, char pw[256]);
void HandleNetworkConnection(int port, char *filename);
unsigned short GetPortFromArguments(char *portStr);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: pantherdec <inputfile> <password>\n");
        exit(1);
    }

    char *filename = argv[1];

    // Remove the newline character, if present
    argv[2][strcspn(argv[2], "\n")] = '\0';

    if (filename)
    {
        DecryptFile(filename, argv[2]);
    }
    else
    {
        fprintf(stderr, "Invalid file! Please provide a valid file.\n");
        exit(1);
    }

    return 0;
}

void DecryptFile(char *file_in, char pw[256])
{
    FILE *fin, *fout;
    unsigned char *inBuf;
    char *salt = "NaCl";
    unsigned char key[16]; // Size for AES128 key.
    unsigned char IV[16];  // The IV size for AES is 128 bits (16 bytes).
    gcry_cipher_hd_t hd;
    size_t fileLen;
    char file_out[256]; // Output filename buffer, assumes the input filename will be less than 256 characters

    gcry_kdf_derive(pw, strlen(pw), GCRY_KDF_PBKDF2, GCRY_MD_SHA512, salt, strlen(salt), 4096, sizeof(key), key);

    // Decryption setup
    gcry_error_t err = gcry_cipher_open(&hd, GCRY_CIPHER_AES128, GCRY_CIPHER_MODE_CBC, 0);
    if (err)
    {
        fprintf(stderr, "Error opening cipher: %s\n", gcry_strerror(err));
        return;
    }
    err = gcry_cipher_setkey(hd, key, sizeof(key));
    if (err)
    {
        fprintf(stderr, "Error setting key: %s\n", gcry_strerror(err));
        gcry_cipher_close(hd);
        return;
    }
    memset(IV, 0, sizeof(IV));
    *((unsigned short *)IV) = htons(5844); // Use network byte order

    err = gcry_cipher_setiv(hd, IV, sizeof(IV));
    if (err)
    {
        fprintf(stderr, "Error setting IV: %s\n", gcry_strerror(err));
        gcry_cipher_close(hd);
        return;
    }

    // Open input file
    fin = fopen(file_in, "rb");
    if (!fin)
    {
        perror("File opening failed");
        gcry_cipher_close(hd);
        return;
    }

    // Get file size
    fseek(fin, 0, SEEK_END);
    fileLen = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    // Allocate memory for input buffer
    inBuf = (unsigned char *)malloc(fileLen);
    if (!inBuf)
    {
        perror("Memory allocation failed");
        fclose(fin);
        gcry_cipher_close(hd);
        return;
    }

    // Read the entire file into the buffer
    fread(inBuf, 1, fileLen, fin);
    fclose(fin); // Close the input file as we have finished reading it

    // Decrypt the buffer
    err = gcry_cipher_decrypt(hd, inBuf, fileLen, NULL, 0);
    if (err)
    {
        fprintf(stderr, "Error decrypting data: %s\n", gcry_strerror(err));
        free(inBuf);
        gcry_cipher_close(hd);
        return;
    }

    // Remove PKCS#7 padding
    unsigned char padValue = inBuf[fileLen - 1];
    // if (padValue > 0x10 || padValue == 0) { // Padding value should not be greater than 16 (0x10) for AES
    //     fprintf(stderr, "Invalid padding value detected\n");
    //     free(inBuf);
    //     gcry_cipher_close(hd);
    //     return;
    // }
    size_t unpaddedLen = fileLen - padValue;

    char output_file_tmp[256]; // Buffer for the output file name
    strcpy(output_file_tmp, file_in);
    char delimiter[] = ".";
    char* token;

    token = strtok(output_file_tmp, delimiter);
    
    char* output_file = (char*)malloc(strlen(".") + strlen(token) + strlen("_decrypt.txt.hzip") + 1);
    strcpy(output_file, ".");
    strcat(output_file, token);
    strcat(output_file, "_decrypt.txt.hzip");

    // Write to output file
    fout = fopen(output_file, "wb");
    if (!fout)
    {
        perror("File opening failed");
        free(inBuf);
        gcry_cipher_close(hd);
        return;
    }
    fwrite(inBuf, 1, unpaddedLen, fout);
    fclose(fout);

    // free the buffers, and close file descriptors if necessary
    gcry_cipher_close(hd);
    free(inBuf);
    printf("%s\n", output_file);
}
