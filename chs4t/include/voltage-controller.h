#ifndef VOLTAGECONTROLLER_H
#define VOLTAGECONTROLLER_H

#include "device.h"
#include <QSharedMemory>

class VoltageController : public Device
{
    Q_OBJECT
public:
    VoltageController(QObject *parent = Q_NULLPTR);

    ~VoltageController();
    double getVoltage() { return Utransf; }
    double getKSVoltage() { return Uks; }
    double getMultiplier() { return transf_mult; }
    void setKSVoltage(double U) { Uks = U; }
    void setVoltage(double U) { Utransf = U; }
    void setDefault() { Uks = Unom; transf_mult = 1.0; }
    void setLow() { transf_mult = Ulowc; }
    double getI() { return Iloc; }
    void setI(double I) { Iloc = I; }

private:

    double Uks;
    double transf_mult;
    double Utransf;
    double Iloc;

    double Unom;
    double Ulowc;

    double Uenable;
    double Udisable;


    QSharedMemory sw851;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);

};

#endif // VOLTAGECONTROLLER_H
