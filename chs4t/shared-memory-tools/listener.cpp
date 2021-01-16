#include "listener.h"

Listener::Listener(const QString id, const int sig, QObject* parent) : Device(parent)
  , led(id)
  , old_state(false)
  , inverse(false)
  , signal(sig)
  , cut_min(0.0)
  , cut_max(1.0)
{
    led.attach();
}

Listener::~Listener() {}


void Listener::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)
    bool state = false;
    if(led.lock())
    {
        bool *data = (bool*)led.data();
        state = *data;
        led.unlock();
        if(state != old_state)
        {
            old_state = state;
            if(inverse)
                state = !state;
            emit changed(cut(static_cast<float>(state), cut_min, cut_max), signal);

        }
    }

}

void Listener::ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)
    Q_UNUSED(dYdt)
}

bool Listener::setID(QString id)
{
    led.setKey(id);
    return led.attach();
}

