//------------------------------------------------------------------------------
//
//      Магистральный пассажирский электровоз постоянного тока ЧС2т.
//      Дополнение для Russian Railway Simulator (RRS)
//
//      (c) RRS development team:
//          Дмитрий Притыкин (maisvendoo),
//          Николай Авилкин (avilkin.nick)
//
//      Дата: 21/08/2019
//
//------------------------------------------------------------------------------
#ifndef     CHS4T_H
#define     CHS4T_H

#include    "QSharedMemory"

#include    "vehicle-api.h"
#include    "pantograph.h"
#include    "protective-device-sm.h"
#include    "voltage-controller.h"
#include    "brake-reversor-controller-sm.h"
#include    <QProcess>
#include    "reckdyn.h"
#include    "battery.h"
#include    "voltage-regulator.h"
#include    "km-21kr2.h"
#include    "rectifier.h"
#include    "trac-transformer.h"
#include    "dc-motor.h"
#include    "registrator.h"
#include    "overload-relay.h"
#include    "dc-motor-compressor.h"
#include    "pressure-regulator.h"
#include    "chs4t-brake-mech.h"
#include    "dako.h"
#include    "relay-sm.h"
#include    "listener.h"
#include    "azv.h"
#include    "generator.h"
#include    "brake-regulator.h"
#include    "handle-edt.h"
#include    "dc-motor-fan.h"
#include    "release-valve.h"
#include    "blinds.h"
#include    "hardware-signals.h"
#include    "ept-converter.h"
#include    "ept-pass-control.h"
#include    "convert-physics-to-modbus.h"
#include    "chs4t-horn.h"
#include    "sl2m.h"
#include    "energy-counter.h"
#include    "chs4t-switcher.h"

/*!
 * \class
 * \brief Основной класс, описывающий весь электровоз
 */
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class CHS4T : public Vehicle
{
public:

    /// Конструктор
    CHS4T();

    /// Деструктор
    ~CHS4T();

private:

    enum
    {
        NUM_PANTOGRAPHS = 2,
        WIRE_VOLTAGE = 25000,
        NUM_MOTORS = 6,
        TED1 = 0,
        TED2 = 1,
        TED3 = 2,
        TED4 = 3,
        TED5 = 4,
        TED6 = 5
    };
    enum
    {
        NUM_RECKDYNS = 6,
        COMPRESSOR0 = 0,
        COMPRESSOR1 = 1,
        M_FAN0 = 2,
        M_FAN1 = 3,
        OIL_FAN0 = 4,
        OIL_FAN1 = 5
    };
    /// Напряжение аккумуляторной батареи
    Battery *battery;

    /// Электродвигатели
    std::array<DCMotor *, 6>  motor;

    /// Токоприемники
    std::array<Pantograph *, NUM_PANTOGRAPHS>    pantographs;

    /// Быстрый выключатель
    ProtectiveDeviceSM *gv;

    /// Контролирует и задает напряжение
    VoltageController *vctl;

    /// Трасформатор
    TracTransformer *transformer015;

    /// Выпрямительные установки
    std::array<Rectifier *, 2> vu;

    /// Симулятор схем
    QProcess *process;

    /// Регистратор
    Registrator *reg;

    /// Контроллер машиниста
    Km21KR2 *km21KR2;

    /// Переключатель ход-тормоз и реверсор
    std::array<BrakeReversorController *, 2> brc;

    double tracForce_kN;

    /// Список звуков перестука
    QList<QString> tap_sounds;

    /// Блок 850
    OverloadRelays *overload_relays;

    /// Главный резервуар (ГР)
    Reservoir *mainReservoir;

    /// Резервуар ГВ
    Reservoir *gvReservoir;

    /// Резервуар 903
    Reservoir *reservoir903;

    /// Запасный резервуар (ЗР)
    Reservoir *spareReservoir;

    /// Задатчик тормозного усилия ЭДТ (ЗТ)
    Reservoir *brakeRefRes;


/*
    PhysToModbus *TM_manometer;
    PhysToModbus *UR_manometer;
    PhysToModbus *ZT_manometer;
    PhysToModbus *GR_manometer;
    PhysToModbus *TC_manometer;

    PhysToModbus *PtM_U_bat;
    PhysToModbus *EPT_U;
    PhysToModbus *Network_U;

    PhysToModbus *Amper_12;
    PhysToModbus *Amper_34;
    PhysToModbus *Amper_56;

    PhysToModbus *Pos_Indicator;

    PhysToModbus *Loco_Crane;

    PhysToModbus *KPD3_Velocity;


    Осталось от ЧС2т
*/


    /// Мотор-компрессоры (МК)
    std::array<DCMotorCompressor *, 2> motor_compressor;

    /// Поездной кран машиниста (КрМ)
    BrakeCrane *brakeCrane;

    ElectroAirDistributor *electroAirDistr;

    /// Тормозная рычажная передача тележек
    std::array<CHS4tBrakeMech *, 2>    brakesMech;

    /// Скоростной клапан ДАКО
    Dako *dako;

    /// Воздухораспределитель (ВР)
    AirDistributor *airDistr;

    /// Кран вспомогательного тормоза (КВТ)
    LocoCrane *locoCrane;

    /// Переключательный клапан (ЗПК)
    SwitchingValve *zpk;

    /// Реле давления РД304 (РД)
    PneumoReley *rd304;

    /// Разветвитель потока воздуха от ДАКО к тележке 1 и РД304
    PneumoSplitter *pnSplit;

    /// Разветвитель от ВР к ДАКО и ЗТ
    PneumoSplitter *airSplit;

    /// Разветвитель от напорной к резервуарам ГВ и 903
    PneumoSplitter *gvSplit;;

    /// Реле 377 экстренное торможение
    PressureRegulator *r377;

    /// Блокировка 360 размыкается при применении вспомогательного
    PressureRegulator *b360;

    /// Блокировка 364 замыкается при 0.8 в трубопроводе к ДАКО
    PressureRegulator *r364;

    /// Вентилятор охлаждения тормозных резисторов
    DCMotorFan *motor_fan_r;

    /// Свисток и тифон
    TrainHorn   *horn;

    /// Регулятор тормозного усилия (САРТ)
    BrakeRegulator  *BrakeReg;

    AutoTrainStop *autoTrainStop;

    /// Рукоятка задатчика тормозного усилия
    HandleEDT       *handleEDT;

    /// Вентили управления токоприемниками
    std::array<QSharedMemory *, NUM_PANTOGRAPHS> pantoListener;

    /// МВ
    std::array<DCMotorFan*, 6> motor_fan;

    /// Блоки управления ВУ вспомогательных машин
    std::array<Reckdyn *, NUM_RECKDYNS> reckdyn;

    /// Контейнеры указателей на динамически добавляемое оборудование
    QVector<RelaySM *> relays;
    QVector<CHS4TSwitcher *> switches;
    QVector<Listener *> lamps;
    QVector<AZV *> azvs;
    QMap<QString, PneumoV *> pvs;

    /// Затычка для работы ЭПТ
    MultiSwitch ept_sw;

    /// Зарядное давление
    double charging_press;

    /// ТЭД в реостатном торможении
    std::array<Generator *, NUM_MOTORS> generator;

    /// Разъединители токоприемников
    std::array<Trigger, NUM_PANTOGRAPHS> pant_switch;

    /// Тригеры поднятия/опускания ТП
    std::array<Trigger, NUM_PANTOGRAPHS> pantup_trigger;

    /// Выключатель ЭПТ
    Trigger     eptSwitch;

    /// Преобразователь питания ЭПТ
    EPTConverter *ept_converter;

    /// Блок управления ЭПТ
    EPTPassControl *ept_pass_control;

    /// RECKDYN возбуждения тормоза
    Reckdyn *brakeRekdyn;

    /// поток воздуха из ГР
    double mainFlow;

    /// Передаточное число тягового редуктора
    double ip;

    /// Ток дополнительной нагрузки на АКБ
    double In;

    /// Жалюзи
    Blinds *blinds;

    /// Скоростемер 3СЛ2М
    SL2M *speed_meter;

    /// Счетчик энергии
    EnergyCounter *energy_counter;




    //------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------




    //------------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------------

    void initCircuit();

    /// Инициадизация тормозных приборов
    void initBrakeDevices(double p0, double pTM, double pFL);

    /// Загрузка данных из конфигурационных файлов
    void loadConfig(QString cfg_path);

    /// Обработка клавиш
    void keyProcess();

    /// Вывод данных на внешние СОИ
    void hardwareOutput();

    /// Сброс данных в регистратор
    void registrate(double t, double dt);

    /// Состояние удерживающей катошки БВ
    bool getHoldingCoilState() const;

    /// Инициализация токоприемников
    void initPantographs();

    /// Инициадизация рычажки
    void initBrakesMech();

    /// Инициализация БВ
    void initFastSwitch();

    /// Инициализация приборов управления тормозами
    void initBrakesControl(QString module_path);

    /// Инициализация защит
    void initProtection();

    /// Инициализация подсистемы снабжения электровоза сжатым воздухом
    void initAirSupplySubsystem();

    /// Инициализация схемы управления тягой
    void initTractionControl();

    /// Инициализация тормозного оборудования
    void initBrakesEquipment(QString module_path);

    /// Инициализация ЭДТ
    void initEDT();

    /// Инициализация вспомогательного оборудования
    void initSupportEquipment();

    /// Инициализация прочего оборудования
    void initOtherEquipment();

    /// Инициализация динамически добавляемого оборудования
    void initRelays();
    void initSwitches();
    void initLamps();
    void initAZV();
    void initPVs();

    /// Инициализация приборов ЭПТ
    void initEPT();


    //void initModbus();

    void initSounds();

    /// Инициализация списка звуков перестука
    void initTapSounds();

    /// Инициализация регистратора
    void initRegistrator();

    /// Общая инициализация локомотива
    void initialization();

    /// Подпрограмма изменения положения пакетника

    void setSwitcherState(Switcher *sw, signal_t signal);

    /// Моделирование работы токоприемников
    void stepPantographs(double t, double dt);

    /// Моделирование работы рычажки
    void stepBrakesMech(double t, double dt);

    void stepFastSwitch(double t, double dt);

    void stepProtection(double t, double dt);

    void stepTractionControl(double t, double dt);

    void stepAirSupplySubsystem(double t, double dt);

    void stepBrakesControl(double t, double dt);

    void stepBrakesEquipment(double t, double dt);

    void stepEDT(double t, double dt);

    void stepSupportEquipment(double t, double dt);

    void stepEPT(double t, double dt);

    void stepDebugMsg(double t, double dt);

    void stepSignals();

    void stepSwitcherPanel();

    void stepTapSound();

    void stepDecodeAlsn();

    /// Шаг моделирования всех систем локомотива в целом
    void step(double t, double dt);

private slots:

    void setSignal(const float &state, const int &signal)
    {
        analogSignal[signal] = state;
    }

};

#endif // CHS4TOO
