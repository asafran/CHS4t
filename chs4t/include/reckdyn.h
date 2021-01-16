#ifndef RECKDYN_H
#define RECKDYN_H

#include    "device.h"
#include    <QSharedMemory>

class Reckdyn : public Device
{
    Q_OBJECT
public:
    Reckdyn(QObject *parent = Q_NULLPTR);

    void setUin(double U);
    void setI(double I) {Iout = I;}
    void setID(QString id);
    void setAngle(double override_angle) { this->setY(0, cut(override_angle, 0.0, 1.0)); enabled  = false; }

    void toggleWinter();

    double getAngle() { return this->getY(0); }
    double getU() { return this->getY(0) * min(Unom,Uin);}

private:

    QSharedMemory contact;

    bool winter_mode;

    bool enabled;

    bool in;

    double Uin;

    double t_coeff;

    double Unom;

    double Uwinter;

    double Uout;

    double Iout;

    double Imax;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);
};

#endif // RECKDYN_H
