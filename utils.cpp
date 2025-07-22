#include "utils.h"
#include "fftw3.h"
#include <qmath.h>
#include <QDebug>
Utils::Utils()
{

}
float get_fft_freq(int n,const float *in,int sample_rate)
{
    int i;
    double *real;
    fftw_complex *complex;
    fftw_plan plan;

    // fftw的内存分配方式和mallco类似，但使用SIMD（单指令多数据流）时，fftw_alloc会将数组以更高效的方式对齐
    real = fftw_alloc_real(n);
    complex = fftw_alloc_complex(n/2+1);    // 实际只会用到(n/2)+1个complex对象

    // Step1：FFT实现时域到频域的转换
    plan = fftw_plan_dft_r2c_1d(n, real, complex, FFTW_ESTIMATE);
    for (i = 0; i < n; i++){
        real[i] = in[i];
    }

    // 对长度为n的实数进行FFT，输出的长度为(n/2)-1的复数
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    double max = -1;
    int max_index = 0;
    for(int i=0;i<n/2-1;i++){
        if(max < complex[i][0]*complex[i][0]+complex[i][1]*complex[i][1]){
            max = complex[i][0]*complex[i][0]+complex[i][1]*complex[i][1];
            max_index = i;
        }
    }
    //qDebug()<< "max "<< max_index <<"max_va"<< max;

    fftw_free(real);
    fftw_free(complex);

    return max_index*1.0*sample_rate/n;
}

void fft_test()
{
    //采样点是200个，采样频率是200hz 信号频率是40
    float data[20000];
    for(int i=0;i<20000;i++){
        data[i] = 9*qSin(2*M_PI*45.744*i/200.0)+1+6*qSin(2*M_PI*20*i/200);
    }

        double freq = get_fft_freq(20*1000,data,200);
    qDebug()<< "111111111111111111111"<< freq;

//    for(int i=0;i<200;i++){
//        qDebug() << data_out[i];
//    }

}
