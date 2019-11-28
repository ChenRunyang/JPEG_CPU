#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

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
        cout << "global is" << *global_ptr + 0;
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
    cout << "Ss is" << para.Ss + 0 << "Se is" << para.Se + 0 << "Ah is" << para.Ah << "Al is" << para.Al;
    */
}

void analysis_data(unsigned char *global_ptr, SOS_Head &para)
{
    extern Huffman_tree Huffman_table[8]; //前两个表是DC表，后面6个表是AC表
    extern IMGINFO IMG;                   //检测有几个component
    bool analysis_end = false;
    while (!analysis_end)
    {
        ;
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