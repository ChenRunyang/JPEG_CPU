#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

unsigned char *global_ptr;
DQTable DQ_TABLE[4]; //最多储存四张DQT表
IMGINFO IMG;
IDE_INFO IDE;

int zigzag_table[64] =
    {
        0, 1, 5, 6, 14, 15, 27, 28,
        2, 4, 7, 13, 16, 26, 29, 42,
        3, 8, 12, 17, 25, 30, 41, 43,
        9, 11, 18, 24, 31, 40, 44, 53,
        10, 19, 23, 32, 39, 45, 52, 54,
        20, 22, 33, 38, 46, 51, 55, 60,
        21, 34, 37, 47, 50, 56, 59, 61,
        35, 36, 48, 49, 57, 58, 62, 63};

int unzigzag_table[64] =
    {
        0, 1, 8, 16, 9, 2, 3, 10,
        17, 24, 32, 25, 18, 11, 4, 5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13, 6, 7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63};


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
    buf = (unsigned char *)malloc(sz * sizeof(unsigned char));
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