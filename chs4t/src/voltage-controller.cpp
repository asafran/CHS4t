#include "voltage-controller.h"

VoltageController::VoltageController(QObject *parent) : Device(parent)
  , Uks(0.0)
  , transf_mult(0.0)
  , Utransf(0.0)
  , Iloc(0.0)
  , Unom(0.0)
  , Uenable(0.0)
  , Udisable(0.0)
{

}

VoltageController::~VoltageController(){}

void VoltageController::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)

    if(sw851.lock())
    {
        bool *data = (bool*)sw851.data();
        *data = (!*data && ( Utransf > (Uenable - std::numeric_limits<double>::epsilon())))
                || (*data && ( Utransf > (Udisable - std::numeric_limits<double>::epsilon())));
        sw851.unlock();
    }

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void VoltageController::ode_system(const state_vector_t &Y,
                            state_vector_t &dYdt,
                            double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)
    Q_UNUSED(dYdt)
}


void VoltageController::load_config(CfgReader &cfg)
{

    QString secName = "Device";

    int order = 1;
    if (cfg.getInt(secName, "Order", order))
    {
        y.resize(static_cast<size_t>(order));
        std::fill(y.begin(), y.end(), 0);

    }

    cfg.getDouble(secName,"Unom",Unom);
    cfg.getDouble(secName,"Ulowc",Ulowc);
    cfg.getDouble(secName,"Ue",Uenable);
    cfg.getDouble(secName,"Ud",Udisable);

    secName = "SM";
    QString id;

    if(cfg.getString(secName, "ID", id))
    {
        sw851.setKey(id);
        sw851.attach();
    }

}
