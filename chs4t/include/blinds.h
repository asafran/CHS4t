//------------------------------------------------------------------------------
//
//      Жалюзи
//
//------------------------------------------------------------------------------
#ifndef     BLINDS_H
#define     BLINDS_H

#include    "device.h"
#include    "pneumo-v.h"
#include    "multiswitch.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class Blinds : public Device
{
public:

    Blinds(QObject *parent = Q_NULLPTR);

    ~Blinds();

    /// Задать состояние жалюзи (открыто/закрыто)
    void setStateIn(bool state) { this->state = state; }

    void setStateOut(bool state) { this->state = state; }

    void setPressure(double p) { pv453->setPin(hs_p(p-w_pressure)); pv454->setPin(hs_p(p-w_pressure)); }

    /// Признак полностью открытых жалюзи
    bool isInOpened() const { return is_opened; }

    bool isOutOpened() const { return is_out_opened; }

    /// Получить текущее положение жалюзи
    float getInPosition() const { return static_cast<float>(getY(0)); }

    float getOutPosition() const { return static_cast<float>(getY(1)); }

private:

    /// Время перемещения между открытым и закрытым состояниями
    double  motion_time;

    double w_pressure;

    /// Заданное состояние жалюзи
    bool    state;

    /// Признак открытия
    bool    is_opened;

    bool    is_out_opened;

    /// Максимальная величина положения жалюзи (в относительном выражении)
    double  max_pos;

    MultiSwitch b3541;

    MultiSwitch b3542;

    PneumoV *pv453;

    PneumoV *pv454;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);
};

#endif // BLINDS_H
