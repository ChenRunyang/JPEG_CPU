#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

extern unsigned char *global_ptr;
extern int unzigzag_table[64];
unsigned char cur_bit = 0x80;
int pre_dc_num[4] = {0}; //最多有四个comp
extern DQTable DQ_TABLE[4];
const int kIDCTMatrix[64] = {
    8192,
    11363,
    10703,
    9633,
    8192,
    6437,
    4433,
    2260,
    8192,
    9633,
    4433,
    -2259,
    -8192,
    -11362,
    -10704,
    -6436,
    8192,
    6437,
    -4433,
    -11362,
    -8192,
    2261,
    10704,
    9633,
    8192,
    2260,
    -10703,
    -6436,
    8192,
    9633,
    -4433,
    -11363,
    8192,
    -2260,
    -10703,
    6436,
    8192,
    -9633,
    -4433,
    11363,
    8192,
    -6437,
    -4433,
    11362,
    -8192,
    -2261,
    10704,
    -9633,
    8192,
    -9633,
    4433,
    2259,
    -8192,
    11362,
    -10704,
    6436,
    8192,
    -11363,
    10703,
    -9633,
    8192,
    -6437,
    4433,
    -2260,
};

void ComputeDCT(const int *in, const int stride, int *out)
{
    int tmp0, tmp1, tmp2, tmp3, tmp4;

    tmp1 = kIDCTMatrix[0] * in[0];
    out[0] = out[1] = out[2] = out[3] = out[4] = out[5] = out[6] = out[7] = tmp1;

    tmp0 = in[stride];
    tmp1 = kIDCTMatrix[1] * tmp0;
    tmp2 = kIDCTMatrix[9] * tmp0;
    tmp3 = kIDCTMatrix[17] * tmp0;
    tmp4 = kIDCTMatrix[25] * tmp0;
    out[0] += tmp1;
    out[1] += tmp2;
    out[2] += tmp3;
    out[3] += tmp4;
    out[4] -= tmp4;
    out[5] -= tmp3;
    out[6] -= tmp2;
    out[7] -= tmp1;

    tmp0 = in[2 * stride];
    tmp1 = kIDCTMatrix[2] * tmp0;
    tmp2 = kIDCTMatrix[10] * tmp0;
    out[0] += tmp1;
    out[1] += tmp2;
    out[2] -= tmp2;
    out[3] -= tmp1;
    out[4] -= tmp1;
    out[5] -= tmp2;
    out[6] += tmp2;
    out[7] += tmp1;

    tmp0 = in[3 * stride];
    tmp1 = kIDCTMatrix[3] * tmp0;
    tmp2 = kIDCTMatrix[11] * tmp0;
    tmp3 = kIDCTMatrix[19] * tmp0;
    tmp4 = kIDCTMatrix[27] * tmp0;
    out[0] += tmp1;
    out[1] += tmp2;
    out[2] += tmp3;
    out[3] += tmp4;
    out[4] -= tmp4;
    out[5] -= tmp3;
    out[6] -= tmp2;
    out[7] -= tmp1;

    tmp0 = in[4 * stride];
    tmp1 = kIDCTMatrix[4] * tmp0;
    out[0] += tmp1;
    out[1] -= tmp1;
    out[2] -= tmp1;
    out[3] += tmp1;
    out[4] += tmp1;
    out[5] -= tmp1;

    tmp0 = in[5 * stride];
    tmp1 = kIDCTMatrix[5] * tmp0;
    tmp2 = kIDCTMatrix[13] * tmp0;
    tmp3 = kIDCTMatrix[21] * tmp0;
    tmp4 = kIDCTMatrix[29] * tmp0;
    out[0] += tmp1;
    out[1] += tmp2;
    out[2] += tmp3;
    out[3] += tmp4;
    out[4] -= tmp4;
    out[5] -= tmp3;
    out[6] -= tmp2;
    out[7] -= tmp1;

    tmp0 = in[6 * stride];
    tmp1 = kIDCTMatrix[6] * tmp0;
    tmp2 = kIDCTMatrix[14] * tmp0;
    out[0] += tmp1;
    out[1] += tmp2;
    out[2] -= tmp2;
    out[3] -= tmp1;
    out[4] -= tmp1;
    out[5] -= tmp2;
    out[6] += tmp2;
    out[7] += tmp1;

    tmp0 = in[7 * stride];
    tmp1 = kIDCTMatrix[7] * tmp0;
    tmp2 = kIDCTMatrix[15] * tmp0;
    tmp3 = kIDCTMatrix[23] * tmp0;
    tmp4 = kIDCTMatrix[31] * tmp0;
    out[0] += tmp1;
    out[1] += tmp2;
    out[2] += tmp3;
    out[3] += tmp4;
    out[4] -= tmp4;
    out[5] -= tmp3;
    out[6] -= tmp2;
    out[7] -= tmp1;
}

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

bool Huffmandecode(SOS_Head &para, int comp_num, int *imgdata, int offset)
{
    int s, r, t, m, tmp; //s和t分别是读入的Huffman编码位数高四位的值和解析出来的Huffman值
    extern Huffman_tree Huffman_table[8];
    unsigned char start = para.Ss;
    unsigned char end = para.Se;
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
                for (tmp = start; tmp < 64; tmp++)
                {
                    imgdata[offset + unzigzag_table[tmp]] = 0; //前面填充r个0
                }
                break;
            }
            cout << "Huffman_AC value is:" << s << " ";
            r = (s >> 4);
            for (tmp = 0; tmp < r; tmp++)
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
    return false;
}

bool Idctdecode(unsigned char *out, int *imgdata, int offset)
{
    int colidcts[64];
    const int kColScale = 11;
    const int kColRound = 1 << (kColScale - 1);
    for (int i = 0; i < 8; ++i)
    {
        int colbuf[8] = {0};
        ComputeDCT(&imgdata[offset + i], 8, colbuf);
        for (int j = 0; j < 8; ++j)
        {
            colidcts[8 * j + i] = (colbuf[j] + kColRound) >> kColScale;
        }
    }
    const int kRowScale = 18;
    const int kRowRound = 257 << (kRowScale - 1);
    for (int i = 0; i < 8; ++i)
    {
        const int rowidx = 8 * i;
        int rowbuf[8] = {0};
        ComputeDCT(&colidcts[rowidx], 1, rowbuf);
        for (int x = 0; x < 8; ++x)
        {
            out[offset + rowidx + x] = max(0, min(255, (rowbuf[x] + kRowRound) >> kRowScale));
        }
    }
}

bool Idqtdecode(unsigned char dqt_num, int *imgdata, int offset)
{
    if (dqt_num > 3)
    {
        cout << "dqt_num error" << endl;
        return false;
    }
    for (int i = 0; i < 64; i++)
    {
        //cout << "DQ:" << DQ_TABLE[dqt_num].data[i] + 0;
        imgdata[offset + i] = imgdata[offset + i] * DQ_TABLE[dqt_num].data[i];
    }
}

//用于测试程序正确性
bool Idctdecode2(unsigned char *out, int *imgdata, int offset)
{
    double tmp[64];
    double coff[64];
    double block[64];
    double det[64];

    for (int i = 0; i < 64; i++)
    {
        block[i] = (double)(*(imgdata + offset + i));
        det[i] = 0;
    }

    /*
    coff[0] = 1.0 / sqrt((double)8);
    for (int m = 1; m < 8; m++)
    {
        coff[m] = sqrt((double)2) / sqrt((double)8);
    }

    for (int k = 0; k < 8; k++)
    {
        for (int n = 0; n < 8; n++)
        {
            for (int x = 0; x < 8; x++)
            {
                tmp[k * 8 + n] += coff[x] * block[k * 8 + x] * cos((2 * n + 1) * x * PI / 2 / 8);
            }
        }
    }

    for (int m = 0; m < 8; m++)
    {
        for (int n = 0; n < 8; n++)
        {
            for (int x = 0; x < 8; x++)
            {
                *(imgdata + offset + m * 8 + n) += (int)(coff[x] * tmp[x * 8 + n] * cos((2 * m + 1) * x * PI / 2 / 8));
            }
        }
    }
    实现方法失败*/
    double cu, cv;
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            for (int u = 0; u < 8; u++)
            {
                for (int v = 0; v < 8; v++)
                {
                    if (u == 0)
                    {
                        cu = 1 / sqrt(2);
                    }
                    else
                    {
                        cu = 1;
                    }
                    if (v == 0)
                    {
                        cv = 1 / sqrt(2);
                    }
                    else
                    {
                        cv = 1;
                    }
                    det[x * 8 + y] += 0.25 * cu * cv * (block[u * 8 + v] * cos((2 * x + 1) * u * PI / 16) * cos((2 * y + 1) * v * PI / 16));
                }
            }
        }
    }
    for (int i = 0; i < 64; i++)
    {
        *(imgdata + offset + i) = ((int)det[i] + 127);
        out[offset + i] = max(0, min(255, *(imgdata + offset + i)));
    }
}
int analysis_data(SOS_Head &para)
{
    extern IMGINFO IMG; //检测有几个component
    extern unsigned char *output;
    bool analysis_end = false;
    bool is_interleaved;
    int MCU_rows = divceil(IMG.img_height, IMG.max_hor_sample * 8);                          //检测纵向行数
    int MCU_cols = divceil(IMG.img_width, IMG.max_vet_sample * 8);                           //检测横向列数
    const unsigned short scan_bitmask = para.Ah == 0 ? (0xffff << para.Al) : (1 << para.Al); //看是哪种类型的jpeg
    unsigned char sample_hor[3];                                                             //最多只能支持三个色彩通道
    unsigned char sample_vec[3];
    unsigned char sample[3];
    int sample_count = 0;
    int offset = 0;
    int sam_order;
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
    int buffer_size = MCU_cols * MCU_rows * sample_count * 64;
    int IMGDATA[buffer_size];
    output = (unsigned char *)malloc(buffer_size * sizeof(unsigned char));
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
                int nblock_y = IMG.com_info[i].vet_sample;
                int nblock_x = IMG.com_info[i].hor_sample;
                for (int samv = 0; samv < nblock_y; ++samv)
                {
                    for (int samh = 0; samh < nblock_x; samh++)
                    {
                        sam_order = nblock_x * samv + samh;
                        int cal_sample = 0;
                        for (int cal_tmp = 0; cal_tmp < i; cal_tmp++) //计算在当前comp的前sam值
                        {
                            cal_sample += sample[cal_tmp];
                        }
                        offset = (mcu_y * MCU_cols + mcu_x) * sample_count * 64 + cal_sample * 64 + sam_order * 64;
                        Huffmandecode(para, i, IMGDATA, offset);
                        Idqtdecode(IMG.com_info[i].DQT_num, IMGDATA, offset);
                        Idctdecode(output, IMGDATA, offset);
                        Idctdecode2(output, IMGDATA, offset);
                    }
                }
            }
        }
    }
    for (int i = 0; i < 128; i++)
    {
        cout << IMGDATA[i] << " ";
    }
    return buffer_size;
}

int scan_data()
{
    bool scan_end = false;
    SOS_Head scan_para;
    int buffer_size;
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
                buffer_size = analysis_data(scan_para);
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
    return buffer_size;
}