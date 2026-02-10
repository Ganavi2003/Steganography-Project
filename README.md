# Steganography-C-Project
BMP image steganography encoder/decoder implemented in C.

# Steganography Encoder/Decoder in C

## Project Overview
This project hides and retrieves secret text files inside BMP images using **Least Significant Bit (LSB) manipulation** in C. It demonstrates binary file handling, bitwise operations, and debugging skills.

## Compilation
```bash
gcc encode.c decode.c -o steg

Usage
Encoding
./steg -e input.bmp secret.txt stego.bmp
input.bmp → Source BMP image
secret.txt → Secret file to hide
stego.bmp → Output image with hidden data

Decoding
./steg -d stego.bmp output.txt
stego.bmp → Stego image containing hidden data
output.txt → File where decoded secret will be saved

Files
encode.c, encode.h → Encoding logic
decode.c, decode.h → Decoding logic
types.h, common.h → Custom types and shared constants
