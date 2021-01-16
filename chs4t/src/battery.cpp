#include    "battery.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Battery::Battery(QObject *parent) : Device(parent)
  , r(1.0)
  , Rd(1.0)
  , In(0.0)
  , Idop(0.0)
  , Is(0.0)
  , Ib(0.0)
  , Emax(96.0)
  , Emin(84.0)
  , C(450.0)
  , U_gen(0.0)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
Battery::~Battery()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Battery::getVoltage() const
{
    return pf(getY(0) - r * In);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double Battery::getCargeCurrent() const
{
    return Ib;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Battery::setLoadCurrent(double I)
{
    this->Idop = I;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Battery::setStarterCurrent(double Is)
{
    this->Is = Is;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Battery::setChargeVoltage(double U_gen)
{
    this->U_gen = cut(U_gen, 0.0, 62.5);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Battery::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)

    Y[0] = cut(Y[0], 0.0, Emax);

    if(amperimetrS.lock())
    {
        double *data = (double*)amperimetrS.data();
        In = *data;
        amperimetrS.unlock();
    }
    if (In > 110.0)
    {
        if(sw.lock())
        {
            bool *data = (bool*)sw.data();
            *data = false;
            sw.unlock();
        }

    }

    if (U_gen <= Y[0])
    {
        Ib = -(In + Idop);
    }
    else
    {
        Ib = cut((U_gen - Y[0]), 0.0, 1.0) * 40.0;
    }

    if(voltS.lock())
    {
        double *data = (double*)voltS.data();
        *data = max(Y[0], U_gen);
        voltS.unlock();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Battery::ode_system(const state_vector_t &Y,
                         state_vector_t &dYdt,
                         double t)
{
    Q_UNUSED(Y)
    Q_UNUSED(t)

    dYdt[0] = (Ib - Is) * (Emax - Emin) / C / 3600;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Battery::load_config(CfgReader &cfg)
{
    QString secName = "Device", id;

    cfg.getDouble(secName, "Emax", Emax);

    setY(0, 60.0);

    cfg.getDouble(secName, "Emin", Emin);
    cfg.getDouble(secName, "r", r);
    cfg.getDouble(secName, "Rd", Rd);
    cfg.getDouble(secName, "Capacity", C);

    secName = "SM";
    cfg.getString(secName, "SW", id);
    sw.setKey(id);
    sw.attach();
    cfg.getString(secName, "Amp", id);
    amperimetrS.setKey(id);
    amperimetrS.attach();
    cfg.getString(secName, "VS", id);
    voltS.setKey(id);
    voltS.attach();

    if(sw.lock())
    {
        bool *data = (bool*)sw.data();
        *data = true;
        sw.unlock();
    }
}
