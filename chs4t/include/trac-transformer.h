#ifndef     TRAC_TRANSFORMER_H
#define     TRAC_TRANSFORMER_H

#include    "device.h"
#include    <QSharedMemory>
#include "multiswitch.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
struct position_t
{
    int     number;
    QString name;
    double  Unom;

    position_t()
        : number(0)
        , name("0")
        , Unom(0.0)
    {

    }
};

struct ps_contact_t
{
    QSharedMemory *sw;

    int p0;
    int p1;

    ps_contact_t()
        : p0(0)
        , p1(0)
    {
        sw = new QSharedMemory("0");
    }
};

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class TracTransformer : public Device
{
public:

    TracTransformer(QObject *parent = Q_NULLPTR);

    ~TracTransformer();

    /// Получить напряжение с обмотки собственных нужд (СН)
    double getU_sn() const;

    /// Задать напряжение первичной обмотки
    void setU1(double value);

    /// Вернуть текущее напряжение на тяговой обмотке
    double getTracVoltage() const;

    double getQ() const { return static_cast<double>(ps_coeff != 0.0) * 0.01; }

    QString getPosName() const;

    float getLockShaft() { return static_cast<float>(static_cast<int>(round(getY(0) * 4)) % 8); }

    /// Задать текущую позицию ЭКГ
    void setPosition(int position);

    enum
    {
        I = 0,
        ItoII = 1,
        II = 2,
        IItoIII = 3,
        III = 4,
        IIItoIV = 5,
        IV = 6,
        IVto1 = 7
    };

private:


    /// Напряжение на первичной обмотке
    double  U1;    

    double  Ups;

    /// Коэффициент трансформации обмотки СН
    double  K_sn;

    double Vps;

    bool state_158;
    bool state_159;

    bool fault;

    /// Текущая позиция ПС
    int     ps_pos;

    double ps_coeff;

    QSharedMemory pv158;
    QSharedMemory pv159;

    QSharedMemory c16;
    QSharedMemory c25;
    QSharedMemory c26;

    /// Описатель текущей позиции ЭКГ
    position_t cur_pos;

    MultiSwitch ps_sw;



    /// Данные о эквивалентных напряжениях тяговой обмотки на каждой позиции ЭКГ
    QMap<int, position_t>   position;
    QVector<ps_contact_t>   contacts;

    void preStep(state_vector_t &Y, double t);

    void ode_system(const state_vector_t &Y, state_vector_t &dYdt, double t);

    void load_config(CfgReader &cfg);
};

#endif // TRAC_TRANSFORMER_H
