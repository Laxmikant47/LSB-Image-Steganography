#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include "common.h"
/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 *
 */

 Status do_encoding(EncodeInfo *encInfo, char* argv[])
{  
    printf("\nINFO: ## Encoding Procedure Started ##\n");     
    // read and validate
    if(read_and_validate_encode_args(argv, encInfo)== e_success)
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
    if (open_files(encInfo) == e_failure)
    {
    	printf("ERROR: %s function failed\n", "open_files" );
    	return e_failure;
    }
    else
    {
        printf("INFO: Opened SkeletonCode/%s\n",encInfo->src_image_fname);
        printf("INFO: Opened %s\n",encInfo->secret_fname);
        printf("INFO: Opened %s\n", encInfo->stego_image_fname);
    	printf("SUCCESS: %s function completed\n", "open_files" );
    }

    /*check capacity*/
    printf("INFO: Checking for SkeletonCode/%s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);
    if(check_capacity(encInfo) == e_success)
    {
        printf("INFO: Done. Found OK\n");
    }
    else
    {
        printf("Capacity Failed\n");
        return e_failure;
    }

    printf("INFO: Copying Image Header\n");
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        printf("Header Copying Failed\n");
        return e_failure;
    }
    // encoding magic string
    printf("INFO: Encoding Magic String Signature\n");
    if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("INFO: Done\n");
    }
    else
    {
        return e_failure;
    }

    // encoding secret file extn
    printf("INFO: Encoding %s File Extenstion\n", encInfo->secret_fname);
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo)== e_success)
    {
        printf("INFO: Encoding %s File Extenstion\n",encInfo->secret_fname);
        printf("INFO : Done\n");
    }
    // encInfo->size_secret_file = 25;
    if (encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success)
    {
        printf("INFO : Encoding %s File Size\n",encInfo->secret_fname);
        printf("INFO : Done\n");
    }
    if(encode_secret_file_data(encInfo)== e_success)
    {
        printf("INFO: Encoding %s File Data\n", encInfo->secret_fname);
        printf("INFO : Done\n");
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image)== e_success)
    {
        printf("INFO: Copying Left Over Data\n");
        printf("INFO : Done\n");
    }
    else
    {
        printf("ERROR : unable to copy remaining file data\n");
        return e_failure;
    }
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    rewind(fptr_image);

    // Return image capacity
    return (width * height * 3) + 54 + 1; // 54 + 1 (header file size + EOF)
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    // printf("%s\n", encInfo->src_image_fname);
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

// check whether user wants to perform encoding or decoding
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1] ,"-e")==0)
    {
        return e_encode;
    }
    else if(strcmp(argv[1] ,"-d")==0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}

Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // validation for .bmp
    if(argv[2] != NULL && strcmp(strchr(argv[2], '.'), ".bmp") == 0)
    {
        encInfo->src_image_fname=argv[2];
    }
    else
    {
        printf("ERROR: Input image file should be a .bmp file\n");
        return e_failure;
    }

    /* validation for .txt*/
    if(argv[3] != NULL && strchr(argv[3], '.') != NULL)
    {
        encInfo->secret_fname = argv[3];
        encInfo->extn_len_secret = strlen(strchr(encInfo->secret_fname, '.'));
        strcpy(encInfo->extn_secret_file,strchr(encInfo->secret_fname, '.'));
    }
    else
    {
        printf("Invalid File Format\n");
        return e_failure;
    }

    if (argv[4] != NULL &&  strcmp(strchr(argv[4], '.'), ".bmp") == 0 )
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        printf("File not mentioned creating default stego.bmp\n");
        encInfo->stego_image_fname ="stego.bmp";
    }
    return e_success;
}

uint get_file_size(FILE *ptr)
{
    fseek(ptr, 0, SEEK_END);
    return ftell(ptr); 
}

Status check_capacity(EncodeInfo *encInfo)
{
    if((encInfo->size_secret_file = get_file_size(encInfo->fptr_secret))!=0)
    {
        //printf("%ld\n", encInfo->size_secret_file);
        printf("INFO: Done. Not Empty\n");
    }
    // rewind is not completed yet!
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    uint temp = (4 + 4 + 4 + 2 + encInfo->size_secret_file) * 8 ;
    if(encInfo->image_capacity >= temp)
    {
        return e_success;
    }
    else
    { 
        return e_failure; 
    }
}

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src, FILE *fptr_stego)
{
    char header[54];
    fseek(fptr_src, 0, SEEK_SET); //  or rewind(fptr_src);
    fread(header, sizeof(char), 54, fptr_src);
    fwrite(header, sizeof(char), 54, fptr_stego);
    return e_success;
}

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int j=0; j<8; j++)
    {
        image_buffer[j] = (image_buffer[j] & ~ (1)) | ((data >> j) & 1) ;
    }
    return e_success;
}

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    for(int i=0 ; i<size; i++)
    {
        char buff[8] ;
        fread(buff, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], buff);
        fwrite(buff, 8, 1, fptr_stego_image);  
    }
}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image((char *)magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    if(encode_secret_file_extn_size(encInfo) == e_success)
    {
        printf("INFO: Encoding %s File Extenstion size\n", encInfo->secret_fname);
        printf("INFO : Done\n");
    }
    encode_data_to_image((char*)file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image);

}

Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
    // printf("\n%u hi\n",encInfo->extn_len_secret);
    char buff[32];
    fread(buff , 32, 1, encInfo->fptr_src_image);
    for(int i=0; i<32; i++)
    {
        buff[i] = buff[i] & ~(1);
        uint num = encInfo->extn_len_secret & (1<<i);
        num = num >>i;
        buff[i] |= num;
    }
    fwrite(buff, 32, 1, encInfo->fptr_stego_image);
}

/* Encode secret file length */
Status encode_secret_file_size(uint file_size, EncodeInfo *encInfo)
{
    char buff[32];
    fread(buff , 32, 1, encInfo->fptr_src_image);
    for(int i=0; i<32; i++)
    {
        buff[i] = buff[i] & ~(1);
        uint num = file_size & (1<<i);
        num = num >>i;
        // printf("%u",num);
        buff[i] |= num;
    }
    fwrite(buff, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    encInfo->secret_data = (char*)malloc(encInfo->size_secret_file);
    fread(encInfo->secret_data, sizeof(char), encInfo->size_secret_file, encInfo->fptr_secret);
    encode_data_to_image(encInfo->secret_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image);
    return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_stego);
    }
    return e_success;
}

// do encoding
