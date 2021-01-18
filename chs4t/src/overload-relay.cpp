#include    "overload-relay.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
OverloadRelays::OverloadRelays(QObject *parent) : Device(parent)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
OverloadRelays::~OverloadRelays()
{

}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double* OverloadRelays::operator[] (const QString key)
{
    return &relays[key].current;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void OverloadRelays::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)
    Q_UNUSED(Y)

    QMap<QString, relay_t>::iterator it = relays.begin();

    for(; it != relays.end(); ++it)
    {
        if(it->sw->lock())
        {
            bool *data = (bool*)it->sw->data();
            *data = (it->current > (it->trig_current + std::numeric_limits<double>::epsilon())) || it->override;
            it->sw->unlock();
        }
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void OverloadRelays::ode_system(const state_vector_t &Y,
                               state_vector_t &dYdt,
                               double t)
{
    Q_UNUSED(Y)
    Q_UNUSED(dYdt)
    Q_UNUSED(t)
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void OverloadRelays::load_config(CfgReader &cfg)
{

    QDomNode secNode = cfg.getFirstSection("TED");

    //num_bt << "701" << "008" << "009" << "145" << "146" << "148" << "860" << "861" << "851" << "0271" << "0261" << "0251" << "0272" << "0262" << "0252" << "06X1" << "06X2" << "BUTT" << "GROUND_RS1" << "GROUND_RS2";

    while (secNode.nodeName() == "TED")
    {
        relay_t relay;
        QString key;
        if(!cfg.getString(secNode, "ID", key) || !cfg.getDouble(secNode, "TRIGG", relay.trig_current))
            continue;

        relay.sw->setKey(key);
        relay.sw->attach();

        ted_relays.append(relay);

        secNode = cfg.getNextSection();
    }

    secNode = cfg.getFirstSection("Relays");
    while (!secNode.isNull())
    {
        relay_t relay;
        QString key, name;
        if(!cfg.getString(secNode, "ID", key) || !cfg.getString(secNode, "NAME", name) || !cfg.getDouble(secNode, "TRIGG", relay.trig_current))
            continue;

        relay.sw->setKey(key);
        relay.sw->attach();

        relays.insert(name, relay);

        secNode = cfg.getNextSection();
    }

}


// надо обработать 406 контактор
// 841 земля ?????
// работа от 12 кв
// обработать 342 контакты в цепи тормоза
// не забыть о доп контактах 006
// 371 времени дефект всп машин 853
// 480 и блок боксования
// отопление комрессоров и зимний режим
// фильтрация масла
/*/
 * 701 отопление
 * 008 кз в первичной
 * 009 кз на корпус
 * 145 кз во вторичной 1
 * 146 кз во вторичной 2
 * 148 перегруз рекдынов на возбуждение при торможении
 * 851 пониженное напряжение на позициях
 * 853 повышенное напряжение на рецкдынах
 *
 * 860 861 земля
 *
 * 060
 * перегрузка при торможении (общ по телегам)
 * 02х перегрузка на якоре по двигателям
/*/
