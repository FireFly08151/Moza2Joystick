#include <QtTest>
#include <QString>
#include "mozaAPI.h"
#include <windows.h>
#include <ShlObj.h>
#include <filesystem>
#include <string>
#include <QWidget>
#include "effects\EffectException.h"
#include "effects\WinDirectInputApiException.h"
class APITest : public QObject
{
    Q_OBJECT
public:
    APITest() = default;
    void Motor();
    void SteeringWheel();
    void DisplayScreen();
    void Pedals();
    void Handbrake();
    void Shifter();
    void Direct();
    void HID();
    void moveTo(int16_t steeringWheelAngle, int16_t speed);
private slots:
    void test()
    {
        moza::installMozaSDK();
        QThread::msleep(3000);
//        Motor();
//        SteeringWheel();
        //DisplayScreen();
        //Pedals();
        //Handbrake();
        //Shifter();
//        Direct();
        HID();
//        QWidget widget;
//        widget.show();
//        ERRORCODE err = NORMAL;
//        moza::motorMoveTo((HWND)widget.winId(),400,160, err);
//        moza::motorStopMove();
//        while(1)
//        {

//        }
//        moveTo(400,160);
        moza::removeMozaSDK();
    }
private:
};

#define DT             0.005
#define MATH_PI        3.1415926535f
#define DEG_TO_RPM_PER_MIN (60.0 / 360)
#define CONSTANT_FORCE_MAX 800
float pos_kp = 1.0;
float pos_ki = 0;
float pos_err_integ = 0.f;
float pos_error = 0;
float spd_ref = 0.f;
float spd_err = 0.f;
float spd_err_integ = 0.f;
// R9
// float spd_kp = 2.f;
// float spd_ki = 200.f;
// R16
float spd_kp = 5.f;
float spd_ki = 40.0f;
float spd_kd = 0.0f;
float spd_derivative = 0.f;
float spd_pre_err = 0;
float torque_ref = 0.f;
float current_spd = 0.f;
float delta_theta = 0.f;
float pre_theta = 0.f;
bool flag = false;

void FloatLimit(float *var, float lim)
{
    (*var) = ((*var) > lim) ? lim : (*var);
    (*var) = ((*var) < -lim) ? -lim : (*var);
}

float deg2rad(float x)
{
    return (x * MATH_PI / 180.f);
}

static bool areFloatsEqualWithinTolerance(float a, float b, float tolerance = 0.01f) {
    // 使用fabs函数计算绝对值
    return std::fabs(a - b) <= tolerance;
}

void APITest::moveTo(int16_t steeringWheelAngle, int16_t speed)
{
    QWidget widget;
    widget.show();
    ERRORCODE err = NORMAL;
    float target_pos = steeringWheelAngle;
    float curr_pos = 0;
    float current_spd = 0.f;
    float target_ref = 0.f;

    auto constantForce = moza::createWheelbaseETConstantForce((HWND)widget.winId(),err);
    if(!constantForce)
    {
        qDebug()<<"no constantForce";
        return;
    }

    QThread::msleep(500);

    constantForce->setDuration(0xffff);
    constantForce->setMagnitude(0);
            try{constantForce->start();}
            catch(...){qDebug()<<"测试-effect 启动停止(循环)：";
            }

    const HIDData* d = NULL;

    while (1)
    {
        d = moza::getHIDData(err);
        if (!std::isnan(d->fSteeringWheelAngle))
        {
            if(areFloatsEqualWithinTolerance(d->fSteeringWheelAngle,
                                              steeringWheelAngle))
            {
                constantForce->setMagnitude(0);
                QThread::msleep(5);
                break;
            }

            if (flag == false)
            {
                pre_theta = d->fSteeringWheelAngle;
                flag = true;
            }
            curr_pos = d->fSteeringWheelAngle;
            // 计算电机当前转速
            delta_theta = curr_pos - pre_theta;
            current_spd = delta_theta / DT * DEG_TO_RPM_PER_MIN;
            // 更新角度值
            pre_theta = curr_pos;

            // 位置控制
            pos_error = target_pos - curr_pos;
            pos_err_integ += pos_error * DT * pos_ki;
            float spd_ref = pos_err_integ + pos_kp * pos_error;

            FloatLimit(&spd_ref, (float)speed);

            // 速度环控制
            spd_err = (spd_ref - current_spd);
            spd_err_integ += spd_err * DT * spd_ki;
            spd_derivative = (spd_err - spd_pre_err) / DT;
            torque_ref = spd_err_integ + spd_err * spd_kp + spd_derivative * spd_kd;
            spd_pre_err = spd_err;
            target_ref = -torque_ref;
            FloatLimit(&target_ref, CONSTANT_FORCE_MAX);

            constantForce->setMagnitude(target_ref);

            // std::cout<<"error_pos:" <<pos_error <<"target_pos:" <<target_pos <<"curr_pos:" <<curr_pos <<"delta_theta:" <<delta_theta<<"speed_rpm:"<< current_spd << "target_ref:" << target_ref<<std::endl;
        }
        QThread::msleep(5);
    }
    QEventLoop loop;
    loop.exec();
}


void APITest::Motor()
{
#if 1
    ERRORCODE err = NORMAL;

    ///////电机
    err = moza::setMotorLimitAngle(150,200);
    qDebug()<<err;
    err = moza::setMotorRoadSensitivity(8);
    err = moza::setMotorFfbStrength(100);
    err = moza::setMotorLimitWheelSpeed(99);
    err = moza::setMotorSpringStrength(98);
    err = moza::setMotorNaturalDamper(97);
    err = moza::setMotorNaturalFriction(96);
    err = moza::setMotorSpeedDamping(95);
    err = moza::setMotorPeakTorque(94);
    err = moza::setMotorNaturalInertiaRatio(150);
    err = moza::setMotorNaturalInertia(400);
    err = moza::setMotorSpeedDampingStartPoint(450);
    err = moza::setMotorHandsOffProtection(1);
    err = moza::setMotorFfbReverse(1);
    err = moza::setMotorEqualizerAmp({std::make_pair("EqualizerAmp7_5",0)});

    QThread::msleep(6000);

    auto limit =                     moza::getMotorLimitAngle(err);
    auto RoadSensitivity =           moza::getMotorRoadSensitivity(err);
    auto FfbStrength =               moza::getMotorFfbStrength(err);
    auto LimitWheelSpeed =           moza::getMotorLimitWheelSpeed(err);
    auto SpringStrength =            moza::getMotorSpringStrength(err);
    auto NaturalDamper =             moza::getMotorNaturalDamper(err);
    auto NaturalFriction =           moza::getMotorNaturalFriction(err);
    auto SpeedDamping =              moza::getMotorSpeedDamping(err);
    auto PeakTorque =                moza::getMotorPeakTorque(err);
    auto NaturalInertiaRatio =       moza::getMotorNaturalInertiaRatio(err);
    auto NaturalInertia =            moza::getMotorNaturalInertia(err);
    auto SpeedDampingStartPoint =    moza::getMotorSpeedDampingStartPoint(err);
    auto HandsOffProtectio =         moza::getMotorHandsOffProtection(err);
    auto FfbRevers =                 moza::getMotorFfbReverse(err);
    auto EqualizerAmp =              moza::getMotorEqualizerAmp(err);

    qDebug()<<"limitAngle:" <<limit->first;
    qDebug()<<"gamelimitAngle:" << limit->second;
    qDebug()<<"RoadSensitivity::"<<RoadSensitivity;
    qDebug()<<"FfbStrength::"           <<FfbStrength;
    qDebug()<<"LimitWheelSpeed =        "<<LimitWheelSpeed;
    qDebug()<<"SpringStrength =        "<<SpringStrength;
    qDebug()<<"NaturalDamper =         "<<NaturalDamper;
    qDebug()<<"NaturalFriction =       "<<NaturalFriction;
    qDebug()<<"SpeedDamping =          "<<SpeedDamping;
    qDebug()<<"PeakTorque =            "<<PeakTorque;
    qDebug()<<"NaturalInertiaRatio =   "<<NaturalInertiaRatio;
    qDebug()<<"NaturalInertia =        "<<NaturalInertia;
    qDebug()<<"SpeedDampingStartPoint ="<<SpeedDampingStartPoint;
    qDebug()<<"HandsOffProtectio =     "<<HandsOffProtectio;
    qDebug()<<"FfbRevers =             "<<FfbRevers;
    if(EqualizerAmp)
    for(const auto& it : *EqualizerAmp)
    {
        qDebug()<<"EqualizerAmp =          "<<it.first.c_str();
        qDebug()<<"EqualizerAmp =          "<<it.second;
        qDebug()<<"-------------------------";
    }
    moza::CenterWheel();
    QThread::msleep(6000);
    moza::SoftReboot();
    QThread::msleep(6000);
#endif
}

void APITest::SteeringWheel()
{
#if 1
    ERRORCODE err = NORMAL;
    err = moza::setSteeringWheelClutchPaddleAxisMode(2);
    err = moza::setSteeringWheelClutchPaddleCombinePos(90);
    err = moza::setSteeringWheelKnobMode(1);
    err = moza::setSteeringWheelJoystickHatswitchMode(1);
    err = moza::setSteeringWheelShiftIndicatorSwitch(2);
    err = moza::setSteeringWheelShiftIndicatorMode(1);
    //    err = moza::setSteeringWheelShiftIndicatorColor({"#FF00CE00",
    //                                                     "#FF00CE00",
    //                                                     "#FF00CE00",
    //                                                     "#FFFF0606",
    //                                                     "#FFFF0606",
    //                                                     "#FFFF0606",
    //                                                     "#FFFF3CFF",
    //                                                     "#FFFF3CFF",
    //                                                     "#FFFF3CFF",
    //                                                     "#FFFF3CFF"});
    //    err = moza::setSteeringWheelShiftIndicatorLightRpm({50,60,70,80,90,92,95,97,99,100});
    err = moza::setSteeringWheelSpeedUnit(1);
    err = moza::setSteeringWheelTemperatureUnit(1);
    err = moza::setSteeringWheelScreenBrightness(89);
    err = moza::setSteeringWheelScreenCurrentUI(10);
    QThread::msleep(6000);

    //    auto ShiftIndicatorBrightness       =moza::getSteeringWheelShiftIndicatorBrightness(err);
    auto   ClutchPaddleAxisMode           =moza::getSteeringWheelClutchPaddleAxisMode    (err);
    auto ClutchPaddleCombinePos         =moza::getSteeringWheelClutchPaddleCombinePos  (err);
    auto KnobMode                       =moza::getSteeringWheelKnobMode                (err);
    auto JoystickHatswitchMode          =moza::getSteeringWheelJoystickHatswitchMode   (err);
    auto ShiftIndicatorSwitch           =moza::getSteeringWheelShiftIndicatorSwitch    (err);
    auto ShiftIndicatorMode             =moza::getSteeringWheelShiftIndicatorMode      (err);
    //    auto ShiftIndicatorColor            =moza::getSteeringWheelShiftIndicatorColor     (err);
    //    auto ShiftIndicatorLightRpm         =moza::getSteeringWheelShiftIndicatorLightRpm  (err);
    auto SpeedUnit                      =moza::getSteeringWheelSpeedUnit               (err);
    auto TemperatureUnit                =moza::getSteeringWheelTemperatureUnit         (err);
    auto ScreenBrightness               =moza::getSteeringWheelScreenBrightness        (err);
    auto ScreenUIList                   =moza::getSteeringWheelScreenUIList            (err);
    auto ScreenCurrentUI                =moza::getSteeringWheelScreenCurrentUI         (err);

    qDebug()<<"ClutchPaddleAxisMode  "<<ClutchPaddleAxisMode  ;
    qDebug()<<"ClutchPaddleCombinePos"<<ClutchPaddleCombinePos;
    qDebug()<<"KnobMode              "<<KnobMode              ;
    qDebug()<<"JoystickHatswitchMode "<<JoystickHatswitchMode ;
    qDebug()<<"ShiftIndicatorSwitch  "<<ShiftIndicatorSwitch  ;
    //    qDebug()<<"ShiftIndicatorMode    "<<ShiftIndicatorMode    ;
    qDebug()<<"SpeedUnit             "<<SpeedUnit             ;
    qDebug()<<"TemperatureUnit       "<<TemperatureUnit       ;
    qDebug()<<"ScreenBrightness      "<<ScreenBrightness      ;
    //    qDebug()<<"ScreenUIList          "<<ScreenUIList          ;
    qDebug()<<"ScreenCurrentUI       "<<ScreenCurrentUI       ;

    //    QVERIFY(ClutchPaddleAxisMode == 2);
    //    QVERIFY(ClutchPaddleCombinePos == 90);
    //    QVERIFY(KnobMode == 1);
    //    QVERIFY(JoystickHatswitchMode == 1);
    //    QVERIFY(ShiftIndicatorSwitch == 2);
    //    QVERIFY(ShiftIndicatorMode == 1);
    //    QVERIFY(SpeedUnit == 1);
    //    QVERIFY(TemperatureUnit == 1);
    //    QVERIFY(ScreenBrightness == 89);
    //    QVERIFY(ScreenCurrentUI == 10);
    if(ScreenUIList)
    for(const auto& it: *ScreenUIList)
    {
        qDebug()<<"index:"<<it.first<<"  "<<"name:"<<it.second.c_str();
    }
#endif
}

void APITest::DisplayScreen()
{
#if 1
    ERRORCODE err = NORMAL;

    moza::setDisplayScreenSpeedUnit(1);
    moza::setDisplayScreenTemperatureUnit(1);
    moza::setDisplayScreenScreenBrightness(70);
    moza::setDisplayScreenScreenCurrentUI(2);
    QThread::msleep(6000);
    auto ScreenSpeedUnit        =         moza::getDisplayScreenSpeedUnit(err);
    auto ScreenTemperatureUnit  =         moza::getDisplayScreenTemperatureUnit(err);
    auto ScreenScreenBrightness =         moza::getDisplayScreenScreenBrightness(err);
    auto ScreenScreenUIList     =         moza::getDisplayScreenScreenUIList(err);
    auto ScreenScreenCurrentUI  =         moza::getDisplayScreenScreenCurrentUI(err);

    qDebug()<<"ScreenSpeedUnit       " << ScreenSpeedUnit       ;
    qDebug()<<"ScreenTemperatureUnit " << ScreenTemperatureUnit ;
    qDebug()<<"ScreenScreenBrightness" << ScreenScreenBrightness;
    qDebug()<<"ScreenScreenUIList";
    for(const auto&it : *ScreenScreenUIList)
    {
        qDebug()<<it.first << "..."<< it.second.c_str();
    }
    qDebug()<<"ScreenScreenCurrentUI " << ScreenScreenCurrentUI ;
#endif
}

void APITest::Pedals()
{

#if 1
    ERRORCODE err = NORMAL;
    err = moza::setPedalClutchOutDir(1);
    err = moza::setPedalBrakeOutDir(1);
    err = moza::setPedalAccOutDir(1);
    err = moza::setPedalBrakePressCombine(88);
    err = moza::setPedalClutchNonLinear({10,30,50,60,70});
    err = moza::setPedalAccNonLinear({20,40,60,70,80});
    err = moza::setPedalBrakeNonLinear({30,50,70,80,90});
    QThread::msleep(6000);
    auto   ClutchOutDir          = moza::getPedalClutchOutDir(err);
    auto   BrakeOutDir           = moza::getPedalBrakeOutDir(err);
    auto   AccOutDir             = moza::getPedalAccOutDir(err);
    auto   BrakePressCombine     = moza::getPedalBrakePressCombine(err);
    auto   ClutchNonLinear       = moza::getPedalClutchNonLinear(err);
    auto   AccNonLinear          = moza::getPedalAccNonLinear(err);
    auto   BrakeNonLinear        = moza::getPedalBrakeNonLinear(err);

    qDebug()<<"ClutchOutDir      "            << ClutchOutDir      ;
    qDebug()<<"BrakeOutDir       "            << BrakeOutDir       ;
    qDebug()<<"AccOutDir         "            << AccOutDir         ;
    qDebug()<<"BrakePressCombine "            << BrakePressCombine ;
    qDebug()<<"ClutchNonLinear   ";
    for(const auto& it : *ClutchNonLinear)
    {
        qDebug() << it   ;
    }
    qDebug()<<"AccNonLinear   ";
    for(const auto& it : *AccNonLinear)
    {
        qDebug()<< it   ;
    }
    qDebug()<<"BrakeNonLinear   ";
    for(const auto& it : *BrakeNonLinear)
    {
        qDebug()<< it   ;
    }
    moza::ClutchCalibrateStrat();
    QThread::msleep(6000);
    moza::ClutchCalibrateFinish();
    QThread::msleep(1000);
//    moza::AccCalibrateStrat();
//    QThread::msleep(6000);
//    moza::AccCalibrateFinish();
//    QThread::msleep(1000);
//    moza::BrakeCalibrateStrat();
//    QThread::msleep(6000);
//    moza::BrakeCalibrateFinish();
//    QThread::msleep(1000);

#endif

}

void APITest::Handbrake()
{
#if 1
    ERRORCODE err = NORMAL;

    moza::setHandbrakeOutDir(1);
    moza::setHandbrakeApplicationMode(1);
    moza::setHandbrakeNonLinear({50,60,70,80,90});

    QThread::msleep(6000);
    auto HandbrakeOutDir = moza::getHandbrakeOutDir(err);
    auto HandbrakeApplicationMode = moza::getHandbrakeApplicationMode(err);
    auto HandbrakeNonLinear = moza::getHandbrakeNonLinear(err);

    qDebug()<<"HandbrakeOutDir         "<<HandbrakeOutDir         ;
    qDebug()<<"HandbrakeApplicationMode"<<HandbrakeApplicationMode;
    qDebug()<<"HandbrakeNonLinear      ";
    for(const auto&it : *HandbrakeNonLinear)
    {
        qDebug()<<it;
    }
    moza::HandbrakeCalibrateStart();
    QThread::msleep(10000);
    moza::HandbrakeCalibrateFinish();
    QThread::msleep(1000);

#endif
}

void APITest::Shifter()
{
#if 1
    ERRORCODE err = NORMAL;

    moza::setHandingShifterAutoBlipOutput(60);
    moza::setHandingShifterAutoBlipDuration(666);
    moza::setHandingShifterAutoBlipSwitch(1);

    QThread::msleep(6000);
    auto AutoBlipOutput   = moza::getHandingShifterAutoBlipOutput(err);
    auto AutoBlipDuration = moza::getHandingShifterAutoBlipDuration(err);
    auto AutoBlipSwitch   = moza::getHandingShifterAutoBlipSwitch(err);
    qDebug()<<"AutoBlipOutput  "<<AutoBlipOutput  ;
    qDebug()<<"AutoBlipDuration"<<AutoBlipDuration;
    qDebug()<<"AutoBlipSwitch  "<<AutoBlipSwitch  ;
    moza::ShifterCalibrateStart();
    QThread::msleep(10000);
    moza::ShifterCalibrateFinish();
    QThread::msleep(1000);
#endif
}

void APITest::Direct()
{
    ERRORCODE err = NORMAL;
    QWidget widget;
    widget.show();
    std::shared_ptr<RS21::direct_input::ETSine> sine = moza::createWheelbaseETSine((HWND)widget.winId(),err);
    qDebug()<<"err is "<< err;
    if(sine)
    {
        sine->setMagnitude(500);
        sine->setDuration(2000);
        sine->setPeriod(2000);
        try{sine->start();}
        catch(...){qDebug()<<"测试-effect 启动停止(循环)：";}
    }
    QEventLoop loop;
    QTimer::singleShot(10000, &loop, SLOT(quit()));
    loop.exec();
}

void APITest::HID()
{
    ERRORCODE err = NORMAL;
    while(1) {
        QThread::msleep(100);
        const HIDData* d = moza::getHIDData(err);
//        qDebug()<<d;
//        qDebug()<<"?????????????????";
//        auto limit = moza::getMotorLimitAngle(err);
//        qDebug()<<limit->first;
//        qDebug()<<limit->second;
        if(d)
        {
//            qDebug()<<"throttle  " << d->throttle;
//        qDebug()<<"steeringWheelAngle  " <<((double)720/65535)*d->steeringWheelAngle/2;
//        qDebug()<<"now steeringWheelAngle";
//        qDebug()<<d->steeringWheelAxle;
//            qDebug()<<"clutchSynthesisShaft  " << d->clutchSynthesisShaft;
//            qDebug()<<"clutchIndependentShaftR  " << d->clutchIndependentShaftR;
//            qDebug()<<"clutchIndependentShaftL  " << d->clutchIndependentShaftL;
//            for(int i=1; i<113; i++)
//            {
//                qDebug()<<"button"<<i<<"="<<d->buttons[1].isPressed();
//            }
        }
    }
}

QTEST_MAIN(APITest)

#include "sdk_api_test.moc"
