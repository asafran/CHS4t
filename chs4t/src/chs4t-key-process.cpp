#include "chs4t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::keyProcess()
{


    if (getKeyState(KEY_V))
    {
        if (isShift())
            eptSwitch.set();
        else
            eptSwitch.reset();
    }

}
