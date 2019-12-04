#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

void parse_DQT(unsigned char *global_ptr, int seg_length)
{
    extern DQTable DQ_TABLE[4];
    int table_num;
    for (int i = 0; i < (seg_length / 65); i++)
    {
        if (((*(global_ptr + 65 * i)) & 0x0f) > 4)
        {
            cout << "Too many DQT table";
            exit(1);
        }
        if (((*(global_ptr + 65 * i)) & 0xf0) != 0)
        {
            cout << "DQT precision not support yet";
            exit(1);
        }
        table_num = *(global_ptr + 65 * i) & 0x0f;
        DQ_TABLE[table_num].precision = (*(global_ptr + 65 * i)) & 0xf0;
        DQ_TABLE[table_num].imfo = (*(global_ptr + 65 * i)) & 0x0f;
    }
}

void parse_SOF0(unsigned char *global_ptr)
{
    extern IMGINFO IMG;
    COM tmp;
    IMG.bitnum = *global_ptr;
    IMG.img_height = (*(global_ptr + 1) << 8) + (*(global_ptr + 2));
    IMG.img_width = (*(global_ptr + 3) << 8) + (*(global_ptr + 4));
    IMG.component_num = *(global_ptr + 5);
    for (int i = 0; i < IMG.component_num; i++)
    {
        tmp.num = *(global_ptr + 6 + i * 3);
        tmp.hor_sample = (*(global_ptr + 7 + i * 3) & 0xf0) >> 4; //一定要注意移位4，血的教训
        tmp.vet_sample = *(global_ptr + 7 + i * 3) & 0x0f;
        tmp.DQT_num = *(global_ptr + 8 + i * 3);
        IMG.max_hor_sample = max(IMG.max_hor_sample, tmp.hor_sample);
        IMG.max_vet_sample = max(IMG.max_vet_sample, tmp.vet_sample);
        IMG.com_info.push_back(tmp);

        //cout << tmp.num + 0 << tmp.hor_sample + 0 << tmp.vet_sample + 0 << tmp.DQT_num + 0 << " ";
    }
}

void parse_DHT(unsigned char *global_ptr, int length)
{
    unsigned char counts[16];
    extern Huffman_tree Huffman_table[8];
    unsigned int count_num = 0;
    length -= 2;
    unsigned int code_value = 0;
    bool shift = false;
    bool first = true; //第一个节点没有1
    Huffman_node tmp;
    while (length) //读入哈夫曼表的数据
    {
        // cout << "length:" << length << endl;
        // cout << *(global_ptr) + 0 << endl;
        unsigned char type = ((*(global_ptr)) & 0xf0) >> 4;
        unsigned char table_flag = (*(global_ptr)&0x0f);
        unsigned char table_num = (type << 1) + table_flag; //前两个表是DC表，后面六个表是AC表
        Huffman_table[table_num].node_num = table_flag;
        Huffman_table[table_num].node_info = type;
        Huffman_table[table_num].effect = true;
        count_num = 0; //统计各个长度的节点总数
        code_value = 0;
        //cout << table_num + 0;
        if (table_num > 7)
        {
            wrong("There are too many tables");
        }
        for (auto x : counts)
        {
            x = 0; //counts表至为0
        }
        for (int codelen = 1; codelen < 17; codelen++)
        {
            counts[codelen - 1] = *(global_ptr + codelen);
            count_num += *(global_ptr + codelen);
        }

        global_ptr += 17;
        first = true;
        for (int i = 0; i < 16; i++)
        {
            shift = false;
            while (counts[i]) //计算哈夫曼节点的value值
            {
                tmp.weight = *(global_ptr);
                code_value += 1;
                if (first) //第一个节点为0
                {
                    tmp.value--;
                    code_value--;
                    first = false;
                }
                if (!shift)
                {
                    code_value = code_value << 1;
                    shift = true;
                    Huffman_table[table_num].length_min[i] = code_value; //为了在decode过程中找到合适的length
                }
                tmp.value = code_value;
                counts[i]--;
                global_ptr++;
                Huffman_table[table_num].data[i].push_back(tmp);
            }
            Huffman_table[table_num].length_max[i] = code_value;
        }
        length -= (count_num + 17); //info:1,data:count_num,length:16
    }
    for (int i = 0; i < 8; i++)
    {
        if (Huffman_table[i].effect)
        {
            for (int j = 0; j < 16; j++)
            {
                for (auto x : Huffman_table[i].data[j])
                {
                    cout << "node value is" << x.value << "weight is" << x.weight + 0 << "length is" << j << endl;
                }
                cout << endl;
            }
            for (auto y : Huffman_table[i].length_min)
            {
                cout << "min valueis" << y << endl;
            }
            for (auto y : Huffman_table[i].length_max)
            {
                cout << "max value is" << y << endl;
            }
        }
    }
}

void parse_AP(unsigned char *global_ptr, int length)
{
    extern IDE_INFO IDE;
    if ((IDE.Jpeg_mark = *((unsigned int *)global_ptr)) != 0x4649464a) //此处一定要注意大小端的问题，切记切记，默认小端
    {
        wrong("This is not a JPEG");
    }
    global_ptr += 6;
    IDE.hor_reso = ((0 | ((*global_ptr))) << 4) | (*(global_ptr + 1));
    global_ptr += 2;
    IDE.ver_reso = ((0 | ((*global_ptr))) << 4) | (*(global_ptr + 1));
    global_ptr += 2;
    IDE.hor_pit = *(global_ptr);
    global_ptr++;
    IDE.ver_pit = *(global_ptr);
    global_ptr++;
    //cout << IDE.hor_reso + 0 << IDE.ver_reso + 0 << IDE.hor_pit + 0 << IDE.ver_pit + 0;
}

void init_header()
{
    extern unsigned char *global_ptr;
    if (*global_ptr != MA || *(global_ptr + 1) != ST)
    {
        wrong("JPEF START ERR");
    }
    global_ptr += 2;
    bool scan_end = false;
    int seg_length;
    while (!scan_end)
    {
        if (*(global_ptr) == MA)
        {
            // cout << "find" << *(global_ptr + 1) + 0 << endl;

            switch (*(global_ptr + 1))
            {
            case DQT:
                global_ptr += 2;
                //cout << "find DQT" << endl;
                seg_length = (*(global_ptr) << 8) + *(global_ptr + 1);
                //cout << seg_length;
                global_ptr += 2;
                parse_DQT(global_ptr, seg_length);
                global_ptr += (seg_length - 2); //多减了一个length的值
                break;
            case SOF0:
                global_ptr += 2;
                // cout << "find SOF0" << endl;
                seg_length = (*(global_ptr) << 8) + *(global_ptr + 1);
                global_ptr += 2;
                parse_SOF0(global_ptr);
                global_ptr += (seg_length - 2);
                break;
            case DHT:
                global_ptr += 2;
                //cout << "find DHT" << endl;
                seg_length = (*(global_ptr) << 8) + *(global_ptr + 1);
                global_ptr += 2;
                parse_DHT(global_ptr, seg_length);
                break;
            case AP0:
                global_ptr += 2;
                seg_length = (*(global_ptr) << 8) + *(global_ptr + 1);
                global_ptr += 2;
                parse_AP(global_ptr, seg_length);
                break;
            case SOS:
                //cout << "end" << endl;
                scan_end = true;
                break;
            default:
                global_ptr++;
                break;
            }
        }
        else
        {
            global_ptr++;
        }
    }
}