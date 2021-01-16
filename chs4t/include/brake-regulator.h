#ifndef BRAKEREGULATOR_H
#define BRAKEREGULATOR_H

#include "device.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class BrakeRegulator : public Device
{
    Q_OBJECT
public:

    ///Конструктор
    BrakeRegulator(QObject *parent = Q_NULLPTR);

    ///Деструктор
    ~BrakeRegulator();

    void setIa(double value)     { Ia = value; }

    void setIf(double value)     { If = value; }

    void setK474(int value)     { k_474 = K474[value]; }

    void setBref(double value)   { Bref = value; }

    double getAngle() const             { return getY(0); }

    void reset() { setY(0, 0); setY(1, 0); }

    void setActive(bool is_active) { this->is_active = is_active; }

private:

    /// Текущий ток якорей ТЭД
    double Ia;

    /// Текущий ток возбуждения
    double If;

    /// Заданное тормозное усилие
    double Bref;

    /// Управляющее воздействие для ИПВ (уровень напряжения на обмотке возбуждения)
    double u;

    /// Коэффициент пропорциональности между давлением в ЗТ и заданным током якоря
    double k_1;

    /// Передаточный коэффициент интегрального регулятора
    double k_2;

    double k_474;

    std::array<double, 4> K474;

    /// Флаг разрешающий работу ЭДТ
    bool allowEDT;

    double T;

    bool is_active;

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void preStep(state_vector_t &Y, double t);

    void load_config(CfgReader &cfg);

public slots:
    void state037(const bool &state);
};

#endif // BRAKEREGULATOR_H
