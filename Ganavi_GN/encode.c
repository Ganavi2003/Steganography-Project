#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include <string.h>

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if(argv[2] == NULL)
    {
        printf("You have not entered image file name\n");
        return e_failure;
    }
    if(strstr(argv[2],".bmp") == NULL)
    {
        printf("You have entered invalid image file name\n");
        return e_failure;
    }
    encInfo->src_image_fname = argv[2];

    if(argv[3] == NULL)
    {
        printf("You have not entered secret file name\n");
        return e_failure;
    }
    if(strstr(argv[3],".txt") == NULL)
    {
        printf("You have entered invalid secret file name\n");
        return e_failure;
    }
    encInfo->secret_fname = argv[3];

    if(argv[4] == NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    else
    {
    if(strstr(argv[4], ".bmp") == NULL)
    {
        printf("Invalid output image file\n");
        return e_failure;
    }
    encInfo->stego_image_fname = argv[4];
}
    char *chr;
    chr = strchr(encInfo->secret_fname,'.');
    strcpy(encInfo->extn_secret_file,chr);

    return e_success;
}
Status do_encoding(EncodeInfo *encInfo)
{
    int ret = open_files(encInfo); 
    if(ret == e_failure)
    {
       printf("open file fail");
       return e_failure;
    }

    Status ret1 = check_capacity(encInfo); //capacity checking
    if(ret1 == e_failure)
    {
       return e_failure;
    }

    Status ret2 = copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image); // copy header of scr file to dest file
    if(ret2 == e_failure)
    {
       return e_failure;
    }

    encode_magic_string(MAGIC_STRING,encInfo);

    encode_secret_file_extn_size(strlen(encInfo->extn_secret_file) ,encInfo);

    encode_secret_file_extn(encInfo->extn_secret_file,encInfo);

    fseek(encInfo->fptr_secret, 0, SEEK_END); 
    long sec_file_size = ftell(encInfo->fptr_secret); 
    rewind(encInfo->fptr_secret); 
    encInfo->size_secret_file = sec_file_size;
    encode_secret_file_size(encInfo->size_secret_file,encInfo);

    encode_secret_file_data(encInfo);

    copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image);

    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}
Status check_capacity(EncodeInfo *encInfo)
{
    int magic_str_len = strlen(MAGIC_STRING);
    
    // secret file extension size checking
    int ext_len = strlen(encInfo->extn_secret_file);
    
    // secret file size checking
    fseek(encInfo->fptr_secret, 0, SEEK_END); 
    long file_size = ftell(encInfo->fptr_secret); 
    rewind(encInfo->fptr_secret); 
    encInfo->size_secret_file = file_size;
    
    // finding length 
    int count = (( magic_str_len + sizeof(int) + ext_len + file_size + sizeof(int) ) * 8 )+ 54;
    // printf("count : %d\n",count);
    int src_file_size = get_image_size_for_bmp(encInfo->fptr_src_image);
    
    if(count <= src_file_size )
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char temp[54]; 
    rewind(fptr_src_image); 
    fread(temp, 54, 1, fptr_src_image); 
    fwrite(temp, 54, 1, fptr_stego_image); 
    return e_success;
}

void encode_byte_to_lsb(char data, char *imagebuffer)
{
    for (int i = 0; i < 8; i++)
    {
        char mask = 1 << (7 - i);
        char bit = data & mask;
        imagebuffer[i] = imagebuffer[i] & 0xFE;
        bit = bit >> ( 7 - i);
        imagebuffer[i] = imagebuffer[i] | bit;
    }
}

void encode_size_to_lsb(int data, char *imagebuffer)
{
    for (int i = 0; i < 32; i++)
    {
        char mask = 1 << (31 - i);
        char bit = data & mask;
        imagebuffer[i] = imagebuffer[i] & 0xFE;
        bit = bit >> ( 31 - i);
        imagebuffer[i] = imagebuffer[i] | bit;
    }
}

Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char temp[8];
    for (int i = 0; i < 2; i++)
    {
        fread(temp, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], temp);
        fwrite(temp, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_extn_size(int file_extn_size , EncodeInfo *encInfo)
{
    char temp[32];
    fread(temp, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(file_extn_size, temp);
    fwrite(temp, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char temp[8];
    for (int i = 0; i < strlen(file_extn)  ; i++)
    {
        fread(temp, 8,1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], temp);
        fwrite(temp, 8,1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status encode_secret_file_size(int sec_file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(sec_file_size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char temp[8];
    char ch;
    while ((ch = fgetc(encInfo->fptr_secret)) != EOF)
    {
        fread(temp, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb((char)ch, temp);
        fwrite(temp, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    while ((ch = fgetc(fptr_src)) != EOF)
    {
        fputc(ch, fptr_dest);
    }
    return e_success;
}
