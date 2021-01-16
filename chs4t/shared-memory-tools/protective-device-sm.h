#ifndef PROTECTIVEDEVICESM_H
#define PROTECTIVEDEVICESM_H

#include    "protective-device.h"
#include    "pressure-regulator.h"
#include    "multiswitch.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class ProtectiveDeviceSM : public ProtectiveDevice
{
public:

    ProtectiveDeviceSM(QObject *parent = Q_NULLPTR);

    ~ProtectiveDeviceSM();

    void setPressure(double press) { p_contact->setPressure(press); }

private:
    MultiSwitch gv_sw;

    QSharedMemory S1;
    QSharedMemory S2;

    PressureRegulator *p_contact;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);
};

#endif // PROTECTIVEDEVICESM_H
