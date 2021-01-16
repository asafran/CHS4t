#ifndef LISTENER_H
#define LISTENER_H

#include <QObject>
#include <QSharedMemory>
#include "device.h"

class Listener : public Device
{
    Q_OBJECT
public:
    Listener(const QString id, const int sig = 10000, QObject* parent = Q_NULLPTR);

    template <typename T>
    T getState() { return static_cast<T>(old_state); }

    bool setID(QString id);

    void setSignal(const int sig) { signal = sig; }

    void setInverse(const bool inv) { inverse = inv; }

    void setCut(const float min, const float max) { cut_min = min; cut_max = max; }

    ~Listener();

private:

    QSharedMemory led;

    bool old_state;

    bool inverse;

    int signal;

    float cut_min;
    float cut_max;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

signals:

    void changed(const float &state, const int &signal);
};

#endif // LISTENER_H
