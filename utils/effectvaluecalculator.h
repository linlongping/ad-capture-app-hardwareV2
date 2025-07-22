#ifndef EFFECTVALUECALCULATOR_H
#define EFFECTVALUECALCULATOR_H
#include <queue>
#include <math.h>
#include <stdint.h>
#include <mutex>
#define BUF_LEN 2000

class EffectValueCalculator
{
public:
    EffectValueCalculator();
    void add_data(int16_t data){
        m_mutex.lock();
        buf.push_back(data);
        if(buf.size()>BUF_LEN){
            buf.pop_front();
        }
        m_mutex.unlock();
    }

    double calc_effect_value(){
        if(buf.empty()) return  0.0;
    //有效值
       m_mutex.lock();
       double sum = 0;
       for(int i=0;i<buf.size();i++){
           sum +=  buf.at(i)*buf.at(i);
       }
       m_mutex.unlock();
      double effect_value = sqrt(sum/buf.size());
      return effect_value;
    }
private:
    std::deque<int16_t> buf;
    std::mutex  m_mutex;
};

#endif // EFFECTVALUECALCULATOR_H
