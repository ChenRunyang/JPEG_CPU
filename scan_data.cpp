#ifndef __JPG__
#define __JPG__
#include "my_jpeg_head.h"
#endif

void init_para(unsigned char *global_ptr)
{
    
}
void scan_data()
{
    extern unsigned char *global_ptr;
    bool scan_end = false;
    while (!scan_end)
    {
        if (*(global_ptr) == MA)
        {
            switch (*(global_ptr + 1))
            {
            case SOS:
                global_ptr += 2;
                //cout << "find SOS" << endl;
                init_para(global_ptr);
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