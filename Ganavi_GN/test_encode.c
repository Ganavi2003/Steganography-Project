#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
#include <string.h>

int main(int argc, char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    int ret = check_operation_type(argv);
    if(ret ==  e_unsupported)
    {
        printf("error: No argument passed\n");
        return 1;
    }
    if(ret == e_encode)
    {
        int ret1 = read_and_validate_encode_args(argv, &encInfo);
        if(ret1 == e_failure)
        {
            printf("invalid no of argument\n");
            return 1;
        }

        // start endoding
        int ret = do_encoding(&encInfo);
        if(ret == e_failure)
        {
            printf("failure encoding");
            return 1;
        }
        else{
            printf("Encoding is successfull..\n");
            return 0;
        }
    }
    if(ret == e_decode)
    {
        int ret = read_and_validate_decode_args(argv, &decInfo);
        if(ret == e_failure)
        {
            printf("invalid no of argument\n");
            return 1;
        }

        //start decoding
        int ret1 = do_decoding(&decInfo);
        if(ret1 == e_failure)
        {
            printf("failure decoding");
            return 1;
        }
        else{
            printf("Decoding is successfull..\n");
            return 0;
        }
    }
}

OperationType check_operation_type(char *argv[])
{
    if(argv[1] == NULL)
    {
        return e_unsupported;
    }
    else if(strcmp(argv[1], "-e") == 0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}