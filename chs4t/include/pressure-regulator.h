#ifndef     PRESSURE_REGULATOR_H
#define     PRESSURE_REGULATOR_H

#include    <QSharedMemory>
#include    "math-funcs.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class PressureRegulator
{
public:

    PressureRegulator(bool mode, QString id = "0", double min = 0.75, double max = 0.9);
    ~PressureRegulator();

    void setPressure(double press);

    double getState() const { return state; }

    void check();

private:

    double p_cur;
    double p_prev;

    double p_min;
    double p_max;

    double state;
    bool prev_state;
    const bool mode;

    QSharedMemory sw;


};

#endif // PRESSURE_REGULATOR_H
