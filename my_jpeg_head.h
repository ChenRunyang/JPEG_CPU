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

typedef struct identify //没什么用
{
    unsigned int Jpeg_mark;
    unsigned short ver_reso;
    unsigned short hor_reso;
    unsigned char ver_pit;
    unsigned char hor_pit;
} IDE_INFO;

typedef struct dqtable
{
    unsigned int imfo : 4;
    unsigned int precision : 4;
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
    unsigned short img_height;
    unsigned short img_width;
    unsigned char component_num;
    unsigned char max_hor_sample;
    unsigned char max_vet_sample;
    std::vector<COM> com_info;
    image() : max_hor_sample(0), max_vet_sample(0) {}
} IMGINFO;

typedef struct H_node
{
    int value;
    unsigned char weight;
    H_node() : value(0), weight(0){};
} Huffman_node;

typedef struct H_tree
{
    bool effect;
    unsigned char node_num;
    unsigned char node_info;
    unsigned int length_max[16];
    unsigned int length_min[16];
    std::vector<Huffman_node> data[16];
    H_tree() : effect(false), node_num(0), node_info(0){};
} Huffman_tree;

typedef struct comp
{
    unsigned char comp_id;
    unsigned int ac_id : 4;
    unsigned int dc_id : 4;
} COMP;

typedef struct sos_header
{
    unsigned int seg_length;
    unsigned char comp_num;
    std::vector<COMP> comp_data;
    unsigned char Ss;
    unsigned char Se;
    unsigned int Ah : 4;
    unsigned int Al : 4;
} SOS_Head;

typedef struct huffmannode
{
    unsigned int value;
    unsigned int distance;
} Huffmandata;

extern void init_header();
extern void wrong(const char *name);
extern void scan_data();

using namespace std;