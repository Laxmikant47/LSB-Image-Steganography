#include <stdio.h>
#include<string.h>
#include <stdlib.h>
#include "decode.h"


Status do_decoding(DecodeInfo *decInfo, char* argv[], int argc)
{
    printf("\nINFO: ## Decoding Procedure Started ##\n");     
    // read and validate
    if(read_and_validate_decode_args(argv, decInfo, argc)== e_success)
    {
        printf("Read and validation completed\n");
    }
    else
    {
        printf("Read and validation Failed\n");
        return e_failure;
    }

    // opening files
    printf("INFO: Opening required files\n");
    if (_open_files(decInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return e_failure;
    }
    else
    {
        printf("INFO: Opened %s\n",decInfo->stego_image);
        // printf("INFO: Opened %s\n",decInfo->secret_file_name);
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }
    //cheking for magic string
    printf("Enter magic string: ");
    scanf("%2s", decInfo->magic_string_usr);
    if(decode_magic_string(decInfo->magic_string , decInfo)==e_success)
    {
        printf("INFO: Magic String Decoded Successfully\n");
    }
    else
    {
        printf("ERROR: Magic String Decoding Failed\n");
        return e_failure;
    }
    
    if(decode_extn( decInfo)==e_success)
    {
        printf("INFO: Extention Decoded\n");
    }
    else
    {
        printf("ERROR: Extention\n");
        return e_failure;
    }
    if(open_secrete_file(decInfo)==e_success)
    {
        printf("INFO: %s File Opened\n", decInfo->secret_file_name);
    }
    else
    {
        printf("ERROR: File Opend Failed\n");
        return e_failure;
    }
    if(decode_secret_data(decInfo)== e_success)
    {
        printf("INFO: Decoded Secret Data Successfully\n");
    }
    else
    {
        printf("ERROR: Decode Secret Data Failed\n");
        return e_failure;
    }
    fclose(decInfo->fptr_out_file);
    fclose(decInfo->fptr_stego);
    return e_success;
}

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo, int argc)
{
    if(argc<3 || argc>4)
    {
        return e_failure;
    }
    if(argv[2] != NULL && strcmp(strchr(argv[2], '.'), ".bmp") == 0)
    {
        decInfo->stego_image =argv[2];
    }
    else
    {
        printf("ERROR: Input image file should be a .bmp file\n");
        return e_failure;
    }

    decInfo->secret_file_name = malloc(sizeof(char)*100);
    if (argv[3] != NULL)
    {
        if((strchr(argv[3], '.') != NULL ))
        {
            decInfo->secret_file_name = strtok(argv[3], ".");
            printf("%s\n", decInfo->secret_file_name);
        }
        else
        {
           strcpy(decInfo->secret_file_name,argv[3]);
        }
    }
    else
    {
        printf("File not mentioned creating default output\n");
        strcpy(decInfo->secret_file_name,"output");
    }
    return e_success;
}

Status _open_files(DecodeInfo *decInfo)
{
    // Src Image file
    decInfo->fptr_stego = fopen(decInfo->stego_image, "r");
    
    // Do Error handling
    if (decInfo->fptr_stego == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image);
    	return e_failure;
    }
    return e_success;
}

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego, 54, SEEK_SET);
    decInfo->magic_string = malloc(2+1);
    decInfo->magic_string = decode_string(2, decInfo);
    // printf("%s hello",decInfo->magic_string);
    if(strcmp(decInfo->magic_string_usr ,decInfo->magic_string)!=0)
    {
        printf("Invalid magic string!!\n");
        return e_failure;
    }
    return e_success;
}

void decode_len(DecodeInfo* decInfo, int* len)
{
    *len = 0;
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_stego);
    for(int i=0; i<32; i++)
    {
        *len |= (buffer[i]&1)<<i;
    }
    //printf("len of extension = %d\n", *len);
}
char* decode_string(int len, DecodeInfo* decInfo)
{
    char arr[8];
    decInfo->temp = (char*)malloc(len+1);
    for(int i=0; i<len; i++)
    {
        fread(arr, sizeof(char), 8, decInfo->fptr_stego);
        decInfo->temp[i] = 0;
        for(int j=0; j<8; j++)
        {
            decInfo->temp[i] |= (arr[j] & 1) << j;
        }
    }
    decInfo->temp[len] = '\0';
    return decInfo->temp;
}

Status decode_extn(DecodeInfo* decInfo)
{
    decode_len(decInfo, &decInfo->file_extn_len);
    char*extn = (char*)malloc(decInfo->file_extn_len+1);
    extn = decode_string(decInfo->file_extn_len, decInfo);
    // printf("%s %s",extn,decInfo->secret_file_name);
    // cancatinating the extenion of file
    strcat(decInfo->secret_file_name, extn);

    // printf("%s",decInfo->secret_file_name);
    return e_success;
}

Status open_secrete_file(DecodeInfo* decInfo)
{
    decInfo->fptr_out_file = fopen(decInfo->secret_file_name, "w");
    if(decInfo->fptr_out_file == NULL)
    {
        printf("ERROR: opening %s file", decInfo->secret_file_name);
        return e_failure;
    }
    return e_success;
}
Status decode_secret_data(DecodeInfo * decInfo)
{
    decode_len(decInfo, &decInfo->secret_msg_len);
    decInfo->secret_msg = (char*)malloc(decInfo->secret_msg_len+1);
    decInfo->secret_msg = decode_string(decInfo->secret_msg_len, decInfo);
    fwrite(decInfo->secret_msg, 1, decInfo->secret_msg_len, decInfo->fptr_out_file);
    return e_success;
}

