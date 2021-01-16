 #include    "pressure-regulator.h"


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
PressureRegulator::PressureRegulator(bool mode, QString id, double min, double max)
  : p_cur(0.0)
  , p_prev(0.0)
  , p_min(min)
  , p_max(max)
  , state(0.0)
  , mode(mode)
  , sw(id)
{
    sw.attach();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
PressureRegulator::~PressureRegulator()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PressureRegulator::setPressure(double press)
{
    p_prev = p_cur;
    p_cur = press;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void PressureRegulator::check()
{
    if(mode)
    {
        double dp = p_cur - p_prev;

        if (p_cur < p_min)
                state = 1.0;

        if (p_cur > p_max)
                state = 0.0;

        if ( (p_cur >= p_min) && (p_cur <= p_max) )
        {
            state = hs_p(dp);
        }
    } else {
        if (p_cur < p_min)
            state = 0.0;

        if (p_cur > p_max)
            state = 1.0;

    }
    if((state == 1.0) != prev_state)
    {
        prev_state = static_cast<bool>(state);
        if(sw.lock())
        {
            bool *data = (bool*)sw.data();
            *data = prev_state;
            sw.unlock();
        }
    }
}

