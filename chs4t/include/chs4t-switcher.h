#ifndef     CHS4T_SWITCHER_H
#define     CHS4T_SWITCHER_H

#include    "device.h"
#include    "switcher.h"
#include    "multiswitch.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class CHS4TSwitcher : public Switcher, public MultiSwitch
{
    Q_OBJECT
public:

    CHS4TSwitcher(const QString &ids, const QString &readed_ba, int key_code = 0, int signal = 0, QObject *parent = Q_NULLPTR);

    ~CHS4TSwitcher();

    void setSoundName(QString soundName) { this->soundName = soundName; }

    void setSpring(int state, int spring_state) { springStates.insert(state, spring_state); }

private:

    int     old_state;

    const int     signal;

    QString soundName;


    QMap<int, int> springStates;

    void preStep(state_vector_t &Y, double t);

    //void load_config(CfgReader &cfg);
signals:
    void changed(const float &state, const int &signal);

};

#endif // CHS4T_SWITCHER_H
