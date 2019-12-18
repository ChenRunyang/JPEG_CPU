#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

extern unsigned char *global_ptr;
extern int unzigzag_table[64];
unsigned char cur_bit = 0x80;
unsigned char pre_dc_num[4] = {0}; //最多有四个comp

int divceil(unsigned short a, unsigned char b)
{
    return (a + b - 1) / b; //防止无法整除的情况
}

int huffman_hash(int t, int s) //t为解析出来的哈夫曼编码，s为编码的长度
{
    if (s == 0)
    {
        return 0;
    }
    else
    {
        int flag = 1;
        flag <<= (s - 1);    //检查符号位，若为0，则为负数
        if ((flag & t) == 0) //为负数
        {
            int tmp = ~0; //-1为所有位为1
            tmp <<= s;
            tmp = tmp | t;
            t = tmp + 1;
        }
        else //为正数，什么都不用做
        {
            ;
        }
        return t;
    }
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

void analysis_para(SOS_Head &para)
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
        tmp_comp.ac_id = ((((*(global_ptr)) & 0xf0) >> 4) + 2); //前两个表是DC表
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
    cout << "end is" << *global_ptr + 0;
}

int ReadSym(const Huffman_tree &table)
{
    int read_sym = 0;
    bool read_end = false;
    int read_length = 0;
    int tmp = 0;
    while ((cur_bit >> tmp) != 0x01)
    {
        tmp++; //注意，这里是cur_bit第tmp+1位
    }
    while (!read_end)
    {
        read_sym = (read_sym << 1) | ((*global_ptr & cur_bit) >> (7 - (read_length - tmp + 7) % 8));                                        //read_length%4方便两个字节进行移位到末尾
        if (read_sym >= table.length_min[read_length] && read_sym <= table.length_max[read_length] && (table.data[read_length].size() > 0)) //在指定哈夫曼表的区间中,表示找到了此哈夫曼节点size>0为了保证第一个节点的长度
        {

            read_end = true; //已找到
        }
        else
        {
            read_length++;
        }
        if (cur_bit == 0x01) //第一个字节检测结束
        {
            cur_bit = 0x80; //检测第二个字节的值
            global_ptr++;
            if (*global_ptr == 0x00 && *(global_ptr - 1) == 0xff)
            {
                global_ptr++; //0xff00跳过此字节
            }
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
    cout << read_sym << endl;
    for (auto x = table.data[read_length].begin(); x < table.data[read_length].end(); x++)
    {
        if (x->value == read_sym)
        {
            return (x->weight);
        }
    }
    cout << "Value not in the huffman table";
    exit(1);
}

int parseHuffman(int s)
{
    int read_sym = 0;
    int read_length = 0;
    int tmp = 0;
    while ((cur_bit >> tmp) != 0x01)
    {
        tmp++; //注意，这里是cur_bit第tmp+1位
    }
    while (s)
    {
        read_sym = (read_sym << 1) | ((*global_ptr & cur_bit) >> (7 - (read_length - tmp + 7) % 8)); //+7防止溢出
        if (cur_bit == 0x01)
        {
            cur_bit = 0x80; //检测第二个字节的值
            global_ptr++;
            if (*global_ptr == 0x00 && *(global_ptr - 1) == 0xff)
            {
                global_ptr++; //0xff00跳过此字节
            }
        }
        else
        {
            cur_bit = cur_bit >> 1;
        }
        read_length++;
        s--;
    }
    if (read_length > 16)
    {
        cout << "find length error";
        exit(1);
    }
    return read_sym;
}

bool Huffmandecode(SOS_Head &para, int comp_num, int mcu_x, int mcu_y, int sam_order, int *imgdata, int MCU_cols, int sample_count, unsigned char *sample)
{
    int s, r, t, m; //s和t分别是读入的Huffman编码位数高四位的值和解析出来的Huffman值
    extern Huffman_tree Huffman_table[8];
    unsigned char start = para.Ss;
    unsigned char end = para.Se;
    int cal_sample = 0;
    for (int cal_tmp = 0; cal_tmp < comp_num; cal_tmp++) //计算在当前comp的前sam值
    {
        cal_sample += sample[cal_tmp];
    }
    int offset = (mcu_y * MCU_cols + mcu_x) * sample_count * 64 + cal_sample * 64 + sam_order * 64;
    while (start <= end) //此处注意end不是64
    {
        if (start == 0) //DC部分进行解码
        {
            if (para.comp_data[comp_num].dc_id > 2)
            {
                cout << "dc table error" << endl;
                exit(1);
            }
            s = ReadSym(Huffman_table[para.comp_data[comp_num].dc_id]); //传入DC_id进行解码
            cout << "Huffman_DC value is:" << s << " ";
            start++;
            t = parseHuffman(s);
            m = huffman_hash(t, s);
            imgdata[offset] = m + pre_dc_num[comp_num];
            pre_dc_num[comp_num] = m; //差分编码记录为前一个DC值
            cout << "parse the DC huffman value is:" << m << endl;
        }
        else
        {
            if (para.comp_data[comp_num].ac_id > 7)
            {
                cout << "ac table error" << endl;
                exit(1);
            }
            cout << "currect global ptr is" << *global_ptr + 0 << endl;
            s = ReadSym(Huffman_table[para.comp_data[comp_num].ac_id]); //传入AC_id进行解码
            if (s == 0x00)
            {
                break;
            }
            cout << "Huffman_AC value is:" << s << " ";
            r = (s >> 4);
            for (int tmp = 0; tmp < r; tmp++)
            {
                imgdata[offset + unzigzag_table[start + tmp]] = 0; //前面填充r个0
            }
            start += r; //高四位作为偏移量
            s = s & 0x0f;
            t = parseHuffman(s);
            m = huffman_hash(t, s);
            imgdata[offset + unzigzag_table[start]] = m;
            start++;
            cout << "parse the AC Huffman value is " << m << endl;
        }
    }
}

void analysis_data(SOS_Head &para)
{
    extern IMGINFO IMG; //检测有几个component
    bool analysis_end = false;
    bool is_interleaved;
    static int MCU_rows = divceil(IMG.img_height, IMG.max_hor_sample * 8);                   //检测纵向行数
    static int MCU_cols = divceil(IMG.img_width, IMG.max_vet_sample * 8);                    //检测横向列数
    const unsigned short scan_bitmask = para.Ah == 0 ? (0xffff << para.Al) : (1 << para.Al); //看是哪种类型的jpeg
    unsigned char sample_hor[3];                                                             //最多只能支持三个色彩通道
    unsigned char sample_vec[3];
    unsigned char sample[3];
    int sample_count = 0;
    for (int i = 0; i < IMG.component_num; i++)
    {
        sample_hor[i] = IMG.com_info[i].hor_sample;
        sample_vec[i] = IMG.com_info[i].vet_sample;
        sample[i] = (sample_hor[i] * sample_vec[i]); //注意1*1为隔点采样
    }
    for (int i = 0; i < IMG.component_num; i++)
    {
        sample_count += sample[i];
    }
    int IMGDATA[MCU_cols * MCU_rows * sample_count * 64];
    for (int mcu_y = 0; mcu_y < MCU_rows; mcu_y++)
    {
        cout << "currect mcu_y is" << mcu_y << endl;
        for (int mcu_x = 0; mcu_x < MCU_cols; mcu_x++)
        {
            cout << "currect mcu_x is" << mcu_x << endl;
            for (int i = 0; i < para.comp_num; i++) //对每个comp进行decode
            {
                cout << "Now the globale ptr is " << *global_ptr + 0 << "OKOK" << endl;
                cout << "Now the DHT num is"
                     << "DC:" << para.comp_data[i].dc_id << " AC:" << para.comp_data[i].ac_id << endl;
                //int nblock_y=IMG.com_info[i].vet_sample;
                //int nblock_x=IMG.com_info[i].hor_sample;
                for (int sam_order = 0; sam_order < sample[i]; sam_order++)
                {
                    Huffmandecode(para, i, mcu_x, mcu_y, sam_order, IMGDATA, MCU_cols, sample_count, sample);
                }
            }
        }
    }
    for (int i = 0; i < 64; i++)
    {
        cout << IMGDATA[i] << " ";
    }
}

void scan_data()
{
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
                analysis_para(scan_para);
                analysis_data(scan_para);
                global_ptr++; //最后一个不为字节不为oxff
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
    }
}