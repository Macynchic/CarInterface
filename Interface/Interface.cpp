#include <iostream>
#include <math.h>
using namespace std;

//Тут просто описаны интерфейсы, это не интересно
class ICar
{
public:

    virtual void EngineStart() = 0;

    virtual void EngineStop() = 0;

    virtual void Refuel(double liters) = 0;

    virtual void RunningIdle() = 0;

protected:

    bool engineIsRunning = false;

};

class IEngine
{
public:

    virtual void Consume(double liters) = 0;

    virtual void Start() = 0;

    virtual void Stop() = 0;

protected:

    bool isRunning = false;
};

class IFuelTank
{
    

public:

    virtual void Consume(double liters) = 0;

    virtual void Refuel(double liters) = 0;

private:

    double fillLevel = 0.0;

    bool isOnReserve = false;

    bool isComplete = false;
};

class IFuelTankDisplay
{

protected:

    double fillLevel = 0.0;

    bool isOnReserve = false;

    bool isComplete = false;
};
//Конец описания интерфейсов


/* Так, начнем с того, что интерфейсы в данной задаче используются довольно специфическим образом
   если быть точнее, то тут неправильно спроектиованы интерфейсы - их состояния (атрибуты) находятся в состоянии protected, НО!
   реализация интерфейсов по условиям задачи указана как public. Тогда появляется вопрос - зачем нам нужны атрибуты зашитые в интерфейсы
   ответ - нахрен не нужны. просто уточнение того, что должно быть в классе, который реализует интерфейс */

class Engine : public IEngine
{

public:
      
/*Создадим конструктор, который будет принимать в себя указатель на объект Интерфейса.Указатель на суперкласс является указателем на все его расширения
  сюда мы будем пподключать наш топливный бак. Отсюдая пошел проектировать топливный бак (иди к топливному баку) */

    Engine(IFuelTank *pfuelTank)                    //Конструктор для движка
    {
        fuelTank = pfuelTank;
    }

    IFuelTank* fuelTank;

    void Consume(double liters) override            //Этот метод посылает сообщение подключенному к двигателю топливному баку. 
                                                    //Вызывая его, вызываем такой-же у подключенного топливного бака
    {
        fuelTank->Consume(liters); 
    }
/* Вот тут начинается заковыка. Объясняю. По идее, двигатель как объект должен получасть все состояния топливного бака. И перед тем как запуститься
двигатель должен проводить проверку атрибутов топливного бака на его состояние. Но это невозможно используя указатель на интерфейс в котором, вроде как,
и храняться состояния, потому что они спрятаны в модификатор private. Таким образом проверку состояния я буду осуществлять через объект класса
FuelTankDisplay уже непосредственно в объекте Car. Как бы машина будет анализировать состояние и топливного бака и двигателя через Дисплей*/
    void Start() override                          
    {
        
        setIsRunning(true);                         //Поэтому тут просто перевод состояния двигателя в включенный
        
    }

    void Stop() override
    {
        setIsRunning(false);                        //Или выключеный
    }


    bool getIsRunning()                             //Гетер состояния двигателя
    {
        return isRunning;
    }

protected:
    
    void setIsRunning(bool state)                   //Сетер состояния двигателя. Спрячем ото всех.
    {                                               //Идем к FuelTankDisplay
        isRunning = state;
    }
    
    bool isRunning = false;
 
};

class FuelTank : public IFuelTank
{
   
    friend class FuelTankDisplay;           //Объявим класс FuelTankDisplay дружественным для того, чтобы у дисплея был доступ к реальному 
                                            //состоянию топливного бака
    
public:

    void Consume(double liters) override    //Дальше просто. вот тут расход топлива за цикл
    {
        if (fillLevel > 0)
        {
            fillLevel -= liters;
        }
        
        if (fillLevel < 5)                  //Если уровень топлива ниже пяти литров, загореться лампочке  isOnReserve  
        {
            isOnReserve = true;
        }

        isComplete = false;                 //Как только запустится двигатель, объем топлива будет точно ниже чем максимальный уже на первом цикле. 
                                            //Тогда погаснет лампочка isComplete

    }

    void Refuel(double liters) override     //Тут идет заправка машины
    {
        
        fillLevel += liters;                //Доливаем топливо в бак
        
        if (fillLevel > 5)                  //Проверяем состояние лампочек. Если топлива больше чем 5 литров, то isOnReserve погасла
        {
            isOnReserve = false;
        }

        if (fillLevel >= 60)                //Проверяем состояние лампочек. Если топлива больше или равно 60 литров, то isOnReserve погасла
        {
            isComplete = true;
        }                                   
        else                                //На всякий случай - погасить лампочку isComplete если бак не полный
        {
            isComplete = false;
        }
    }

    void setFillLevel(double liters)        //Вот тут сеттер для установки начального уровня топлива при создании машины
    {
        fillLevel = liters;                 //хардкод для задания значения объема бака
        
        if (fillLevel < 5)                  //И сразу проверяем состояние (атрибуты) нашего топливного бака по входным параметрам значения fillLevel
        {
            isOnReserve = true;
        }
        if (fillLevel > 5)                  //Если больше пятерки isOnReserve не горит, меньше - горит
        {
            isOnReserve = false;
        }
        if (liters == 60)                   //Если старт - максимальный объем то зажжем isComlete
        {
            isComplete = true;
        }
    }

protected:

    double fillLevel = 0.0;                 //Состояния будущих объектов. Возвращаемся назад к двигателю

    bool isOnReserve = false;

    bool isComplete = false;
};

class FuelTankDisplay : public IFuelTankDisplay
{
protected:

    FuelTank* myFuelTank;                               //Берем указатель на объект класса FuelTank и прячем от пользователя

public:
        
    FuelTankDisplay(FuelTank *currentFuelTank)          //Тут у нас конструктор
    {
        myFuelTank = currentFuelTank;
    }

    double getFillLevel()                               //Дальше все просто. Просто выкидываем атрибуты подключенного топливного бака через объект дисплея
    { 
        return round((myFuelTank->fillLevel)*10)/10;
    }

    bool getIsOnReserve()                       
    {
        return myFuelTank->isOnReserve;
    }

    bool getIsComplete()
    { 

        return myFuelTank->isComplete;
    }


protected:

    double fillLevel = 0.0;                             //Атрибуты класса

    bool isOnReserve = false;

    bool isComplete = false;
};

class Car : public ICar
{
protected:

    FuelTank* fuelTank = new FuelTank;                  //Создаем обычный топливный бак
    Engine* engine = new Engine(fuelTank);              //Создаем двигатель. Подключаем к нему наш топливный бак

public:
    
    FuelTankDisplay* fuelTankDisplay = new FuelTankDisplay(fuelTank);       //Создаем дисплей. Подключаем к нему топливный бак

    Car()                                                                   //Конструктор дефолтный. Создает бибику с 20 литрами топляка в баке
    {
        fuelTank->setFillLevel(20);
    }
    
    
    Car(double liters)                                                      //Конструктор недефолтный. Создает бибику с заданным кол-вом топляка
    {
        if (liters > 0)
        {
            fuelTank->setFillLevel(liters);
            if (liters > 60)
            {
                fuelTank->setFillLevel(60);
            }
        }
    }

    ~Car()                                                           //Деструктор. Когда уничтожаем бибику - чистим за собой движок, топливный бак и дисплей
    {
        delete engine;
        delete fuelTankDisplay;
        delete fuelTank;      
    }
    
    void EngineStart() override                                      //Завести двигатель. Если топляк больше нуля - запускаем двигатель. Передаем состояние 
                                                                     //двигателя в engineIsRunning машины
    {
        if (fuelTankDisplay->getFillLevel() > 0)
        {
            engine->Start();
            engineIsRunning = engine->getIsRunning();
        }
    }
    
    void EngineStop() override                                        //Остановить двигатель. Состояние двигателя передать в engineIsRunning машины
    {
        engine->Stop();
        engineIsRunning = engine->getIsRunning();
    }

    void Refuel(double liters) override                             //Дозаправка
    {
        if (liters > 0)                                             //Проверка что не заливаем отрицательное значение
        {
            engine->fuelTank->Refuel(liters);
        }
       

        if (fuelTankDisplay->getFillLevel() > 60)                   //Защита от перелива. Если больше чем 60 значит мы залили полный бак
        {
            fuelTank->setFillLevel(60);
        }
    }

    void RunningIdle() override                                     //Собственно работа самого двигателя
    {
        if (engine->getIsRunning())                                 //Проверка что двигатель запущен
        {
            engine->Consume(0.0003);                                //Если запущен, то откачка топлива по 0.0003 за цикл
        }
        if (fuelTankDisplay->getFillLevel() <= 0)                   //Как только топливо на нуле - выключить двигатель
        {
            EngineStop();
        }
        
    }

    bool getEngineIsRunning()                                       //Гетер для получения состояния двигателя
    {
        return engineIsRunning;                                 
    }

protected:
    
    bool engineIsRunning = false;                                   //Состояние машины
};


int main()
{
    Car MyCar(60);
    
    cout << MyCar.fuelTankDisplay->getIsComplete() << endl;
}   

