#include    "chs4t.h"

#include    "chs4t-signals.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepSignals()
{

    analogSignal[STRELKA_POS] = static_cast<float>(transformer015->getY(0)) / 42.0f;

    //analogSignal[STRELKA_AMP1] = static_cast<float>(motor->getI12() / 1000.0);


    analogSignal[STRELKA_AMP3] = static_cast<float>(abs(generator[0]->getIa()) / 1800.0);
    analogSignal[STRELKA_AMP2] = static_cast<float>(abs(generator[0]->getIf()) / 1800.0);


        //analogSignal[STRELKA_AMP3] = static_cast<float>(motor->getI56() / 1000.0);
        //analogSignal[STRELKA_AMP2] = static_cast<float>(motor->getI34() / 1000.0);



    analogSignal[STRELKA_PM] = static_cast<float>(mainReservoir->getPressure() / 1.6);
    analogSignal[STRELKA_TC] = static_cast<float>(brakesMech[0]->getBrakeCylinderPressure() / 1.0);
    analogSignal[STRELKA_EDT] = static_cast<float>(brakeRefRes->getPressure() / 1.0);
    analogSignal[STRELKA_UR] = static_cast<float>(brakeCrane->getEqReservoirPressure() / 1.0);
    analogSignal[STRELKA_TM] = static_cast<float>(pTM / 1.0);

    analogSignal[STRELKA_UKS] = static_cast<float>(vctl->getVoltage()/30000.0);

    analogSignal[STRELKA_U_BAT] = static_cast<float>(battery->getVoltage() / 100.0);
    analogSignal[STRELKA_U_EPT] = static_cast<float>((battery->getCargeCurrent() + 50.0) / 100.0);

    analogSignal[KRAN395_RUK] = static_cast<float>(brakeCrane->getHandlePosition());
    analogSignal[KRAN254_RUK] = static_cast<float>(locoCrane->getHandlePosition());

    analogSignal[KONTROLLER] = static_cast<float>(km21KR2->getMainShaftPos());
    analogSignal[REVERSOR] = static_cast<float>(km21KR2->getRevShaftPos());
    analogSignal[SHTURVAL] = static_cast<float>(km21KR2->getHandleHeight());
    //analogSignal[SHTUR_SVISTOK] = static_cast<float>(horn->isSvistok());
/*
    analogSignal[SIGLIGHT_P] = static_cast<float>(stepSwitch->isParallel());
    analogSignal[SIGLIGHT_SP] = static_cast<float>(stepSwitch->isSeriesParallel());
    analogSignal[SIGLIGHT_S] = static_cast<float>(stepSwitch->isSeries());
    analogSignal[SIGLIGHT_ZERO] = static_cast<float>(stepSwitch->isZero());
*/
 //   analogSignal[SIGLIGHT_R] = static_cast<float>(EDT);

    analogSignal[SIGLIGHT_O] = ept_pass_control->stateReleaseLamp();
    analogSignal[SIGLIGHT_PEREKRISHA] = ept_pass_control->stateHoldLamp();
    analogSignal[SIGLIGHT_T] = ept_pass_control->stateBrakeLamp();

    analogSignal[SIGLIGHT_NO_BRAKES_RELEASE] = static_cast<float>(brakesMech[0]->getBrakeCylinderPressure() >= 0.1);

    analogSignal[PANT1] = static_cast<float>(pantographs[0]->getHeight());
    analogSignal[PANT2] = static_cast<float>(pantographs[1]->getHeight());


    analogSignal[SIGLIGHT_RAZED] = static_cast<float>( !pant_switch[0].getState() && !pant_switch[1].getState() );

    analogSignal[INDICATOR_BV] = static_cast<float>(gv->getLampState());


    analogSignal[SW_EPT] = static_cast<float>(eptSwitch.getState());

    //analogSignal[SW_VK] = blindsSwitcher->getHandlePos();


    analogSignal[BLINDS] = blinds->getInPosition();
    //analogSignal[SIGLIGHT_GALYZI] = static_cast<float>(blinds->isInOpened());

    analogSignal[HANDLE_RT] = handleEDT->getHandlePos();

    analogSignal[STRELKA_SPEED] = speed_meter->getArrowPos();
    analogSignal[VAL_PR_SKOR1] = speed_meter->getShaftPos();
    analogSignal[VAL_PR_SKOR2] = speed_meter->getShaftPos();

    //analogSignal[SW_EDT] = EDTSwitch.getState();

    analogSignal[WHEEL_1] = static_cast<float>(dir * wheel_rotation_angle[0] / 2.0 / Physics::PI);
    analogSignal[WHEEL_2] = static_cast<float>(dir * wheel_rotation_angle[1] / 2.0 / Physics::PI);
    analogSignal[WHEEL_3] = static_cast<float>(dir * wheel_rotation_angle[2] / 2.0 / Physics::PI);
    analogSignal[WHEEL_4] = static_cast<float>(dir * wheel_rotation_angle[3] / 2.0 / Physics::PI);
    analogSignal[WHEEL_5] = static_cast<float>(dir * wheel_rotation_angle[4] / 2.0 / Physics::PI);
    analogSignal[WHEEL_6] = static_cast<float>(dir * wheel_rotation_angle[5] / 2.0 / Physics::PI);


}


