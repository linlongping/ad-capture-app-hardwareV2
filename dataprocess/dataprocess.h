#ifndef DATAPROCESS_H
#define DATAPROCESS_H

#include <vector>
#include <deque>
#include <mutex>
#include <thread>
#include <QMutex>

/**
 * @file dataprocess.h
 * @brief ���ݴ�������ඨ��
 * @details �������ݵ�Ԫ��DataUnit�����ݴ�����DataProcess�Ķ��壬
 *          ����ʵ������ת�������ڼ�⼰��Чֵ��������ݴ�����
 */

/**
 * @class DataUnit
 * @brief ���ݵ�Ԫ��
 * @details ��װ����ɼ����ݣ�����ԭʼ���ݡ�ת�������ݼ���������Ϣ��
 *          �ṩ����ת�����������ڱ�׼�����ݴ���
 */
class DataUnit
{
public:
    /**
     * @brief ���캯��
     * @details ��ʼ�����ݵ�Ԫ������ԭʼ���ݲ�����ת������ȡ16λ��������
     *          �Ե�9λ����������ȡ������
     * @param[in] in_data �����16λ�����������飨����Ϊ18��
     */
    DataUnit(int16_t in_data[]){
        memcpy(data,in_data,18*2);
        multiply_data(data,m_data);
        for(int i=0;i<16;i++){
            m_bit[i] = (in_data[17]>>i)&0x01;
        }
        m_bit[9] = m_bit[9] ==1?0:1;
    };

    /**
     * @brief ����ת������
     * @details ��16λ��������ת��Ϊ˫���ȸ��������ݣ�����ת��������ʵ�ֶ���
     * @param[in] in_data �����16λ������������
     * @param[out] out_data �����˫���ȸ�������������
     */
    void multiply_data(int16_t in_data[],double out_data[]);

    double m_data[18];      ///< ת��֮���˫���ȸ��������ݣ�����Ϊ18��
    int16_t data[18];       ///< ԭʼ16λ�������ݣ�����Ϊ18��
    uint8_t m_bit[16];      ///< 16λ�����������е�9λ����ȡ������
};

/**
 * @class DataProcess
 * @brief ���ݴ�����
 * @details ά�����ݻ��������ṩ������Ӻ����ڼ�⹦�ܣ����ڷ������ݵ���������
 *          �ͼ�����Чֵ��֧�ֶ��̰߳�ȫ����
 */
class DataProcess
{
public:
    /**
     * @brief ���캯��
     * @details ��ʼ�����ݴ���ʵ���������յ����ݻ�����
     */
    DataProcess();

    /**
     * @brief ������ݵ�������
     * @details ���µ����ݵ�Ԫ���뻺�������̰߳�ȫ����
     * @param[in] data ����ӵ�DataUnit��������
     */
    void add_data(DataUnit data);

    std::deque<DataUnit> m_data_buf;  ///< ���ݻ����������ڴ洢��ʷ���ݵ�Ԫ
    QMutex m_mutex;                   ///< ����������֤���̻߳����µ����ݲ�����ȫ

    /**
     * @brief �������ڵ���ֹ��
     * @details ����ָ��ͨ�����ݣ�Ѱ��������㣨��һ��ֵ��0�ҵ�ǰֵ>0�����յ㣬
     *          �������ڳ��Ⱥ���Чֵ
     * @param[in] channel ͨ������
     * @param[out] start_index �����������
     * @param[out] end_index �����յ�����
     * @param[out] period ����õ������ڳ���
     * @param[out] effect_value ����õ�����Чֵ
     */
    void find_period_start_end(int channel,int *start_index,int *end_index,
                               double &period,double &effect_value);
};

#endif // DATAPROCESS_H
