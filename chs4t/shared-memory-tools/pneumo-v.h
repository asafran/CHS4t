#ifndef PNEUMOV_H
#define PNEUMOV_H

#include    <QObject>
#include    <QSharedMemory>

class PneumoV : public QObject
{
    Q_OBJECT
public:
    PneumoV();
    PneumoV(QString id, QObject *parent = Q_NULLPTR);

    void setPin(double P) { Pin = P; }
    void setPout(double P) { Pout = P; }
    void setK1(double k) { K1= k; }
    double getQ() { return K1 * (Pin - Pout) * static_cast<double>(state); }
    bool getState() { return state; }
    double getPout();
    void step();
private:

    bool overrided;

    bool state;

    bool old_state;

    QSharedMemory pv;

    double K1;

    double Pout;

    double Pin;

    double Q;

signals:

    void stateChanged(const bool &state);

public slots:

    void overridePV(const bool &state);

    void enablePV();
};

#endif // PNEUMOV_H
