#include "pneumo-v.h"

PneumoV::PneumoV(QObject *parent)
  : QObject(parent)
  , overrided(false)
  , state(false)
  , old_state(false)
  , pv()
  , K1(0.0)
  , Pout(0.0)
  , Pin(0.0)
  , Q(0.0)
{

}
PneumoV::PneumoV(QString id, QObject *parent) : QObject(parent)
  , overrided(false)
  , state(false)
  , old_state(false)
  , pv(id)
  , K1(5e-3)
  , Pout(0.0)
  , Pin(0.0)
  , Q(0.0)
{
    pv.attach();
}

double PneumoV::getPout()
{
    if(state)
    {
        return Pout;
    }
    return Pin;
}

void PneumoV::step()
{
    if(!overrided && pv.lock())
    {
        state = *((bool*)pv.data());
        pv.unlock();
        if(state != old_state)
            emit stateChanged(state);
    }
}
void PneumoV::overridePV(const bool &state)
{
    overrided = true;
    this->state = state;
}
void PneumoV::enablePV()
{
    overrided = false;
}
