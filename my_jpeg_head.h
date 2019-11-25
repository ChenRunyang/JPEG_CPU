#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <OpenCL/cl.h>
#include <vector>

#define SOS 0xda
#define MA 0xff
#define ST 0xd8
#define DQT 0xdb
#define DHT 0xc4
#define AP0 0xe0
#define APC 0xec
#define APE 0xee
#define SOF0 0xc0
#define END 0xd9

typedef struct identify
{
    unsigned int Jpeg_mark;
    unsigned short ver_reso;
    unsigned short hor_reso;
    unsigned char ver_pit;
    unsigned char hor_pit;
} IDE_INFO;

typedef struct dqtable
{
    unsigned char imfo : 4;
    unsigned char precision : 4;
    char data[64] = {0};
} DQTable;

typedef struct component_info
{
    char num;
    unsigned char hor_sample : 4;
    unsigned char vet_sample : 4;
    char DQT_num;
} COM;

typedef struct image
{
    unsigned char bitnum;
    short img_height;
    short img_width;
    char component_num;
    std::vector<COM> com_info;
} IMGINFO;

typedef struct H_node
{
    int value;
    unsigned char weight;
    unsigned char length;
    H_node() : value(0), weight(0), length(0){};
} Huffman_node;

typedef struct H_tree
{
    bool effect;
    unsigned char node_num;
    unsigned char node_info;
    std::vector<Huffman_node> data;

    H_tree() : effect(false), node_num(0), node_info(0){};
} Huffman_tree;

extern void init_header();
extern void wrong(const char *name);
extern void scan_data();

using namespace std;