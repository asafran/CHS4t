#include "chs4t-switcher.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS4TSwitcher::CHS4TSwitcher(const QString &ids, const QString &readed_ba, const int key_code, const int signal, QObject* parent)
    : Switcher(parent, key_code, 0)
    , MultiSwitch(ids, readed_ba)
    , old_state(-1)
    , signal(signal)
    , soundName("Switcher")
{
    setKolStates(getPositions());
    setKeyCode(key_code);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS4TSwitcher::~CHS4TSwitcher()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4TSwitcher::preStep(state_vector_t &Y, double t)
{
    Q_UNUSED(Y)
    Q_UNUSED(t)

    QMap<int, int>::const_iterator springs_i = springStates.constBegin();

    while (springs_i != springStates.constEnd()) {
        if (springs_i.key() == state && !getKeyState(keyCode))
        {
            state = springs_i.value();
        }
        springs_i++;
    }

    if (old_state != state)
    {
        emit soundPlay(soundName);
        emit changed(getHandlePos(), signal);

        old_state = state;

        setPos(state);
    }
}
/*
void CHS4TSwitcher::load_config(CfgReader &cfg)
{
    QString ids, ab, secName = objectName();

    if(cfg.getString(secName, "BA", ab) && cfg.getString(secName, "ID", ids))
    {
        initialize(ids,ab);
    }

}
*/
