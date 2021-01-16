#include "relay-sm.h"

RelaySM::RelaySM(const QString &s, const double &vu, const double &vd, const QString &ids, const QString &readed_ba, QObject *parent) : Device(parent), MultiSwitch(ids, readed_ba)
  , S(s)
  , coil(false)
  , overrided(false)
  , state(false)
  , old_state(true)
  , Vu(vu)
  , Vd(vd)
{
    y.resize(1);
    std::fill(y.begin(), y.end(), 0);
    S.attach();
}
RelaySM::RelaySM(QObject *parent) : Device(parent), MultiSwitch()
  , S("0")
  , coil(false)
  , overrided(false)
  , state(false)
  , old_state(true)
  , Vu(0)
  , Vd(0)
{
    y.resize(1);
    std::fill(y.begin(), y.end(), 0);
}

RelaySM::~RelaySM()
{

}

void RelaySM::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)

    if(S.lock() && !overrided)
    {
        bool *data = (bool*)S.data();
        coil = *data;
        S.unlock();
    }

    state = static_cast<bool>(hs_p(Y[0] - 0.7));

    if (state != old_state)
    {
        setPos(state);
        emit stateChanged(state);
        old_state = state;
        if (state == 1)
            emit soundPlay("GV_On");

        if (state == 0)
            emit soundPlay("GV_Off");
    }

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void RelaySM::ode_system(const state_vector_t &Y,
                            state_vector_t &dYdt,
                            double t)
{
    Q_UNUSED(t)

    double s_coil = static_cast<double>(coil);

    double s_on_target = s_coil - hs_p(Y[0] - 1.0);
    double s_off_target = s_coil - hs_p(Y[0]);

    dYdt[0] = (s_on_target * Vu)+(s_off_target * Vd);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
/*
void RelaySM::load_config(CfgReader &cfg)
{

    int order = 1;
    if (cfg.getInt("Device", "Order", order))
    {
        y.resize(static_cast<size_t>(order));
        std::fill(y.begin(), y.end(), 0);

    }

    QString secName = objectName();
    double V;
    if (cfg.getDouble(secName, "Vu", V))
        Vu = V;

    if (cfg.getDouble(secName, "Vd", V))
        Vd = V;

    QString ids, ab;

    if(cfg.getString(secName, "BA", ab) && cfg.getString(secName, "ID", ids))
    {
        initialize(ids,ab);
    }

    QString id = "0";
    cfg.getString(secName, "S", id);
    S.setKey(id);
    S.attach();


}
void RelaySM::override()
{
    overrided = !overrided;
}
void RelaySM::override(const bool &pos)
{
    overrided = true;
    state = pos;
}
*/
void RelaySM::enable()
{
    overrided = false;
}
void RelaySM::override(const bool &pos)
{
    overrided = true;
    coil = pos;
}
