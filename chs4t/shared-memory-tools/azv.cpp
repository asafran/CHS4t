#include "azv.h"

AZV::AZV(QString id_ampmtr, QString id_contact, int i_nom, double n, double r, double q, QObject *parent) : Device(parent)
  , ampmtr(id_ampmtr)
  , contact(id_contact)
  , I_nom(i_nom)
  , I(0)
  , N(n)
  , R(r)
  , Q(q)
  , sw(false)
  , blink(false)

{
    y.resize(2);
    std::fill(y.begin(), y.end(), 0);
    ampmtr.attach();
    contact.attach();
}

AZV::AZV(QObject *parent) : Device(parent)
  , ampmtr()
  , contact()
  , I_nom(16)
  , I(0)
  , N(3)
  , R(0.009)
  , Q(50)
  , sw(false)
  , blink(false)

{
    y.resize(2);
    std::fill(y.begin(), y.end(), 0);
}

AZV::~AZV() {}

void AZV::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(t)

    if(ampmtr.lock())
    {
        double *data = (double*)ampmtr.data();
        I = *data;
        ampmtr.unlock();
    }
    if(contact.lock())
    {
        bool *data = (bool*)contact.data();
        *data = static_cast<bool>(hs_p(Y[0] - 1.0));
        contact.unlock();
    }
    if(hs_p(Y[1] - Q) || hs_p((I/I_nom) - 3))
    {
        blink = true;
        emit overload(blink);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void AZV::ode_system(const state_vector_t &Y,
                            state_vector_t &dYdt,
                            double t)
{
    Q_UNUSED(t)


    dYdt[1] = (hs_p(Y[1])*(pow(I,2)*R)) - N;

    double s_coil = hs_n((I/I_nom) - 3) * hs_n(Y[1] - Q) * static_cast<double>(sw) * static_cast<double>(!blink);

    double s_on_target = s_coil - hs_p(Y[0] - 1.0);
    double s_off_target = s_coil - hs_p(Y[0]);



    dYdt[0] = (s_on_target * 100)+(s_off_target * 100);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void AZV::toggle()
{
    if (blink)
    {
        blink = false;
        emit overload(blink);
    }
    sw = !sw;
    emit swChanged(sw);
}

