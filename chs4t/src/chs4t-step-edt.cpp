#include "chs4t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepEDT(double t, double dt)
{

    brakeRekdyn->setUin(gv->getU_out()*vctl->getMultiplier());
    brakeRekdyn->setAngle(BrakeReg->getAngle());

    for (size_t i = 0; i < generator.size(); ++i)
    {
        generator[i]->setUf(brakeRekdyn->getU()/6);
        generator[i]->setOmega(wheel_omega[i] * ip);
        generator[i]->setRt(3.35);
        generator[i]->step(t, dt);
    }

    BrakeReg->setIa(generator[0]->getIa());
    BrakeReg->setIf(generator[0]->getIf());
    BrakeReg->setBref(brakeRefRes->getPressure());    

    brakeRekdyn->step(t, dt);
    BrakeReg->step(t, dt);

}
