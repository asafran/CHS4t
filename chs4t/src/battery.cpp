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
  , Unom(62.5)
  , Emax(96.0)
  , Emin(84.0)
  , C(450.0)
  , U_gen(0.0)
  , time(2.0)
  , mode(false)
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
    return pf(getY(0)+getY(1) - r * (In + Idop));
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
double Battery::getStabilizerCurrent() const
{
    return hs_p(U_gen) * (hs_p(Ib) + In + Idop);
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
    this->U_in = cut(U_gen, 0.0, Unom);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Battery::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)

    Y[2] *= hs_p(U_in);
    Y[0] = cut(Y[0], 0.0, Emax);
    Y[1] = cut(Y[1], 0.0, 15.0);
    Y[2] = cut(Y[2], 0.0, 100.0);
    U_gen = U_in  * hs_p(Y[2] - time);

    double y = Y[0] + Y[1];

    if(amperimetrS.lock())
    {
        double *data = (double*)amperimetrS.data();
        In = *data;
        amperimetrS.unlock();
    }
    if (In + Idop> 110.0 + std::numeric_limits<double>::epsilon())
    {
        if(sw.lock())
        {
            bool *data = (bool*)sw.data();
            *data = false;
            sw.unlock();
        }

    }

    if (U_gen <= y - std::numeric_limits<double>::epsilon())
    {
        Ib = -(In + Idop);
    }
    else
    {
        Ib = cut((U_gen - y), 0.0, 1.0) * 40.0;
    }

    if(voltS.lock())
    {
        double *data = (double*)voltS.data();
        *data = max(getVoltage(), U_gen);
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

    dYdt[0] = Ib * (Emax - Emin) / (C+(C2*static_cast<double>(mode))) / 3600;
    double U_up = U_gen - 48.1;
    dYdt[1] = cut((U_up - Y[1]), -1.0, 1.0);
    dYdt[2] = hs_p(U_in - 2.0);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Battery::load_config(CfgReader &cfg)
{
    QString secName = "Device", id;

    cfg.getDouble(secName, "Emax", Emax);

    setY(0, 48.0);

    cfg.getDouble(secName, "Emin", Emin);
    cfg.getDouble(secName, "r", r);
    cfg.getDouble(secName, "Rd", Rd);
    cfg.getDouble(secName, "Capacity", C);
    cfg.getDouble(secName, "Capacity2", C2);
    cfg.getDouble(secName, "Unom", Unom);
    cfg.getDouble(secName, "Time", time);

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
