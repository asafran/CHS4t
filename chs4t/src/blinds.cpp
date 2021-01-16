#include    "blinds.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Blinds::Blinds(QObject *parent) : Device(parent)
  , motion_time(1.0)
  , state(false)
  , is_opened(false)
  , max_pos(1.0)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Blinds::~Blinds()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Blinds::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)

    double opened = hs_p(Y[0] - 0.95 * max_pos);
    double opened_out = hs_p(Y[1] - 0.95 * max_pos);

    is_opened = static_cast<bool>(opened);
    is_out_opened = static_cast<bool>(opened_out);

    b3541.setPos(static_cast<int>(opened));
    b3542.setPos(static_cast<int>(opened_out));

    pv453->setPout(opened);
    pv454->setPout(opened_out);
    pv453->step();
    pv454->step();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Blinds::ode_system(const state_vector_t &Y,
                        state_vector_t &dYdt,
                        double t)
{
    Q_UNUSED(t)

    dYdt[0] = 3.0 * (max_pos * static_cast<double>(pv453->getState()) - Y[0]) / motion_time;

    dYdt[1] = 3.0 * (max_pos * static_cast<double>(pv454->getState()) - Y[1]) / motion_time;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Blinds::load_config(CfgReader &cfg)
{
    QString secName = "Device", ids, ba = "1100,0011";

    cfg.getDouble(secName, "MotionTime", motion_time);
    cfg.getDouble(secName, "MaxPos", max_pos);
    cfg.getDouble(secName, "Pressure", w_pressure);
    secName = "SM";
    cfg.getString(secName, "b3541", ids);
    b3541 = MultiSwitch(ids, ba);
    ba = "00,11";
    cfg.getString(secName, "b3542", ids);
    b3542 = MultiSwitch(ids, ba);
    cfg.getString(secName, "pv453", ids);
    pv453 = new PneumoV(ids,this);
    cfg.getString(secName, "pv454", ids);
    pv454 = new PneumoV(ids,this);
}
