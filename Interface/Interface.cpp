#include <iostream>
#include <math.h>
using namespace std;

//It's just external interfaces, according to the condition of the problem
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
//End of interface description


/* So, let's start with the fact that interfaces in this problem are used in a rather specific way.
    to be more precise, the interfaces are designed incorrectly here - their states (attributes) are in the protected state, BUT!
    the implementation of interfaces according to the conditions of the task is specified as public. Then the question arises - why do we need attributes wired into interfaces
    The answer is - fuck not needed. just a clarification of what should be in a class that implements an interface */

class Engine : public IEngine
{

public:
      
/*Let's create a constructor that will take a pointer to an Interface object. A pointer to a superclass is a pointer to all its extensions
   here we will connect our fuel tank. From here I went to design the fuel tank (go to the fuel tank)*/

    Engine(IFuelTank *pfuelTank)                    //engine constructor
    {
        fuelTank = pfuelTank;
    }

    IFuelTank* fuelTank;

    void Consume(double liters) override            //This method sends a message to the fuel tank connected to the engine.
                                                    //Calling it, we call the same one for the connected fuel tank
    {
        fuelTank->Consume(liters); 
    }
/* This is where the trick begins. I explain. In theory, the engine as an object should receive all the states of the fuel tank. And before you run
the engine must carry out an attribute check of the fuel tank for its condition. But this is not possible using an interface pointer in which, sort of,
and store states because they are hidden in the private modifier. Thus, I will check the state through a class object
FuelTankDisplay is already directly in the Car object. How would the machine analyze the status of both the fuel tank and the engine through the display*/
    void Start() override                          
    {
        
        setIsRunning(true);                         //Therefore, here it is simply a transfer of the engine state to on
        
    }

    void Stop() override
    {
        setIsRunning(false);                        //Or off
    }


    bool getIsRunning()                             //Engine status geter
    {
        return isRunning;
    }

protected:
    
    void setIsRunning(bool state)                   //Engine status setter. Let's hide from everyone
    {                                               //Going to FuelTankDisplay
        isRunning = state;
    }
    
    bool isRunning = false;
 
};

class FuelTank : public IFuelTank
{
   
    friend class FuelTankDisplay;           //Let's declare the class FuelTankDisplay friendly so that the display has access to the real
                                            //fuel tank condition
    
public:

    void Consume(double liters) override    //Further simply. here is the fuel consumption per cycle
    {
        if (fillLevel > 0)
        {
            fillLevel -= liters;
        }
        
        if (fillLevel < 5)                  //If the fuel level is below five liters, the isOnReserve light will turn on. 
        {
            isOnReserve = true;
        }

        isComplete = false;                 //As soon as the engine starts, the fuel volume will be exactly lower than the maximum already on the first cycle.
                                            //Then the isComplete light will turn off.

    }

    void Refuel(double liters) override     //Here comes the refueling
    {
        
        fillLevel += liters;                //Adding fuel to the tank
        
        if (fillLevel > 5)                  //Check the condition of the light bulbs. If there is more than 5 liters of fuel, then isOnReserve goes out
        {
            isOnReserve = false;
        }

        if (fillLevel >= 60)                //Check the condition of the light bulbs. If the fuel is greater than or equal to 60 liters, then isOnReserve is extinguished
        {
            isComplete = true;
        }                                   
        else                                //Just in case - turn off the isComplete light if the tank is not full
        {
            isComplete = false;
        }
    }

    void setFillLevel(double liters)        //Here is a setter for setting the initial fuel level when creating a car
    {
        fillLevel = liters;                 //hardcode for setting the value of the tank volume
        
        if (fillLevel < 5)                  //And immediately check the state (attributes) of our fuel tank according to the input parameters of the fillLevel value
        {
            isOnReserve = true;
        }
        if (fillLevel > 5)                  //If more than five isOnReserve is off, less is on
        {
            isOnReserve = false;
        }
        if (liters == 60)                   //If the start is the maximum volume, then fire isComlete
        {
            isComplete = true;
        }
    }

protected:

    double fillLevel = 0.0;                 //States of future objects. Back to the engine

    bool isOnReserve = false;

    bool isComplete = false;
};

class FuelTankDisplay : public IFuelTankDisplay
{
protected:

    FuelTank* myFuelTank;                               //We take a pointer to an object of the FuelTank class and hide it from the user

public:
        
    FuelTankDisplay(FuelTank *currentFuelTank)          //Тут у нас конструктор
    {
        myFuelTank = currentFuelTank;
    }

    double getFillLevel()                               //Then everything is simple. We simply throw out the attributes of the connected fuel tank through the display object
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

    double fillLevel = 0.0;                             //class attributes

    bool isOnReserve = false;

    bool isComplete = false;
};

class Car : public ICar
{
protected:

    FuelTank* fuelTank = new FuelTank;                  //Create a regular fuel tank
    Engine* engine = new Engine(fuelTank);              //We create an engine. We connect our fuel tank to it

public:
    
    FuelTankDisplay* fuelTankDisplay = new FuelTankDisplay(fuelTank);       //We create a display. We connect the fuel tank to it

    Car()                                                                   //Default constructor. Creates car with 20 liters of fuel in the tank
    {
        fuelTank->setFillLevel(20);
    }
    
    
    Car(double liters)                                                      //The constructor is non-default. Creates a car with the given number of fuel
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

    ~Car()                                                           //Destructor. When we destroy car, we clean the engine, fuel tank and display
    {
        delete engine;
        delete fuelTankDisplay;
        delete fuelTank;      
    }
    
    void EngineStart() override                                      //Start the engine. If the fuel is greater than zero, we start the engine. Passing state 
                                                                     //engine in engineIsRunning machine
    {
        if (fuelTankDisplay->getFillLevel() > 0)
        {
            engine->Start();
            engineIsRunning = engine->getIsRunning();
        }
    }
    
    void EngineStop() override                                        //Stop engine. Engine state to pass to engineIsRunning of the machine
    {
        engine->Stop();
        engineIsRunning = engine->getIsRunning();
    }

    void Refuel(double liters) override                             //Refueling
    {
        if (liters > 0)                                             //Checking that we do not fill in a negative value
        {
            engine->fuelTank->Refuel(liters);
        }
       

        if (fuelTankDisplay->getFillLevel() > 60)                   //Overflow protection. If more than 60, then we filled a full tank
        {
            fuelTank->setFillLevel(60);
        }
    }

    void RunningIdle() override                                     //The actual operation of the engine itself
    {
        if (engine->getIsRunning())                                 //Checking that the engine is running
        {
            engine->Consume(0.0003);                                //If running, then pumping fuel at 0.0003 per cycle
        }
        if (fuelTankDisplay->getFillLevel() <= 0)                   //As soon as the fuel is at zero - turn off the engine
        {
            EngineStop();
        }
        
    }

    bool getEngineIsRunning()                                       //Geter for getting the state of the engine
    {
        return engineIsRunning;                                 
    }

protected:
    
    bool engineIsRunning = false;                                   //Car condition
};


int main()
{
    Car MyCar(60);
    
    cout << MyCar.fuelTankDisplay->getIsComplete() << endl;
}   

