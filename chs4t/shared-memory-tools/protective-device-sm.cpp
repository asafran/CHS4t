#include "protective-device-sm.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
ProtectiveDeviceSM::ProtectiveDeviceSM(QObject *parent) : ProtectiveDevice(parent)
  , gv_sw()
  , S1()
  , S2()
  , p_contact()
{

}
ProtectiveDeviceSM::~ProtectiveDeviceSM()
{

}
void ProtectiveDeviceSM::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)

    if(S1.lock())
    {
        bool *data = (bool*)S1.data();
        holding_coil = *data;
        S1.unlock();
    }
    if(S2.lock())
    {
        bool *data = (bool*)S2.data();
        is_return = *data;
        S2.unlock();
    }

    old_state = state;
    state = static_cast<int>(hs_p(Y[0] - 1.0));

    lamp_state = 1 - state;

    if (state != old_state)
    {
        gv_sw.setPos(state);
        if (state == 1)
            emit soundPlay("GV_On");

        if (state == 0)
            emit soundPlay("GV_Off");
    }
    p_contact->check();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ProtectiveDeviceSM::ode_system(const state_vector_t &Y,
                            state_vector_t &dYdt,
                            double t)
{
    Q_UNUSED(t)

    double s_state = Y[0] - 1.0;


    double s_s1 = static_cast<double>(holding_coil);
    double s_s2 = static_cast<double>(is_return);
    double s_on_target = s_s1 * s_s2 * hs_n(Y[0] - 2.0);

    double s1 = s_s1 + s_on_target - hs_p(Y[0]);

    U_out = U_in * hs_p(s_state);

    dYdt[0] = s1 * Vn;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void ProtectiveDeviceSM::load_config(CfgReader &cfg)
{

    QString secName = "Device";

    int order = 1;
    if (cfg.getInt(secName, "Order", order))
    {
        y.resize(static_cast<size_t>(order));
        std::fill(y.begin(), y.end(), 0);

    }

    secName = "SM";
    QString ids, ba;

    if(cfg.getString(secName, "BA", ba) && cfg.getString(secName, "ID", ids))
    {
        gv_sw = MultiSwitch(ids, ba);
        gv_sw.setPos(0);
    }

    QString id = "0";
    cfg.getString(secName, "S1", id);
    S1.setKey(id);
    S1.attach();
    cfg.getString(secName, "S2", id);
    S2.setKey(id);
    S2.attach();
    cfg.getString(secName, "P", id);
    p_contact = new PressureRegulator(false,id,0.65,0.70);


}
