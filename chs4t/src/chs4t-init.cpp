#include    "chs4t.h"

#include    <QDir>
#include    <QSystemSemaphore>



void CHS4T::initCircuit()
{
    process = new QProcess();
    process->setProcessChannelMode(QProcess::SeparateChannels);
    process->setReadChannel(QProcess::StandardError);

    QString program = "F:\\VieverBuilds\\release\\SimulIDE_0.3.12.13\\bin";
    process->setWorkingDirectory(program);
    QSystemSemaphore sem("sem1", 1, QSystemSemaphore::Create);
    sem.acquire();

    process->start("F:\\VieverBuilds\\release\\SimulIDE_0.3.12.13\\bin\\simulide.exe", QStringList() << "-p=C:/Users/asafr/Documents/CHS4t/CHS4T.simu" << "-s=sem1" );

    process->waitForStarted();

    sem.acquire();

    process->closeReadChannel(QProcess::StandardOutput);
}


//------------------------------------------------------------------------------
// Инициализация токоприемников
//------------------------------------------------------------------------------
void CHS4T::initPantographs()
{
    CfgReader cfg;
    cfg.load(config_dir + QDir::separator() + "pantograph" + ".xml");
    QString secName = "SM", test = config_dir;



    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        pantographs[i] = new Pantograph();
        pantographs[i]->read_custom_config(config_dir + QDir::separator() + "pantograph");
        pantographs[i]->setUks(Uks);
    }
    QString field = "pv395";
    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i, field = "pv398")
    {
        QString id = "0";
        cfg.getString(secName, field, id);
        pantoListener[i] = new QSharedMemory(id);
        pantoListener[i]->attach();
    }
    vctl = new VoltageController();
    vctl->read_custom_config(config_dir + QDir::separator() + "voltage-controller");
    vctl->setDefault();
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initBrakesMech()
{


    brakesMech[0] = new CHS4tBrakeMech();
    brakesMech[0]->read_custom_config(config_dir + QDir::separator() + "brake-mech-front");

    brakesMech[1] = new CHS4tBrakeMech();
    brakesMech[1]->read_custom_config(config_dir + QDir::separator() + "brake-mech-back");
}

//------------------------------------------------------------------------------
// Инициализация быстродействующего выключателя
//------------------------------------------------------------------------------
void CHS4T::initFastSwitch()
{


    gv = new ProtectiveDeviceSM();
    gv->read_custom_config(config_dir + QDir::separator() + "gv");

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initProtection()
{


    overload_relays = new OverloadRelays();
    overload_relays->read_custom_config(config_dir + QDir::separator() + "overload-relays");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initBrakesControl(QString module_path)
{


    brakeCrane = loadBrakeCrane(module_path + QDir::separator() + "krm395");
    brakeCrane->read_config("krm395");

    locoCrane = loadLocoCrane(module_path + QDir::separator() + "kvt254");
    locoCrane->read_config("kvt254");

    handleEDT = new HandleEDT();
    handleEDT->read_custom_config(config_dir + QDir::separator() + "handle-edt");
    handleEDT->setBrakeKey(KEY_Period);
    handleEDT->setReleaseKey(KEY_Comma);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initAirSupplySubsystem()
{

    gvReservoir = new Reservoir(0.057);
    reservoir903 = new Reservoir(0.12);
    mainReservoir = new Reservoir(1);
    spareReservoir = new Reservoir(0.078);
    brakeRefRes = new Reservoir(0.004);

    pvs["b348"] = new PneumoV(this);
    pvs["o358"] = new PneumoV(this);

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void CHS4T::initRelays()
{
    CfgReader cfg;
    cfg.load(config_dir + QDir::separator() + "relays" + ".xml");
    QString id, ba;
    cfg.getString("Pneumo", "r377", id);
    r377 = new PressureRegulator(false, id, 0.35, 0.40);
    cfg.getString("Pneumo", "b360", id);
    b360 = new PressureRegulator(true, id, 0.07, 0.08);
    cfg.getString("Pneumo", "r364", id);
    r364 = new PressureRegulator(false, id, 0.07, 0.08);
    cfg.getString("EPT362", "ID", id);
    cfg.getString("EPT362", "BA", ba);
    ept_sw = MultiSwitch(id,ba);
    QDomNode secNode = cfg.getFirstSection("Relay");
    while (!secNode.isNull())
    {
        QString s, ids, ba, name;
        //int key, signal;
        double vu, vd;
        cfg.getString(secNode, "S", s);
        cfg.getString(secNode, "BA", ba);
        cfg.getString(secNode, "ID", ids);
        //cfg.getInt(secNode, "KEY_CODE", key);
        cfg.getString(secNode, "NAME", name);
        //cfg.getInt(secNode, "SIG", signal);
        cfg.getDouble(secNode, "Vu", vu);
        cfg.getDouble(secNode, "Vd", vd);

        RelaySM *relay = new RelaySM(s, vu, vd, ids, ba, this);
        relay->setObjectName(name);
        //connect(relay, &Relay::changed, this, &CHS4T::setSignal);
        relays.append(relay);

        secNode = cfg.getNextSection();
    }


}


void CHS4T::initPVs()
{
    CfgReader cfg;
    cfg.load(config_dir + QDir::separator() + "pneumo-v" + ".xml");
    QDomNode secNode = cfg.getFirstSection("PV");
    while (!secNode.isNull())
    {
        QString id, name;

        cfg.getString(secNode, "ID", id);
        cfg.getString(secNode, "NAME", name);

        PneumoV *pv = new PneumoV(id,this);
        pv->setObjectName(name);
        pvs.insert(name,pv);

        secNode = cfg.getNextSection();
    }


}

void CHS4T::initSwitches()
{
    CfgReader cfg;
    cfg.load(config_dir + QDir::separator() + "switches" + ".xml");
    QDomNode secNode = cfg.getFirstSection("Switch");
    while (!secNode.isNull())
    {
        QString ids, ba, name, soundname;
        int key, signal, spring, spring_state, initial = 0;

        cfg.getString(secNode, "BA", ba);
        cfg.getString(secNode, "ID", ids);
        cfg.getInt(secNode, "KEY_CODE", key);
        cfg.getString(secNode, "NAME", name);
        cfg.getInt(secNode, "SIG", signal);


        CHS4TSwitcher *switcher = new CHS4TSwitcher(ids, ba, key, signal, this);
        switcher->setObjectName(name);
        if (cfg.getInt(secNode, "SPR", spring) && cfg.getInt(secNode, "SPRSTATE", spring_state))
            switcher->setSpring(spring, spring_state);
        if (cfg.getInt(secNode, "INITIAL", initial))
                switcher->setState(initial);
        if (cfg.getString(secNode, "SOUND", soundname))
        {
            switcher->setSoundName(soundname);
            connect(switcher, &Switcher::soundPlay, this, &CHS4T::soundPlay);
        }
        connect(switcher, &CHS4TSwitcher::changed, this, &CHS4T::setSignal);
        switches.append(switcher);
        secNode = cfg.getNextSection();
    }
}

void CHS4T::initLamps()
{
    CfgReader cfg;
    cfg.load(config_dir + QDir::separator() + "lamps" + ".xml");
    QDomNode secNode = cfg.getFirstSection("Lamp");
    while (!secNode.isNull())
    {
        QString id, name;
        int signal;
        double min, max;
        bool inverse;


        cfg.getString(secNode, "ID", id);
        cfg.getString(secNode, "NAME", name);

        cfg.getInt(secNode, "SIG", signal);
        Listener *listener = new Listener(id, signal, this);
        listener->setObjectName(name);
        if(cfg.getDouble(secNode, "MIN", min) && cfg.getDouble(secNode, "MAX", max))
            listener->setCut(static_cast<float>(min), static_cast<float>(max));
        if(cfg.getBool(secNode, "INVERSE", inverse))
            listener->setInverse(inverse);

        connect(listener, &Listener::changed, this, &CHS4T::setSignal);

        lamps.append(listener);
        secNode = cfg.getNextSection();
    }

}

void CHS4T::initAZV()
{
    CfgReader cfg;
    cfg.load(config_dir + QDir::separator() + "azv" + ".xml");
    QDomNode secNode = cfg.getFirstSection("AZV");
    while (!secNode.isNull())
    {
        QString id_amp, id_sw, name;
        int inom;
        double n, r, q;

        cfg.getString(secNode, "ID", id_sw);
        cfg.getString(secNode, "IDAMP", id_amp);
        cfg.getInt(secNode, "Inom", inom);
        cfg.getDouble(secNode, "N", n);
        cfg.getDouble(secNode, "R", r);
        cfg.getDouble(secNode, "Q", q);
        cfg.getString(secNode, "NAME", name);
        //cfg.getInt(secNode, "SIG", signal);

        AZV *azv = new AZV(id_amp, id_sw, inom, n, r, q, this);
        azv->setObjectName(name);
        //connect(switcher, &CHS4TSwitcher::changed, this, &CHS4T::setSignal);
        azv->toggle();
        azvs.append(azv);
        secNode = cfg.getNextSection();
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initTractionControl()
{

    km21KR2 = new Km21KR2();
    km21KR2->read_custom_config(config_dir + QDir::separator() + "km21");

    transformer015 = new TracTransformer();

    std::array<DCMotor *, NUM_MOTORS>::iterator dcmotor;
    for (dcmotor = motor.begin(); dcmotor != motor.end(); ++dcmotor)
        {
            *dcmotor = new DCMotor();
            (*dcmotor)->setCustomConfigDir(config_dir);
            (*dcmotor)->read_custom_config(config_dir + QDir::separator() + "AL4442nP");

            //connect(motor[i], &DCMotor::soundSetPitch, this, &VL60k::soundSetPitch);
            //connect(motor[i], &DCMotor::soundSetVolume, this, &VL60k::soundSetVolume);

        }

    connect(this->findChild<RelaySM *>("LK1"), &RelaySM::stateChanged, motor[TED1], &DCMotor::stateLK);
    connect(this->findChild<RelaySM *>("LK1"), &RelaySM::stateChanged, motor[TED2], &DCMotor::stateLK);
    connect(this->findChild<RelaySM *>("LK1"), &RelaySM::stateChanged, motor[TED3], &DCMotor::stateLK);

    connect(this->findChild<RelaySM *>("LK2"), &RelaySM::stateChanged, motor[TED4], &DCMotor::stateLK);
    connect(this->findChild<RelaySM *>("LK2"), &RelaySM::stateChanged, motor[TED5], &DCMotor::stateLK);
    connect(this->findChild<RelaySM *>("LK2"), &RelaySM::stateChanged, motor[TED6], &DCMotor::stateLK);

    connect(this->findChild<RelaySM *>("TED1"), &RelaySM::stateChanged, motor[TED1], &DCMotor::stateDisabler);
    connect(this->findChild<RelaySM *>("TED2"), &RelaySM::stateChanged, motor[TED2], &DCMotor::stateDisabler);
    connect(this->findChild<RelaySM *>("TED3"), &RelaySM::stateChanged, motor[TED3], &DCMotor::stateDisabler);

    connect(this->findChild<RelaySM *>("TED4"), &RelaySM::stateChanged, motor[TED4], &DCMotor::stateDisabler);
    connect(this->findChild<RelaySM *>("TED5"), &RelaySM::stateChanged, motor[TED5], &DCMotor::stateDisabler);
    connect(this->findChild<RelaySM *>("TED6"), &RelaySM::stateChanged, motor[TED6], &DCMotor::stateDisabler);

    this->findChild<RelaySM *>("TED1")->override(true);
    this->findChild<RelaySM *>("TED2")->override(true);
    this->findChild<RelaySM *>("TED3")->override(true);

    this->findChild<RelaySM *>("TED4")->override(true);
    this->findChild<RelaySM *>("TED5")->override(true);
    this->findChild<RelaySM *>("TED6")->override(true);

    transformer015->read_custom_config(config_dir + QDir::separator() + "trac-transformer");

    for (size_t i = 0; i < 2; ++i)
        {
            vu[i] = new Rectifier();
            vu[i]->read_custom_config(config_dir + QDir::separator() + "VU");
            brc[i] = new BrakeReversorController();
        }
    brc[0]->read_custom_config(config_dir + QDir::separator() + "brakereversorI");
    brc[1]->read_custom_config(config_dir + QDir::separator() + "brakereversorII");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------


void CHS4T::initBrakesEquipment(QString module_path)
{

    dako = new Dako();
    dako->read_custom_config(config_dir + QDir::separator() + "dako");

    electroAirDistr = loadElectroAirDistributor(module_path + QDir::separator() + "evr305");
    electroAirDistr->read_config("evr305");

    airDistr = loadAirDistributor(module_path + QDir::separator() + "vr242");
    airDistr->read_config("vr242");

    autoTrainStop = loadAutoTrainStop(module_path + QDir::separator() + "epk150");
    autoTrainStop->read_config("epk150");

    zpk = new SwitchingValve();
    zpk->read_config("zpk");

    rd304 = new PneumoReley();
    rd304->read_config("rd304");

    pnSplit = new PneumoSplitter();
    pnSplit->read_config("pneumo-splitter");

    airSplit = new PneumoSplitter();
    airSplit->read_custom_config(config_dir + QDir::separator() + "321-dako-splitter");

    gvSplit = new PneumoSplitter();
    gvSplit->read_config("pneumo-splitter");
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initEDT()
{
    std::array<Generator *, NUM_MOTORS>::iterator gen;
    for (gen = generator.begin(); gen != generator.end(); ++gen)
    {
        *gen = new Generator();
        (*gen)->setCustomConfigDir(config_dir);
        (*gen)->read_custom_config(config_dir + QDir::separator() + "AL4442nP");
    }



    BrakeReg = new BrakeRegulator();
    BrakeReg->read_custom_config(config_dir + QDir::separator() + "brake-regulator");
    connect(this->findChild<RelaySM *>("r037"), &RelaySM::stateChanged, BrakeReg, &BrakeRegulator::state037);

    brakeRekdyn = new Reckdyn();
    brakeRekdyn->read_custom_config(config_dir + QDir::separator() + "reckdyn-brake");


}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initOtherEquipment()
{

    //horn = new CHS4tHorn();


    speed_meter = new SL2M();
    speed_meter->read_custom_config(config_dir + QDir::separator() + "3SL-2M");

}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initSupportEquipment()
{

    motor_fan_r = new DCMotorFan();
    motor_fan_r->read_custom_config(config_dir + QDir::separator() + "motor-fan-r");

    std::array<DCMotorFan *, 6>::iterator it;
    for (it = motor_fan.begin(); it != motor_fan.end(); ++it)
        *it = new DCMotorFan();

    motor_fan[0]->read_custom_config(config_dir + QDir::separator() + "motor-fan");
    motor_fan[1]->read_custom_config(config_dir + QDir::separator() + "motor-fan");
    motor_fan[0]->stateContact(true);
    motor_fan[1]->stateContact(true);

    motor_fan[2]->read_custom_config(config_dir + QDir::separator() + "oil-fan");
    motor_fan[3]->read_custom_config(config_dir + QDir::separator() + "oil-fan");

    motor_fan[4]->read_custom_config(config_dir + QDir::separator() + "op-fan");
    motor_fan[5]->read_custom_config(config_dir + QDir::separator() + "op-fan");

    connect(motor_fan[0], &DCMotorFan::started, this->findChild<RelaySM *>("B190"), &RelaySM::override);
    connect(motor_fan[1], &DCMotorFan::started, this->findChild<RelaySM *>("B191"), &RelaySM::override);
    connect(motor_fan[4], &DCMotorFan::started, this->findChild<RelaySM *>("B210"), &RelaySM::override);
    connect(motor_fan[5], &DCMotorFan::started, this->findChild<RelaySM *>("B211"), &RelaySM::override);

    connect(this->findChild<RelaySM *>("01527"), &RelaySM::stateChanged , motor_fan[2], &DCMotorFan::stateContact);
    connect(this->findChild<RelaySM *>("01527"), &RelaySM::stateChanged , motor_fan[3], &DCMotorFan::stateContact);
    connect(this->findChild<RelaySM *>("01530"), &RelaySM::stateChanged , motor_fan[4], &DCMotorFan::stateContact);
    connect(this->findChild<RelaySM *>("01530"), &RelaySM::stateChanged , motor_fan[5], &DCMotorFan::stateContact);

    for (size_t i = 0; i < reckdyn.size(); ++i)
    {
        reckdyn[i] = new Reckdyn();
        reckdyn[i]->read_custom_config(config_dir + QDir::separator() + "reckdyn");

    }

    CfgReader cfg;
    cfg.load(config_dir + QDir::separator() + "reckdyn" + ".xml");
    std::array<QString, 8> readed;
    std::fill(readed.begin(), readed.end(), "0");
    cfg.getString("SM", "C0", readed[COMPRESSOR0]);
    cfg.getString("SM", "C1", readed[COMPRESSOR1]);
    cfg.getString("SM", "MF0", readed[M_FAN0]);
    cfg.getString("SM", "MF1", readed[M_FAN1]);
    cfg.getString("SM", "OIL0", readed[OIL_FAN0]);
    cfg.getString("SM", "OIL1", readed[OIL_FAN1]);

    QString id_brk = "0";
    cfg.getString("SM", "c8", id_brk);
    brakeRekdyn->setID(id_brk);

    for(int i = 0 ; i != NUM_RECKDYNS; ++i)
    {
        reckdyn[i]->setID(readed[i]);
    }
    //reckdyn[COMPRESSOR0]->toggleWinter();
    //reckdyn[COMPRESSOR1]->toggleWinter();

    std::array<QString, 2> c_readed;
    cfg.getString("SM", "P0", readed[0]);
    cfg.getString("SM", "P1", readed[1]);
    for (size_t i = 0; i < motor_compressor.size(); ++i)
    {
        motor_compressor[i] = new DCMotorCompressor(new PressureRegulator(true,readed[i]));
        motor_compressor[i]->read_custom_config(config_dir + QDir::separator() + "motor-compressor");
    }

    battery = new Battery();
    battery->read_custom_config(config_dir + QDir::separator() + "battery");

    blinds = new Blinds();
    blinds->read_custom_config(config_dir + QDir::separator() + "blinds");

    energy_counter = new EnergyCounter();
    energy_counter->read_custom_config(config_dir + QDir::separator() + "energy-counter");
}

//------------------------------------------------------------------------------
// modbus от чс2т
/*------------------------------------------------------------------------------
void CHS4T::initModbus()
{

    QString modbusCfgDir = config_dir + QDir::separator() + "modbus";

    TM_manometer = new PhysToModbus();
    TM_manometer->load((modbusCfgDir + QDir::separator() + "manometer-TM").toStdString());

    UR_manometer = new PhysToModbus();
    UR_manometer->load((modbusCfgDir + QDir::separator() + "manometer-UR").toStdString());

    ZT_manometer = new PhysToModbus();
    ZT_manometer->load((modbusCfgDir + QDir::separator() + "manometer-ZT").toStdString());

    GR_manometer = new PhysToModbus();
    GR_manometer->load((modbusCfgDir + QDir::separator() + "manometer-GR").toStdString());

    TC_manometer = new PhysToModbus();
    TC_manometer->load((modbusCfgDir + QDir::separator() + "manometer-TC").toStdString());

    PtM_U_bat = new PhysToModbus();
    PtM_U_bat->load((modbusCfgDir + QDir::separator() + "U-bat").toStdString());

    EPT_U = new PhysToModbus();
    EPT_U->load((modbusCfgDir + QDir::separator() + "EPT-U").toStdString());

    Network_U = new PhysToModbus();
    Network_U->load((modbusCfgDir + QDir::separator() + "Network-U").toStdString());

    Amper_12 = new PhysToModbus();
    Amper_12->load((modbusCfgDir + QDir::separator() + "Amper-12").toStdString());

    Amper_34 = new PhysToModbus();
    Amper_34->load((modbusCfgDir + QDir::separator() + "Amper-34").toStdString());

    Amper_56 = new PhysToModbus();
    Amper_56->load((modbusCfgDir + QDir::separator() + "Amper-56").toStdString());

    Pos_Indicator = new PhysToModbus();
    Pos_Indicator->load((modbusCfgDir + QDir::separator() + "Pos-Indicator").toStdString());

    Loco_Crane = new PhysToModbus();
    Loco_Crane->load((modbusCfgDir + QDir::separator() + "kvt254").toStdString());

    KPD3_Velocity = new PhysToModbus();
    KPD3_Velocity->load((modbusCfgDir + QDir::separator() + "kpd3").toStdString());
}

*///------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initSounds()
{
    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        connect(pantographs[i], &Pantograph::soundPlay, this, &CHS4T::soundPlay);
    }

/*
    for (size_t i = 0; i < NUM_PANTOGRAPHS; ++i)
    {
        pantoSwitcher[i]->setSoundName("tumbler");
        connect(pantoSwitcher[i], &Switcher::soundPlay, this, &CHS4T::soundPlay);
    }

    fastSwitchSw->setSoundName("tumbler");
    connect(fastSwitchSw, &Switcher::soundPlay, this, &CHS4T::soundPlay);
*/

    connect(brakeCrane, &BrakeCrane::soundPlay, this, &CHS4T::soundPlay);
    connect(brakeCrane, &BrakeCrane::soundSetVolume, this, &CHS4T::soundSetVolume);


    connect(locoCrane, &LocoCrane::soundPlay, this, &CHS4T::soundPlay);
    connect(locoCrane, &LocoCrane::soundSetVolume, this, &CHS4T::soundSetVolume);


    for (size_t i = 0; i < motor_compressor.size(); ++i)
    {
        motor_compressor[i]->setSoundName(QString("kompressor%1").arg(i+1));
        connect(motor_compressor[i], &DCMotorCompressor::soundPlay, this, &CHS4T::soundPlay);
        connect(motor_compressor[i], &DCMotorCompressor::soundStop, this, &CHS4T::soundStop);

        //mk_switcher[i]->setSoundName("tumbler");
        //connect(mk_switcher[i], &Switcher::soundPlay, this, &CHS4T::soundPlay);
    }


    connect(km21KR2, &Km21KR2::soundPlay, this, &CHS4T::soundPlay);


    connect(motor[0], &DCMotor::soundSetVolume, this, &CHS4T::soundSetVolume);
    connect(motor[0], &DCMotor::soundSetPitch, this, &CHS4T::soundSetPitch);


    connect(autoTrainStop, &AutoTrainStop::soundSetVolume, this, &CHS4T::soundSetVolume);


    connect(generator[0], &Generator::soundSetPitch, this, &CHS4T::soundSetPitch);
    connect(generator[0], &Generator::soundSetVolume, this, &CHS4T::soundSetVolume);


    //connect(horn, &CHS4tHorn::soundPlay, this, &CHS4T::soundPlay);
    //connect(horn, &CHS4tHorn::soundStop, this, &CHS4T::soundStop);


    connect(speed_meter, &SL2M::soundSetVolume, this, &CHS4T::soundSetVolume);


    motor_fan_r->setSoundName("PTR_fan");
    connect(motor_fan_r, &DCMotorFan::soundPlay, this, &CHS4T::soundPlay);
    connect(motor_fan_r, &DCMotorFan::soundStop, this, &CHS4T::soundStop);


    for (size_t i = 0; i < motor_fan.size(); ++i)
    {
        motor_fan[i]->setSoundName(QString("Motor_Fan%1").arg(i+1));
        connect(motor_fan[i], &DCMotorFan::soundPlay, this, &CHS4T::soundPlay);
        connect(motor_fan[i], &DCMotorFan::soundStop, this, &CHS4T::soundStop);
    }


    connect(motor_fan_r, &DCMotorFan::soundSetPitch, this, &CHS4T::soundSetPitch);

}



//------------------------------------------------------------------------------
// Инициализация регистратора
//------------------------------------------------------------------------------
void CHS4T::initRegistrator()
{

    reg = nullptr;
    reg = new Registrator("../charts/energy", 1.0, Q_NULLPTR);
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
void CHS4T::initBrakeDevices(double p0, double pTM, double pFL)
{

    charging_press = p0;

    gvReservoir->setY(0,0);
    reservoir903->setY(0,0);
    mainReservoir->setY(0, pFL);
    spareReservoir->setY(0, charging_press);
    brakeCrane->init(pTM, pFL);
    locoCrane->init(pTM, pFL);    
    airDistr->init(pTM, pFL);
    autoTrainStop->init(pTM, pFL);
}

//------------------------------------------------------------------------------
// Инициализация списка звуков перестука
//------------------------------------------------------------------------------
void CHS4T::initTapSounds()
{
    QString f_p = "tap_";

    tap_sounds << (f_p + "5-10");
    tap_sounds << (f_p + "10-20");
    tap_sounds << (f_p + "20-30");
    tap_sounds << (f_p + "30-40");
    tap_sounds << (f_p + "40-50");
    tap_sounds << (f_p + "50-60");
    tap_sounds << (f_p + "60-70");
    tap_sounds << (f_p + "70-80");
    tap_sounds << (f_p + "80-90");
    tap_sounds << (f_p + "90-100");
    tap_sounds << (f_p + "100-110");
    tap_sounds << (f_p + "110-~");
}
