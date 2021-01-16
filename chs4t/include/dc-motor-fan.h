#ifndef DCMOTORFAN_H
#define DCMOTORFAN_H

#include    "device.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class DCMotorFan : public Device
{
    Q_OBJECT
public:

    DCMotorFan(QObject *parent = Q_NULLPTR);

    ~DCMotorFan();

    void setU(double value);
    double getU() { return U; }
    double getI() { return I; }

    void setSoundName(QString name) { soundName = name; }

private:

    double U;
    double I;
    double cPhi;
    double R;
    double omega_nom;
    double ks;
    double J;

    QString soundName;

    double Unom;
    double kf;
    double enabled;
    double contact;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);

signals:

    void started(const bool &pos);

public slots:

    void stateContact(const bool &state);

    void stateDisabler(const bool &state);
};

#endif // DCMOTORFAN_H
