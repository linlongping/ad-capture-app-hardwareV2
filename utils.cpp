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

    // fftw���ڴ���䷽ʽ��mallco���ƣ���ʹ��SIMD����ָ�����������ʱ��fftw_alloc�Ὣ�����Ը���Ч�ķ�ʽ����
    real = fftw_alloc_real(n);
    complex = fftw_alloc_complex(n/2+1);    // ʵ��ֻ���õ�(n/2)+1��complex����

    // Step1��FFTʵ��ʱ��Ƶ���ת��
    plan = fftw_plan_dft_r2c_1d(n, real, complex, FFTW_ESTIMATE);
    for (i = 0; i < n; i++){
        real[i] = in[i];
    }

    // �Գ���Ϊn��ʵ������FFT������ĳ���Ϊ(n/2)-1�ĸ���
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
    //��������200��������Ƶ����200hz �ź�Ƶ����40
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
