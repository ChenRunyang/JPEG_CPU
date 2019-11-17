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
    char weight;
}Huffman_node;

typedef struct Huf_tree
{
    int node_num;
    unsigned char node_info;
    std::vector<Huffman_node> data;
}


extern void init_header();
extern void wrong(const char *name);

using namespace std;