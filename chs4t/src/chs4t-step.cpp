#include "chs4t.h"

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepPantographs(double t, double dt)
{
    // Управление разъединителями токоприемников
    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        bool coil = false;
        if(pantoListener[i]->lock())
        {
            bool *data = (bool*)pantoListener[i]->data();
            coil = *data;
            pantoListener[i]->unlock();
        }

        // Подъем/опускание ТП
        pantographs[i]->setState(coil);
        pantographs[i]->setUks(vctl->getKSVoltage());
        vctl->step(t,dt);
        pantographs[i]->step(t, dt);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepBrakesMech(double t, double dt)
{
        brakesMech[0]->step(t, dt);
        brakesMech[1]->step(t, dt);

        Q_r[1] = brakesMech[0]->getBrakeTorque();
        Q_r[2] = brakesMech[0]->getBrakeTorque();
        Q_r[3] = brakesMech[0]->getBrakeTorque();

        Q_r[4] = brakesMech[1]->getBrakeTorque();
        Q_r[5] = brakesMech[1]->getBrakeTorque();
        Q_r[6] = brakesMech[1]->getBrakeTorque();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepFastSwitch(double t, double dt)
{
    gv->setU_in(max(pantographs[0]->getUout(),
                pantographs[1]->getUout()));

    vctl->setVoltage(gv->getU_out() * vctl->getMultiplier());
    gv->setPressure(gvReservoir->getPressure());
    gv->step(t, dt);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepProtection(double t, double dt)
{
    overload_relays->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepTractionControl(double t, double dt)
{
    ip = 1.75;

    km21KR2->setControl(keys, control_signals);
    km21KR2->step(t, dt);

    QVector<relay_t>::iterator overload_relay = overload_relays->getRelptr()->begin();

    double I_vu = 0;

    std::array<DCMotor *, NUM_MOTORS>::iterator it;
    std::vector<double>::const_iterator omega = wheel_omega.begin();
    std::vector<double>::iterator Q = Q_a.begin();

    for (it = motor.begin(); it != &motor[TED4]; ++it)
    {
        (*it)->setU(vu[0]->getU_out());
        (*it)->setDirection(brc[0]->getDirection());
    }
    for (; it != motor.end(); ++it)
    {
        (*it)->setU(vu[1]->getU_out());
        (*it)->setDirection(brc[1]->getDirection());
    }
    Q++;
    for (it = motor.begin(); it != motor.end(); ++it, ++Q, ++omega, ++overload_relay)
    {

        (*it)->setOmega(ip * *omega);
        (*it)->setBetaStep(0);
        *Q = (*it)->getTorque() * ip;
        (*it)->step(t, dt);
        I_vu += (*it)->getIa();
        (*overload_relay).current = (*it)->getIa();
    }

    for (auto i : brc)
    {
        i->set031Pressure(reservoir903->getPressure());
        i->set071Pressure(reservoir903->getPressure());
        i->step(t, dt);
    }
    for (auto v : vu)
    {
        v->setI_out(I_vu);
        v->setU_in(transformer015->getTracVoltage());
        v->step(t, dt);
    }
    transformer015->setU1(gv->getU_out() * vctl->getMultiplier());
    transformer015->step(t,dt);
}


//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepAirSupplySubsystem(double t, double dt)
{
    mainReservoir->setAirFlow(mainFlow);


    for (size_t i = 0; i < motor_compressor.size(); ++i)
    {
        motor_compressor[i]->setU(reckdyn[i]->getU());
        reckdyn[i]->setI(motor_compressor[i]->getI());
        motor_compressor[i]->setPressure(mainReservoir->getPressure());
        motor_compressor[i]->step(t, dt);
    }

    double compressor_flow = motor_compressor[0]->getAirFlow() + motor_compressor[1]->getAirFlow();

    mainReservoir->setFlowCoeff(1e-3);
    mainReservoir->step(t, dt);

    gvSplit->setP_out1(gvReservoir->getPressure());
    gvSplit->setP_out2(reservoir903->getPressure());
    gvSplit->setQ_in(pf(1e-2 * (mainReservoir->getY(0) - gvSplit->getP_in())));
    gvSplit->step(t,dt);

    gvReservoir->setAirFlow(gvSplit->getQ_out1());
    reservoir903->setAirFlow(gvSplit->getQ_out2());
    gvReservoir->step(t,dt);
    reservoir903->step(t,dt);

    mainFlow = compressor_flow - (gvSplit->getQ_out1()+gvSplit->getQ_out2());

    QMap<QString,PneumoV *>::iterator it;
    for(it = pvs.begin(); it != pvs.end(); ++it)
    {
        (*it)->setPin(reservoir903->getPressure());
        (*it)->step();
    }

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepBrakesControl(double t, double dt)
{
    brakeCrane->setChargePressure(charging_press);
    brakeCrane->setFeedLinePressure(mainReservoir->getPressure());
    brakeCrane->setBrakePipePressure(pTM);
    brakeCrane->setControl(keys);

    if (control_signals.analogSignal[30].is_active)
    {
        int krm_position = control_signals.analogSignal[KRM_1].cur_value * 1 +
                           control_signals.analogSignal[KRM_2].cur_value * 2 +
                           control_signals.analogSignal[KRM_3].cur_value * 3 +
                           control_signals.analogSignal[KRM_4].cur_value * 4 +
                           control_signals.analogSignal[KRM_5].cur_value * 5 +
                           control_signals.analogSignal[KRM_6].cur_value * 6 +
                           control_signals.analogSignal[KRM_7].cur_value * 7;
        if (krm_position != 0)
        {
            krm_position -= 1;
            brakeCrane->setPosition(krm_position);
            //brakeCrane->setHandlePos(krm_position);
        }

    }

    p0 = brakeCrane->getBrakePipeInitPressure();
    brakeCrane->step(t, dt);    

    handleEDT->setControl(keys, control_signals);
    handleEDT->step(t, dt);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepBrakesEquipment(double t, double dt)
{
    brakesMech[0]->setAirFlow(pnSplit->getQ_out1());
    brakesMech[0]->step(t, dt);

    brakesMech[1]->setAirFlow(rd304->getBrakeCylAirFlow());
    brakesMech[1]->step(t, dt);

    rd304->setPipelinePressure(mainReservoir->getPressure());
    rd304->setWorkAirFlow(pnSplit->getQ_out2());
    rd304->setBrakeCylPressure(brakesMech[1]->getBrakeCylinderPressure());
    rd304->step(t, dt);

    pnSplit->setQ_in(zpk->getOutputFlow());
    pnSplit->setP_out1(brakesMech[0]->getBrakeCylinderPressure());
    pnSplit->setP_out2(rd304->getWorkPressure());
    pnSplit->step(t, dt);

    zpk->setInputFlow1(dako->getQtc());
    zpk->setInputFlow2(locoCrane->getBrakeCylinderFlow());
    zpk->setOutputPressure(pnSplit->getP_in());
    zpk->step(t, dt);

    locoCrane->setFeedlinePressure(mainReservoir->getPressure());
    locoCrane->setBrakeCylinderPressure(zpk->getPressure2());
    locoCrane->setControl(keys);

    b360->setPressure(zpk->getPressure2());
    b360->check();
    r364->setPressure(airSplit->getP_in());
    r364->check();

    //double pos = Loco_Crane->getModbus(control_signals.analogSignal[LOCO_CRANE].cur_value);
    //locoCrane->setHandlePosition(cut(pos, 0.0, 1.0));

    locoCrane->step(t, dt);

    pvs["o348"]->setPin(dako->getPy());
    pvs["o348"]->setPout(0.0);
    pvs["o348"]->setK1(1.0);
    pvs["b348"]->overridePV(!pvs["o348"]->getState());
    pvs["b348"]->setPin(airSplit->getP_in());
    pvs["b348"]->setPout(dako->getPy());
    pvs["o358"]->overridePV(!pvs["b358"]->getState());
    pvs["o358"]->setPin(brakeRefRes->getPressure());
    pvs["o358"]->setPout(0.0);
    pvs["o358"]->setK1(0.5);

    dako->setPgr(mainReservoir->getPressure());
    dako->setPkvt(zpk->getPressure2());
    dako->setPtc(zpk->getPressure1());
    dako->setQvr(airSplit->getQ_out1() - pvs["o348"]->getQ());
    dako->setVelocity(velocity);
    dako->step(t, dt);

    pvs["b358"]->setPin(airSplit->getP_in());
    pvs["b358"]->setPout(brakeRefRes->getPressure());

    airSplit->setP_out1(pvs["b348"]->getPout());
    airSplit->setP_out2(pvs["b358"]->getPout());
    airSplit->setQ_in(electroAirDistr->getQbc_out());
    airSplit->step(t, dt);

    brakeRefRes->setAirFlow(airSplit->getQ_out2() - pvs["o358"]->getQ());
    brakeRefRes->step(t, dt);

    electroAirDistr->setPbc_in(airSplit->getP_in());
    electroAirDistr->setSupplyReservoirPressure(spareReservoir->getPressure());
    electroAirDistr->setInputSupplyReservoirFlow(airDistr->getAirSupplyFlow());
    electroAirDistr->setQbc_in(airDistr->getBrakeCylinderAirFlow());
    double control_signal = 0.0;
    control_signal -= hs_p(pvs["eptO"]->getQ());
    control_signal += hs_p(pvs["eptT"]->getQ()) * 2.0;
    electroAirDistr->setControlLine(cut(control_signal, -1.0, 1.0));
    electroAirDistr->step(t, dt);

    spareReservoir->setAirFlow(electroAirDistr->getOutputSupplyReservoirFlow());
    spareReservoir->step(t, dt);

    airDistr->setAirSupplyPressure(electroAirDistr->getSupplyReservoirPressure());
    airDistr->setBrakeCylinderPressure(electroAirDistr->getPbc_out());
    airDistr->setBrakepipePressure(pTM);
    airDistr->step(t, dt);

    autoTrainStop->setFeedlinePressure(mainReservoir->getPressure());
    autoTrainStop->setBrakepipePressure(pTM);
    autoTrainStop->setControl(keys);
    autoTrainStop->step(t, dt);

    r377->setPressure(pTM);
    r377->check();

    auxRate = autoTrainStop->getEmergencyBrakeRate() + airDistr->getAuxRate();

    //mainFlow -= pf(brakeCrane->getBrakePipeFlow()+brakeCrane->getEqReservoirFlow());
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepSupportEquipment(double t, double dt)
{
    double R = 0.6;

    battery->setChargeVoltage(gv->getU_out() * vctl->getMultiplier() * 0.00884);
    battery->step(t,dt);

    blinds->setPressure(reservoir903->getPressure());
    blinds->step(t, dt);

    for (auto fan : motor_fan)
    {
        fan->step(t, dt);
    }

    motor_fan[0]->setU(reckdyn[M_FAN0]->getU());
    motor_fan[1]->setU(reckdyn[M_FAN1]->getU());

    motor_fan[2]->setU(reckdyn[OIL_FAN0]->getU());
    motor_fan[3]->setU(reckdyn[OIL_FAN1]->getU());

    motor_fan[4]->setU(reckdyn[OIL_FAN0]->getU());
    motor_fan[5]->setU(reckdyn[OIL_FAN1]->getU());


    reckdyn[OIL_FAN0]->setI(motor_fan[2]->getI() + motor_fan[4]->getI());
    reckdyn[OIL_FAN1]->setI(motor_fan[3]->getI() + motor_fan[5]->getI());


    //energy_counter->setFullPower(Uks * (motor->getI12() + motor->getI34() + motor->getI56()) );
    //energy_counter->setResistorsPower( puskRez->getR() * ( pow(motor->getI12(), 2) + pow(motor->getI34(), 2) + pow(motor->getI56(), 2) ) );
    energy_counter->step(t, dt);

    for(std::array<Reckdyn *, NUM_RECKDYNS>::iterator it = reckdyn.begin(); it != reckdyn.end(); ++it)
    {
        (*it)->step(t, dt);
        (*it)->setUin(gv->getU_out());
    }

    ;
    for(QVector<RelaySM *>::iterator it = relays.begin(); it != relays.end(); ++it)
    {
        (*it)->step(t,dt);
    }


    for(QVector<AZV *>::iterator it = azvs.begin(); it != azvs.end(); ++it)
    {
        (*it)->step(t,dt);
    }


    for(QVector<CHS4TSwitcher *>::iterator it = switches.begin(); it != switches.end(); ++it)
    {
        (*it)->setControl(keys);
        (*it)->step(t,dt);
    }


    for(QVector<Listener *>::iterator itlp = lamps.begin(); itlp != lamps.end(); ++itlp)
    {
        (*itlp)->step(t,dt);
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::stepTapSound()
{
    double speed = abs(this->velocity) * 3.6;

    for (int i = 0; i < tap_sounds.count(); ++i)
    {
        emit volumeCurveStep(tap_sounds[i], static_cast<float>(speed));
    }
}
