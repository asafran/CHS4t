#ifndef AZV_H
#define AZV_H

#include <QSharedMemory>
#include "device.h"

class AZV : public Device
{
    Q_OBJECT
public:
    AZV(QString id_ampmtr, QString id_contact, int I_nom, double N, double R, double Q, QObject *parent = Q_NULLPTR);
    AZV(QObject *parent = Q_NULLPTR);

    ~AZV();

private:

    QSharedMemory ampmtr;
    QSharedMemory contact;

    int I_nom;

    double I;

    double N;

    double R;

    double Q;

    bool sw;

    bool blink;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);



signals:

    void swChanged(const bool &state);
    void overload(const bool &state);

public slots:

    void toggle();

};

#endif // AZV_H
