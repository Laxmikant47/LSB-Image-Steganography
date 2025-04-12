#ifndef DECODE_H
#define DECODE_H
#include "common.h"
#include "types.h"

#include<stdio.h>
/* 
 * Structure to store information required for
 * decoding image file to destination text file
 * Info about output and image file 
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 8

typedef struct _DecodeInfo
{
    /* Stego Image Info */
    char *stego_image;
    FILE *fptr_stego;
    /*magic strings*/
    char *magic_string;
    char magic_string_usr[6];
    char* temp;
    // entn len
    int file_extn_len;
    
    //output secret file
    char *secret_file_name ;
    int secret_msg_len;
    char* secret_msg;
    FILE *fptr_out_file; 
}DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo, int arg);

Status _open_files(DecodeInfo *decInfo);

Status do_decoding(DecodeInfo *decInfo, char* argv[], int argc);

Status decode_magic_string(char *magic_string, DecodeInfo *decInfo);

Status decode_extn(DecodeInfo* decInfo);

// status decode_extn

// decode secret file size

Status decode_secret_data(DecodeInfo * decInfo);
char* decode_string(int len, DecodeInfo* decInfo);
void decode_len(DecodeInfo* decInfo, int* len);
Status open_secrete_file(DecodeInfo* decInfo);

#endif

