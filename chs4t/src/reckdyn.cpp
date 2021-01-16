#include "reckdyn.h"

Reckdyn::Reckdyn(QObject *parent) : Device(parent)
  , winter_mode(false)
  , enabled(true)
  , in(false)
  , Uin(0)
  , t_coeff(0)
  , Unom(0)
  , Uout(0)
  , Iout(0)
  , Imax(0)
{

}
void Reckdyn::setUin(double U)
{
    Uin = U * t_coeff;
}
void Reckdyn::toggleWinter()
{
    double U = Unom;
    Unom = Uwinter;
    Uwinter = U;
    winter_mode = !winter_mode;
}

void Reckdyn::setID(QString id)
{
    contact.setKey(id);
    contact.attach();
}
void Reckdyn::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)

    if(contact.lock())
    {
        bool *data = (bool*)contact.data();
        in = *data;
        contact.unlock();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Reckdyn::ode_system(const state_vector_t &Y,
                            state_vector_t &dYdt,
                            double t)
{
    Q_UNUSED(t)

    double s_coil = static_cast<double>(in);

    double s_on_target = s_coil - hs_p(Y[0] - 1.0);
    double s_off_target = s_coil - hs_p(Y[0]);

    dYdt[0] = ((s_on_target * 10 * static_cast<double>( Iout < Imax - std::numeric_limits<double>::epsilon()))+(s_off_target * 100)) * static_cast<double>(enabled);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void Reckdyn::load_config(CfgReader &cfg)
{
    QString secName = "Device";
    int order = 1;
    if (cfg.getInt(secName, "Order", order))
    {
        y.resize(static_cast<size_t>(order));
        std::fill(y.begin(), y.end(), 0);

    }

    cfg.getDouble(secName, "Unom", Unom);
    cfg.getDouble(secName, "Imax", Imax);
    cfg.getDouble(secName, "Uwinter", Uwinter);
    cfg.getDouble(secName, "Coeff", t_coeff);

}
