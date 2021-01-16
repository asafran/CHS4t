#include "chs4t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::setSwitcherState(Switcher *sw, signal_t signal)
{
    if (signal.is_active)
    {
        int pos = signal.cur_value - 1;

        emit soundPlay("tumbler");

        if (pos >= 0)
            sw->setState(pos);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void setTriggerState(Trigger &trig, signal_t signal)
{
    if (signal.is_active)
    {
        int pos = signal.cur_value - 1;

        if (pos >= 0)
        {
            if (static_cast<bool>(pos))
                trig.set();
            else
                trig.reset();
        }
    }
}

//------------------------------------------------------------------------------
//
/*------------------------------------------------------------------------------
void CHS4T::stepSwitcherPanel()
{

    // Верхнаяя половина приборной панели
    setTriggerState(eptSwitch, control_signals.analogSignal[SWP1_EPT]);
    //setSwitcherState(mk_switcher[0], control_signals.analogSignal[SWP1_MK1]);

    setSwitcherState(this->indChild<CHS4TSwitcher *>("368"), control_signals.analogSignal[SWP1_BV]);
    setSwitcherState(this->indChild<CHS4TSwitcher *>("402"), control_signals.analogSignal[SWP1_TP1]);
    setSwitcherState(this->indChild<CHS4TSwitcher *>("389"), control_signals.analogSignal[SWP1_VK]);
    setSwitcherState(this->indChild<CHS4TSwitcher *>("403"), control_signals.analogSignal[SWP1_TP2]);

    // Нижняя половина панели
    //setSwitcherState(mk_switcher[1], control_signals.analogSignal[SWP2_MK2]);
    control_signals.analogSignal[SWP2_MV].cur_value -= 1.0;
    //setSwitcherState(motor_fan_switcher, control_signals.analogSignal[SWP2_MV]);

    if (control_signals.analogSignal[EDT_OFF].is_active && control_signals.analogSignal[EDT_ON].is_active)
    {
        if (handleEDT->getEDTState())
            EDTSwitch.set();

        else
            EDTSwitch.reset();
    }

    if (control_signals.analogSignal[LOCO_RELEASE].is_active)
        locoRelease = static_cast<bool>(control_signals.analogSignal[LOCO_RELEASE].cur_value);

}
*/
