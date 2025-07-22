#ifndef UTILS_H
#define UTILS_H

class Utils
{
public:
    Utils();
};

int find_file_number();
void delete_file();
int get_new_fd();


float get_fft_freq(int n,const float *in,int sample_rate);
void fft_test();
#endif // UTILS_H
