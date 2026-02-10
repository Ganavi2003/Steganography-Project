#include<stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
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
    decInfo->stego_fname = argv[2];

    if (argv[3] == NULL) 
    { 
        printf("Created default output file\n"); 
        strcpy(decInfo->dest_fname, "output"); 
    } 
    else 
    { 
        if (strchr(argv[3], '.') != NULL) 
        { 
            printf("You have entered invalid output file name\n");
            return e_failure;
        } 
        else 
        { 
            strcpy(decInfo->dest_fname,argv[3]);
        } 
    }
    return e_success;
}
Status do_decoding(DecodeInfo *decInfo )
{
    decInfo->fptr_stego = fopen(decInfo->stego_fname, "rb");
    if (decInfo->fptr_stego == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_fname);

    	return e_failure;
    }

    skip_bmp_header(decInfo->fptr_stego);

    char magic_string[50];
    decode_magic_string(decInfo->fptr_stego,magic_string);
    
    char user_magic_string[50];
    printf("Enter the magic string you know to proceed decoding:\n");
    scanf("%s",user_magic_string);
    if(strcmp(user_magic_string, magic_string) != 0)
    {
        return e_failure;
    }

    int ext_size;
    decode_extn_size(decInfo->fptr_stego,&ext_size);

    char extn[10];
    decode_extn(decInfo->fptr_stego,extn,ext_size);

    strcat(decInfo->dest_fname, extn);
    
    decInfo->fptr_dest = fopen(decInfo->dest_fname, "wb");
    if (decInfo->fptr_dest == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->dest_fname);

    	return e_failure;
    }

    int file_size;
    decode_secrete_file_size(decInfo->fptr_stego, &file_size);

    decode_sec_data(decInfo->fptr_stego, decInfo->fptr_dest, file_size);
    fclose(decInfo->fptr_stego);

    return e_success;
}
void skip_bmp_header(FILE *fptr_stego)
{
    fseek(fptr_stego,54,SEEK_SET);
}

char lsb_to_byte(char *buffer)
{
    char ch = 0; 
    for (int i = 0; i < 8; i++) 
    { 
        ch = (ch << 1) | (buffer[i] & 1); 
    } 
    return ch;
}

int lsb_to_size(char *buffer)
{
    int size = 0; 
    for (int i = 0; i < 32; i++) 
    { 
        size = (size << 1) | (buffer[i] & 1); 
    } 
    return size;
}

Status decode_magic_string(FILE *stego,char *magic_string)
{
    for (int i = 0; i < 2; i++)
    {
        char temp[8];
        fread(temp,8,1,stego);
        magic_string[i] = lsb_to_byte(temp);
    }
    magic_string[2] = '\0';
    return e_success; 
}

Status decode_extn_size(FILE *stego,int *ext_size)
{
    char temp[32];
    fread(temp,32,1,stego);
    *ext_size = lsb_to_size(temp);
    return e_success; 
}

Status decode_extn(FILE *stego,char *ext,int ext_size)
{
    for(int i = 0 ; i <  ext_size  ; i++ )
    {
        char temp[8];
        fread(temp,8,1,stego);
        ext[i] = lsb_to_byte(temp);
    }
    ext[ext_size] = '\0';
    return e_success; 
}

Status decode_secrete_file_size(FILE *stego, int *file_size)
{
    char temp[32];
    fread(temp,32,1,stego);
    *file_size = lsb_to_size(temp);
    return e_success; 
}

Status decode_sec_data(FILE *stego, FILE *fptr_dest, int file_size)
{
    char temp[8];
    for (int i = 0; i < file_size; i++)
    {
        fread(temp, 8, 1, stego);
        char ch = lsb_to_byte(temp);
        fputc(ch, fptr_dest);
    }
    return e_success;
}