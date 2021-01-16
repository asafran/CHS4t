#include "brake-regulator.h"

//------------------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------------------
BrakeRegulator::BrakeRegulator(QObject* parent) : Device(parent)
  , Ia(0.0)
  , If(0.0)
  , Bref(0.0)
  , u(0.0)
  , k_1(3125)
  , k_2(5e-3)
  , k_474(1.0)
  , allowEDT(false)
  , T(4.0)
  , is_active(false)
{

}

//------------------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------------------
BrakeRegulator::~BrakeRegulator()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BrakeRegulator::ode_system(const state_vector_t& Y,
                                state_vector_t& dYdt,
                                double t)
{
    Q_UNUSED(Y)
    Q_UNUSED(t)

    double I_ref = Bref * k_1 * k_474;

    dYdt[0] = (I_ref - abs(Ia)) * k_2;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BrakeRegulator::preStep(state_vector_t& Y, double t)
{
    Q_UNUSED(t)

    if (!is_active)
    {
        Y[0] = 0.0;
        Y[1] = 0.0;
        return;
    }

    Y[0] = cut(Y[0], 0.0, 1.0);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BrakeRegulator::load_config(CfgReader &cfg)
{
    QString secName = "Device";

    cfg.getDouble(secName, "K1", k_1);
    cfg.getDouble(secName, "K2", k_2);

    K474[0] = 1.0;
    cfg.getDouble(secName, "K4741", K474[1]);
    cfg.getDouble(secName, "K4742", K474[2]);
    cfg.getDouble(secName, "K4743", K474[3]);
}

void BrakeRegulator::state037(const bool &state)
{
    is_active = state;
}
