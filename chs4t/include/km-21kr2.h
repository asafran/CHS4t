#ifndef KM21KR2_H
#define KM21KR2_H

#include "device.h"
#include "multiswitch.h"
#include "km-21kr2-state.h"
#include  "hardware-signals.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class Km21KR2 : public Device
{
public:
    /// Конструктор
    Km21KR2(QObject *parent = Q_NULLPTR);

    ///Деструктор
    ~Km21KR2();

    void setHod(bool hod) { this->hod = hod; }

    ControllerState getCtrlState() { return controlState; }

    double getMainShaftPos() const
    {
        if (fieldWeakShaft == 0)
            return mainShaftPos * 0.1;
        else
            return fieldWeakShaft * 0.1;
    }

    double getHandleHeight() const { return getY(0); }

    double getRevShaftPos() const { return static_cast<double>(reverseState); }

private:

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);

    void preStep(state_vector_t &Y, double t);

    void stepKeysControl(double t, double dt);

    void stepExternalControl(double t, double dt);

    void connectSignals(ControllerSignals cs, bool &k);

    void addSignalsInControllerState();

    bool k01;
    bool k02;

    bool k21;
    bool k22;
    bool k23;
    bool k25;

    bool k31;
    bool k32;
    bool k33;


    bool autoSet;
    bool autoReset;
    bool lastControllerPositionIsZero;
    bool reverseIsPressedOneTime;
    bool hod;

    int reverseState;
    int prev_reverseState;

    int mainShaftPos;
    int fieldWeakShaft;

    double mainShaftHeight;

    bool is_inc;
    bool is_dec;

    ControllerState controlState;

    enum
    {
        D_AUTO = 0,
        D_ONE = 1,
        N = 2,
        U_ONE = 3,
        U_AUTO = 4,
    };
    enum
    {
        NO_OP = 0,
        OP1 = 1,
        OP2 = 2,
        OP3 = 3,
        OP4 = 4,
    };

    MultiSwitch kr21_op;

    MultiSwitch kr21_sw;

    MultiSwitch reverser_sw;

    QSharedMemory latch;
};

#define TO_INT(variable) static_cast<int>(variable)

#endif // KM21KR2_H
