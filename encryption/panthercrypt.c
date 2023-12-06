#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <gcrypt.h>
#include <unistd.h>

// Declare the EncryptFile function
void EncryptFile(char *file_in, char pw[256]);

// CLI Input

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Please use a valid command: ./panthercrypt [password]");
        return -1;
    }

    // Remove the newline character, if present
    argv[1][strcspn(argv[1], "\n")] = '\0';

    char *file_in = malloc(256);

    if (file_in == NULL)
    {
        perror("Memory allocation failed");
        return 1;
    }

    scanf("%s", file_in);

    if (strcmp(file_in, "Input file cannot be opened.\n") == 0)
    {
        printf("Input file cannot be opened.\n");
        return -1;
    }

    if (strcmp(file_in, "Output file cannot be opened.\n") == 0)
    {
        printf("Output file cannot be opened.\n");
        return -2;
    }

    if (strcmp(file_in, "The file is empty.\n") == 0)
    {
        printf("The file is empty.\n");
        return -2;
    }

    EncryptFile(file_in, argv[1]);

    return 0;
}

void EncryptFile(char *file_in, char pw[256])
{
    FILE *fin, *fout;
    unsigned char *outBuf;
    unsigned char *padBuf;
    char *salt = "NaCl";
    unsigned char key[16]; // Size for AES128 key.
    unsigned char IV[16];  // The IV size for AES is 128 bits (16 bytes).
    gcry_cipher_hd_t hd;
    size_t fileLen, paddedLen;

    gcry_kdf_derive(pw, strlen(pw), GCRY_KDF_PBKDF2, GCRY_MD_SHA512, salt, strlen(salt), 4096, sizeof(key), key);

    // encryption
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

    // Calculate padded length
    paddedLen = ((fileLen + 15) / 16) * 16;

    // Allocate memory for input and output buffer
    outBuf = (unsigned char *)malloc(paddedLen);
    if (!outBuf)
    {
        perror("Memory allocation failed");
        fclose(fin);
        gcry_cipher_close(hd);
        return;
    }

    // Read the entire file into the buffer
    fread(outBuf, 1, fileLen, fin);
    fclose(fin); // Close the input file as we have finished reading it

    // Apply PKCS#7 padding
    unsigned char padValue = paddedLen - fileLen;
    memset(outBuf + fileLen, padValue, padValue);

    // Encrypt the buffer
    err = gcry_cipher_encrypt(hd, outBuf, paddedLen, NULL, 0);
    if (err)
    {
        fprintf(stderr, "Error encrypting data: %s\n", gcry_strerror(err));
        free(outBuf);
        gcry_cipher_close(hd);
        return;
    }

    char output_file_tmp[256]; // Buffer for the output file name
    strcpy(output_file_tmp, file_in);
    char delimiter[] = ".";
    char* token;

    token = strtok(output_file_tmp, delimiter);
    
    char* output_file = (char*)malloc(strlen(".") + strlen(token) + strlen("_encrypted.txt.hzip") + 1);
    strcpy(output_file, ".");
    strcat(output_file, token);
    strcat(output_file, "_encrypted.txt.hzip");

    // Write to output file
    fout = fopen(output_file, "wb");
    if (!fout)
    {
        perror("File opening failed");
        free(outBuf);
        gcry_cipher_close(hd);
        return;
    }
    fwrite(outBuf, 1, paddedLen, fout);
    fclose(fout);

    // free the buffers, and close file descriptors if necessary
    gcry_cipher_close(hd);
    free(outBuf);
    printf("File successfully encrypted!\n");
}