#include"types.h"
typedef struct _DecodeInfo
{
   char *stego_fname;
   FILE *fptr_stego;

   char dest_fname[20];
   FILE *fptr_dest;

}DecodeInfo;
Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo);

void skip_bmp_header(FILE *fptr_stegoo);

Status decode_magic_string(FILE *stego,char *magic_string);

Status decode_extn_size(FILE *stego,int *ext_size);

Status decode_extn(FILE *stego,char *ext,int ext_size);

Status decode_secrete_file_size(FILE *stego,int *file_size);

Status decode_sec_data(FILE *stego, FILE *fptr_dest,int file_size);
