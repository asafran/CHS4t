#ifndef     OVERLOADRELAY_H
#define     OVERLOADRELAY_H

#include    "device.h"
#include    "QSharedMemory"
#include    "multiswitch.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

struct relay_t
{
    QSharedMemory *sw;

    double current;
    double trig_current;
    bool override;

    relay_t()
        : current(0)
        , trig_current(1)
        , override(false)
    {
        sw = new QSharedMemory("0");
    }
};


class OverloadRelays : public Device
{
public:

    OverloadRelays(QObject *parent = Q_NULLPTR);

    void override(const QString key);

    QVector<relay_t>* getRelptr() { return &ted_relays; }

    QVector<relay_t> ted_relays;

    QMap<QString, relay_t>& getRelays() { return relays; };

    ~OverloadRelays();

    double* operator[] (const QString key);

private:

    QMap<QString, relay_t> relays;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);



};
/*
enum
{
    R_701,
    R_008,
    R_009,
    R_145,
    R_146,
    R_148,
    R_860,
    R_861,
    R_851,
    RC_0251,
    RC_0261,
    RC_0271,
    RC_0272,
    RC_0262,
    RC_0252,
    RC_0651,
    RC_0661,
    RC_0671,
    RC_0652,
    RC_0662,
    RC_0672,

};
*/
#endif // OVERLOADRELAY_H
