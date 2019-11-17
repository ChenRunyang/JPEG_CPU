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
        tmp.hor_sample = *(global_ptr + 7 + i * 3) & 0xf0;
        tmp.vet_sample = *(global_ptr + 7 + i * 3) & 0x0f;
        tmp.DQT_num = *(global_ptr + 8 + i * 3);
        IMG.com_info.push_back(tmp);
        cout << tmp.num + 0 << tmp.hor_sample + 0 << tmp.vet_sample + 0 << tmp.DQT_num + 0 << " ";
    }
}

void parse_DHT(unsigned char * global_ptr,int length)
{
    ;
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
            cout << "find" << *(global_ptr + 1) + 0 << endl;

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
                cout << "find DHT" << endl;
                seg_length = (*(global_ptr) << 8) + *(global_ptr + 1);
                global_ptr += 2;
                parse_DHT(global_ptr,seg_length);
                break;
            case SOS:
                cout << "end" << endl;
                scan_end = true;
                global_ptr++;
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