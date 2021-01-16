#include "chs4t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepDebugMsg(double t, double dt)
{
    Q_UNUSED(dt)

    DebugMsg = QString("t: %1 x: %2 v: %3 ТМ: %4 GV: %5 Q: %6 Ibat: %7 pos: %8 Ukr: %9")
        .arg(t, 10, 'f', 1)
        .arg(railway_coord / 1000.0, 5, 'f', 1)
        .arg(velocity * Physics::kmh, 5, 'f', 1)
        .arg(BrakeReg->getAngle(), 10, 'f', 4)
        .arg(pvs["b348"]->getQ(), 10, 'f', 10)
        .arg(pvs["o348"]->getQ(), 10, 'f', 10)
        .arg(battery->getCargeCurrent(), 10, 'f', 2)
        .arg(transformer015->getY(0), 10, 'f', 2)
        .arg(pantographs[0]->getUout());
}