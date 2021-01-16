#ifndef BRAKEREVERSORCONTROLLER_H
#define BRAKEREVERSORCONTROLLER_H

#include "device.h"
#include "multiswitch.h"
#include "pneumo-v.h"

class BrakeReversorController : public Device
{
    Q_OBJECT
public:
    BrakeReversorController(QObject *parent = Q_NULLPTR);

    ~BrakeReversorController();

    void set071Pressure(double p) { pvs[T71]->setPin(hs_p(p-w_pressure)); pvs[H71]->setPin(hs_p(p-w_pressure)); }
    void set031Pressure(double p) { pvs[VP31]->setPin(hs_p(p-w_pressure)); pvs[NZ31]->setPin(hs_p(p-w_pressure)); }

    void setLKstat(bool lk);

    int getDirection() { return state_031; }
    int getBrake() { return state_071; }

    double getQ();

private:

    enum
    {
        T71 = 0,
        H71 = 1,
        VP31 = 2,
        NZ31 = 3,
        NUM_PVS = 4
    };

    std::array<PneumoV *,NUM_PVS> pvs;

    int state_071;
    int state_031;

    int old_state_071;
    int old_state_031;

    double Vu;
    double Vd;

    double w_pressure;

    MultiSwitch reversor_sw;
    MultiSwitch c071_sw;


    void preStep(state_vector_t &Y, double t);

    void postStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);

signals:

    void state071Changed(const bool &state);

    void stateReversorChanged(const bool &state);

public slots:

    void overridePV(QString &key);

    void overrideReversor();
    void override071();

};



#endif // BRAKEREVERSORCONTROLLER_H
