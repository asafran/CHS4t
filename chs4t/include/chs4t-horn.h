#ifndef     CHS4T_HORN_H
#define     CHS4T_HORN_H

#include    "train-horn.h"
#include    "hardware-signals.h"

class CHS4tHorn : public TrainHorn
{
public:

    CHS4tHorn(QObject *parent = Q_NULLPTR);

    ~CHS4tHorn();

private:

    bool lock;

    void preStep(state_vector_t &Y, double t);
};

#endif // CHS4T_HORN_H
