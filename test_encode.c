#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"


int main(int argc, char * argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    uint img_size;

    if(argc<3 || argc>5)
    {
        printf("Error: invalid command input\n");
        return e_failure;
    }
    
    if(check_operation_type(argv) ==  e_encode)
    {
        if(do_encoding(&encInfo, argv) == e_success)
        {
            printf("INFO: ## Encoding Done Successfully ##\n");
        }
        else
        {
            printf("INFO: ## Encoding Failed!! ##\n");
        }
    }
    else if(check_operation_type(argv) ==  e_decode)
    {
        if(do_decoding(&decInfo, argv, argc)== e_success)
        {
            printf("INFO: ## Decoding Done Successfully ##\n");
        }
        else
        {
            printf("INFO: ## Decoding Failed!! ##\n");
        }
    }
    else
    {
        printf("Invalid operation type\n");
    }
    return 0;
}
