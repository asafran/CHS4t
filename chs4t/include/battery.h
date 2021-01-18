#ifndef     BATTERY_H
#define     BATTERY_H

#include    "device.h"
#include    <QSharedMemory>

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class Battery : public Device
{
public:

    Battery(QObject *parent = Q_NULLPTR);

    ~Battery();

    double getVoltage() const;

    double getCargeCurrent() const;

    double getStabilizerCurrent() const;

    void setLoadCurrent(double I);

    void setStarterCurrent(double Is);

    void setChargeVoltage(double U_gen);

private:

    /// Внутреннее сопротивление батареи
    double  r;

    /// Добавочное сопротивление в цепи заряда
    double  Rd;

    /// Ток, потребляемый нагрузкой
    double  In;

    double  Idop;

    /// Ток, потребляемый стартером дизеля
    double  Is;

    /// Ток заряда/разряда
    double  Ib;

    double Unom;

    /// Максимальная ЭДС
    double  Emax;

    /// Минимальная ЭДС
    double  Emin;

    /// Емкость, А*ч
    double  C;

    double  C2;

    /// Напряжение заряда
    double  U_gen;

    double  U_in;

    double  time;

    bool mode;

    QSharedMemory voltS;
    QSharedMemory amperimetrS;
    QSharedMemory sw;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);
};

#endif // BATTERY_H
