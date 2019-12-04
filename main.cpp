#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

unsigned char *global_ptr;
DQTable DQ_TABLE[4]; //最多储存四张DQT表
IMGINFO IMG;
IDE_INFO IDE;

int zigzag_table[8][8] =
    {
        {0, 1, 5, 6, 14, 15, 27, 28},
        {2, 4, 7, 13, 16, 26, 29, 42},
        {3, 8, 12, 17, 25, 30, 41, 43},
        {9, 11, 18, 24, 31, 40, 44, 53},
        {10, 19, 23, 32, 39, 45, 52, 54},
        {20, 22, 33, 38, 46, 51, 55, 60},
        {21, 34, 37, 47, 50, 56, 59, 61},
        {35, 36, 48, 49, 57, 58, 62, 63}};

Huffman_tree Huffman_table[8];

int main(int argc, char **argv)
{
    int sz;
    unsigned char *buf;

    if (argc <= 1)
    {
        wrong("Please input the jpeg full name");
    }
    FILE *input_file;                  //usr input the jpeg file
    input_file = fopen(argv[1], "rb"); //only one jpeg is allowed in this demo
    if (input_file == NULL)
    {
        wrong("jpeg file open error");
    }
    fseek(input_file, 0, SEEK_END);
    sz = (int)ftell(input_file);
    buf = (unsigned char *)malloc(sz);
    cout << sz << " ";
    if (!buf)
    {
        wrong("Not enough mem");
    }
    rewind(input_file);
    sz = (int)fread(buf, 1, sz, input_file);
    fclose(input_file);

    global_ptr = (unsigned char *)(buf);
    init_header();
    scan_data();
}