#include    "trac-transformer.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TracTransformer::TracTransformer(QObject *parent) : Device(parent)
    , U1(0.0)
    , K_sn(62.7)
    , Vps(0)
    , state_158(false)
    , state_159(false)
    , ps_pos(0.0)
    , ps_coeff(0)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
TracTransformer::~TracTransformer()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double TracTransformer::getU_sn() const
{
    return U1 / K_sn;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracTransformer::setU1(double value)
{
    U1 = value;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
double TracTransformer::getTracVoltage() const
{
    return cur_pos.Unom * U1 / 25000.0;
}

QString TracTransformer::getPosName() const
{
    return cur_pos.name;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracTransformer::setPosition(int position)
{
    ps_pos = position;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracTransformer::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(Y)
    Q_UNUSED(t)
    double fractpart;
    fractpart = modf(Y[0] , Q_NULLPTR);

    ps_pos = static_cast<int>(round(Y[0]));

    if (position.contains(ps_pos))
    {
        cur_pos = position[ps_pos];
    }

    QVector<ps_contact_t>::iterator it = contacts.begin();
    for(;it != contacts.end(); ++it)
    {
        if(it->sw->lock())
        {
            bool *data = (bool*)it->sw->data();
            *data = ((it->p0 <= ps_pos) && (ps_pos <= it->p1));
            it->sw->unlock();
        }
    }
    int lock_shaft;
    if(Y[0] != 0.0)
    {


        lock_shaft = static_cast<int>(round(Y[0] * 4)) % 8;


        //lock_shaft = static_cast<int>(Y[0] * 4) % 8;
    }
    else
        lock_shaft = 0;



    if(pv158.lock())
    {
        bool *data = (bool*)pv158.data();
        state_158 = *data;
        pv158.unlock();
    }
    if(pv159.lock())
    {
        bool *data = (bool*)pv159.data();
        state_159 = *data;
        pv159.unlock();
    }
    switch (lock_shaft) {
            case I:{
                if(state_158)
                    ps_coeff = 1.0;
                else if(state_159)
                    ps_coeff = -1.0;
                else
                    ps_coeff *= hs_p(std::abs(fractpart) - 0.009);
                break;
            }
            case II:{
                if (!state_158 && !state_159)
                    ps_coeff = -1.0;
                else if (state_159 && state_158)
                    ps_coeff = 1.0;
                else {
                    ps_coeff = 0;
                }
                break;
            }
            case III:{
                if(!state_158)
                    ps_coeff = 1.0;
                else if(!state_159)
                    ps_coeff = -1.0;
                else
                    ps_coeff *= hs_p(std::abs(fractpart) - 0.009);
                break;
            }
            case IV:{

                if (state_158 && state_159)
                    ps_coeff = -1.0;
                else if (!state_159 && !state_158)
                    ps_coeff = 1.0;
                else {
                    ps_coeff = 0;
                }
                break;
            }
            }


    ps_sw.setPos(lock_shaft);

    emit soundSetVolume("Trac_Transformer", static_cast<int>(100.0 * U1 / 25000.0));
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracTransformer::ode_system(const state_vector_t &Y,
                                 state_vector_t &dYdt,
                                 double t)
{
    Q_UNUSED(Y)
    Q_UNUSED(dYdt)
    Q_UNUSED(t)


    dYdt[0] = Vps * ps_coeff;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void TracTransformer::load_config(CfgReader &cfg)
{
    QDomNode secNode = cfg.getFirstSection("Position");

    while (secNode.nodeName() == "Position")
    {
        position_t pos;

        cfg.getInt(secNode, "Number", pos.number);
        cfg.getDouble(secNode, "Voltage", pos.Unom);
        cfg.getString(secNode, "Name", pos.name);

        position.insert(pos.number, pos);

        secNode = cfg.getNextSection();
    }

    secNode = cfg.getFirstSection("PS_Contacts");

    while (secNode.nodeName() == "PS_Contacts")
    {
        ps_contact_t contact;

        QString key;
        if(!cfg.getString(secNode, "ID", key) || !cfg.getInt(secNode, "P0", contact.p0) || !cfg.getInt(secNode, "P1", contact.p1))
            continue;
        contact.sw->setKey(key);
        contact.sw->attach();

        contacts.append(contact);

        secNode = cfg.getNextSection();
    }


    QString secName = "Device";

    int order = 1;
    if (cfg.getInt(secName, "Order", order))
    {
        y.resize(static_cast<size_t>(order));
        std::fill(y.begin(), y.end(), 0);
    }
    cfg.getDouble(secName, "Vps", Vps);

    secName = "PS";
    QString ids, ba;

    if(cfg.getString(secName, "BA", ba) && cfg.getString(secName, "ID", ids))
    {
        ps_sw = MultiSwitch(ids,ba);
    }

    QString id = "0";
    cfg.getString(secName, "PV8", id);
    pv158.setKey(id);
    pv158.attach();
    cfg.getString(secName, "PV9", id);
    pv159.setKey(id);
    pv159.attach();

    id = "0";
    secName = "Contacts";
    cfg.getString(secName, "c01516", id);
    c16.setKey(id);
    c16.attach();
    if(c16.lock())
    {
        bool *data = (bool*)c16.data();
        *data = true;
        c16.unlock();
    }

    cfg.getString(secName, "c01525", id);
    c25.setKey(id);
    c25.attach();
    cfg.getString(secName, "c01526", id);
    c26.setKey(id);
    c26.attach();

}
