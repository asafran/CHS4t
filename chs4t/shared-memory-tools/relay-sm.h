#ifndef RELAYSM_H
#define RELAYSM_H

#include <QObject>
#include "device.h"
#include "multiswitch.h"

class RelaySM : public Device, protected MultiSwitch
{
    Q_OBJECT
public:
    RelaySM(const QString &s, const double &vu, const double &vd, const QString &ids, const QString &readed_ba, QObject *parent = Q_NULLPTR);
    RelaySM(QObject *parent = Q_NULLPTR);

    bool getState() { return state; }

    ~RelaySM();


protected:

    QSharedMemory S;

    bool coil;
    bool overrided;

    bool state;
    bool old_state;

    double Vu;
    double Vd;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

signals:

    void stateChanged(const bool &state);

public slots:

    void enable();
    void override(const bool &pos);

};

#endif // RELAYSM_H
