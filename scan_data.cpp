#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

void scan_data()
{
    extern unsigned char *global_ptr;
    bool scan_end = false;
    int seg_length;
    while (!scan_end)
    {
        if (*(global_ptr) == MA)
        {
            switch (*(global_ptr + 1))
            {
            case SOS:
                global_ptr += 2;
                //cout << "find SOS" << endl;
                seg_length = (*(global_ptr) << 8) + *(global_ptr + 1);
                //cout << seg_length << endl;
                global_ptr += 2;

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