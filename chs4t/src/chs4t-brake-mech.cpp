#include    "chs4t-brake-mech.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS4tBrakeMech::CHS4tBrakeMech(QObject *parent)
    : BrakeMech(parent)
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
CHS4tBrakeMech::~CHS4tBrakeMech()
{

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4tBrakeMech::load_config(CfgReader &cfg)
{
    BrakeMech::load_config(cfg);
}