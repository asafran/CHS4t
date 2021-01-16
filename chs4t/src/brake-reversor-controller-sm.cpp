#include "brake-reversor-controller-sm.h"

BrakeReversorController::BrakeReversorController(QObject *parent) : Device(parent)
  , state_071(0)
  , state_031(0)
  , old_state_071(0)
  , old_state_031(0)
  , Vu(0)
  , Vd(0)
  , w_pressure(0)
{

}
BrakeReversorController::~BrakeReversorController(){}

void BrakeReversorController::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)

    for( auto pv : pvs)
    {
        pv->step();
    }

    state_031 = 0;
    state_031 += (Y[1] <= 0);
    state_031 += (hs_p(Y[1] - 1.0) * 2);

    state_071 = 0;
    state_071 += (Y[0] <= 0);
    state_071 += (hs_p(Y[0] - 1.0) * 2);


    if (state_031 != old_state_031)
    {
        reversor_sw.setPos(state_031);
        old_state_031 = state_031;
        emit stateReversorChanged(state_031);
        emit soundPlay("GV_On");
    }
    if (state_071 != old_state_071)
    {
        c071_sw.setPos(state_071);
        old_state_071 = state_071;
        emit state071Changed(state_071);
        emit soundPlay("GV_On");
    }


}
void BrakeReversorController::postStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)
    switch (state_031)
    {
    case 1:

        pvs[NZ31]->setPout(1.0);
        pvs[VP31]->setPout(0.0);
        break;

    case 0:

        pvs[NZ31]->setPout(0.0);
        pvs[VP31]->setPout(0.0);
        break;

    case 2:

        pvs[NZ31]->setPout(0.0);
        pvs[VP31]->setPout(1.0);
        break;
    }

    switch (state_071)
    {
    case 1:

        pvs[T71]->setPout(1.0);
        pvs[H71]->setPout(0.0);
        break;

    case 0:

        pvs[T71]->setPout(0.0);
        pvs[H71]->setPout(0.0);
        break;

    case 2:

        pvs[T71]->setPout(0.0);
        pvs[H71]->setPout(1.0);
        break;
    }
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void BrakeReversorController::ode_system(const state_vector_t &Y,
                            state_vector_t &dYdt,
                            double t)
{
    Q_UNUSED(t)

    double s_pv_h = hs_p(pvs[H71]->getQ());
    double s_pv_t = -hs_p(pvs[T71]->getQ());
/*
    double s_on_target = s_pv_h * hs_p(Y[0] - 1.0);
    double s_off_target = s_pv_t * hs_p(Y[0]);
*/
    dYdt[0] = (s_pv_h * Vu)+(s_pv_t * Vd);


    double s_pv_f = hs_p(pvs[VP31]->getQ());
    double s_pv_b = -hs_p(pvs[NZ31]->getQ());
/*
    s_on_target = s_pv_f - hs_p(Y[1] - 1.0); //0
    s_off_target = s_pv_b * hs_p(Y[1]);//1
*/
    dYdt[1] = (s_pv_f * Vu)+(s_pv_b * Vd);
}



void BrakeReversorController::load_config(CfgReader &cfg)
{

    QString secName = "Device";

    int order = 1;
    if (cfg.getInt(secName, "Order", order))
    {
        y.resize(static_cast<size_t>(order));
        std::fill(y.begin(), y.end(), 0);
        setY(0, 2.0);

    }

    double vu, vd, pressure;
    if(cfg.getDouble(secName, "Vu", vu) && cfg.getDouble(secName, "Vd", vd) && cfg.getDouble(secName, "Pressure", pressure))
    {
        Vu = vu;
        Vd = vd;
        w_pressure = pressure;
    }



    secName = "Reversor";
    QString ids, ab;

    if(cfg.getString(secName, "BA", ab) && cfg.getString(secName, "ID", ids))
    {
        reversor_sw = MultiSwitch(ids,ab);
    }

    QString id = "0";
    cfg.getString(secName, "PVF", id);
    pvs[VP31] = new PneumoV(id,this);
    cfg.getString(secName, "PVB", id);
    pvs[NZ31] = new PneumoV(id,this);
    secName = "PR";
    if(cfg.getString(secName, "BA", ab) && cfg.getString(secName, "ID", ids))
    {
        c071_sw = MultiSwitch(ids,ab);
    }
    cfg.getString(secName, "PVH", id);
    pvs[H71] = new PneumoV(id,this);
    cfg.getString(secName, "PVT", id);
    pvs[T71] = new PneumoV(id,this);
}

double BrakeReversorController::getQ()
{
    double Q = 0.0;
    for( auto pv : pvs)
    {
        Q += pv->getQ();
    }
    return Q;
}

void BrakeReversorController::overridePV(QString &key)
{

}

void BrakeReversorController::overrideReversor()
{

}
void BrakeReversorController::override071()
{

}
