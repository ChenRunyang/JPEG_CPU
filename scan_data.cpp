#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

int divceil(unsigned short a, unsigned char b)
{
    return (a + b - 1) / b; //防止无法整除的情况
}

void init_para(SOS_Head &para)
{

    para.seg_length = 0;
    para.comp_num = 0;
    para.Ah = 0;
    para.Al = 0;
    para.comp_data.clear();
    para.Se = 0;
    para.Ss = 0;
}

void analysis_para(unsigned char *global_ptr, SOS_Head &para)
{
    para.seg_length = (*(global_ptr) << 8) + *(global_ptr + 1);
    global_ptr += 2;
    para.comp_num = *(global_ptr);
    global_ptr++;
    COMP tmp_comp;
    for (int tmp = 0; tmp < (para.comp_num); tmp++)
    {
        tmp_comp.comp_id = *global_ptr;
        global_ptr++;
        //cout << "global is" << *global_ptr + 0;
        tmp_comp.ac_id = (((*(global_ptr)) & 0xf0) >> 4);
        tmp_comp.dc_id = (*(global_ptr)&0x0f);
        global_ptr++;
        para.comp_data.push_back(tmp_comp);
    }
    para.Ss = *global_ptr;
    para.Se = *(global_ptr + 1);
    global_ptr += 2;
    para.Ah = ((*global_ptr & 0xf0) >> 4);
    para.Al = *global_ptr & 0x0f;
    global_ptr++;
    /*for (int i = 0; i < para.comp_num; i++)
    {
        cout << "comp id is" << para.comp_data[i].comp_id + 0 << "ac id is" << para.comp_data[i].ac_id << "dc id is" << para.comp_data[i].dc_id << endl;
    }
    cout << "Ss is" << para.Ss + 0 << "Se is" << para.Se + 0 << "Ah is" << para.Ah << "Al is" << para.Al;*/
}

int ReadSym(unsigned char *global_ptr, Huffman_tree table, unsigned char &cur_bit)
{
    int read_sym = 0;
    bool read_end = false;
    int read_length = 0;
    while (!read_end)
    {
        read_sym = (read_sym << 1) | ((*global_ptr & cur_bit) >> (7 - (read_length % 8)));          //read_length%4方便两个字节进行移位到末尾
        if (read_sym >= table.length_min[read_length] && read_sym <= table.length_max[read_length]) //在指定哈夫曼表的区间中,表示找到了此哈夫曼节点
        {
            read_end = true;
        }
        else
        {
            read_length++;
            if (cur_bit == 0x01) //第一个字节检测结束
            {
                cur_bit = 0x80;
                global_ptr++;
            }
            else
            {
                cur_bit = cur_bit >> 1;
            }
        }
        if (table.effect == false)
        {
            cout << "Not init yet" << endl;
            exit(1);
        }
        for (auto x = table.data[read_length].begin(); x < table.data[read_length].end(); x++)
        {
            if (x->value == read_sym)
            {
                return (x->value);
            }
            else
            {
                return 0;
            }
        }
    }
}

bool Huffmandecode(unsigned char *global_ptr, SOS_Head &para, int comp_num, unsigned char &cur_bit)
{
    int s, r, t; //s和t分别是读入的Huffman编码位数高四位的值和解析出来的Huffman值
    extern Huffman_tree Huffman_table[8];
    if (para.Ss == 0) //DC部分进行解码
    {
        s = ReadSym(global_ptr, Huffman_table[para.comp_data[comp_num].dc_id], cur_bit); //传入DC_id进行解码
        cout << "Huffman_DC value is:" << s;
    }
    else
    {
        cout << "Huffman_AC value" << endl;
    }
}
void analysis_data(unsigned char *global_ptr, SOS_Head &para)
{
    extern IMGINFO IMG; //检测有几个component
    cout << "img height is" << IMG.img_height << IMG.max_hor_sample + 0;
    bool analysis_end = false;
    bool is_interleaved;
    cout << "max hor sample is" << IMG.max_hor_sample + 0 << IMG.max_vet_sample + 0 << IMG.img_height << IMG.img_width;
    int MCU_rows = divceil(IMG.img_height, IMG.max_hor_sample * 8);                          //检测纵向行数
    int MCU_cols = divceil(IMG.img_width, IMG.max_vet_sample * 8);                           //检测横向列数
    unsigned char cur_bit;                                                                   //检测当前是第几位
    const unsigned short scan_bitmask = para.Ah == 0 ? (0xffff << para.Al) : (1 << para.Al); //看是哪种类型的jpeg
    for (int mcu_y = 0; mcu_y < MCU_rows; mcu_y++)
    {
        for (int mcu_x = 0; mcu_x < MCU_cols; mcu_x++)
        {
            for (int i = 0; i < para.comp_num; i++) //对每个comp进行decode
            {
                cur_bit = 0x80;
                Huffmandecode(global_ptr, para, i, cur_bit);
            }
        }
    }
}

void scan_data()
{
    extern unsigned char *global_ptr;
    bool scan_end = false;
    SOS_Head scan_para;
    while (!scan_end)
    {
        if (*(global_ptr) == MA)
        {
            init_para(scan_para);
            switch (*(global_ptr + 1))
            {
            case SOS:
                global_ptr += 2;
                //cout << "find SOS" << endl;
                analysis_para(global_ptr, scan_para);
                analysis_data(global_ptr, scan_para);
                break;

            case END:
                scan_end = true;
                cout << "end" << endl;
                break;
            default:
                global_ptr++;
                break;
            }
        }
        global_ptr++;
    }
}