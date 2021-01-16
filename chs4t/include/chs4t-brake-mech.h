#ifndef CHS4TBRAKEMECH_H
#define CHS4TBRAKEMECH_H

#include    "brake-mech.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class CHS4tBrakeMech : public BrakeMech
{
public:

    CHS4tBrakeMech(QObject *parent = Q_NULLPTR);

    ~CHS4tBrakeMech();

private:

    void load_config(CfgReader &cfg);
};

#endif // CHS4TBRAKEMECH_H
