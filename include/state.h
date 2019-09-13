#ifndef STATE_H
#define STATE_H

#include <mbed.h>

    enum Action{GoFront,GoRight,GoLeft,GoBack,Catch,Wait,Receive,Finish,Stop};
    enum Requirement{DistanceFront,DistanceRight,DistanceLeft,DistanceBack,RotateRight,RotateLeft,NoRequirement};

class State
{

private:

    int actionNum;
    enum Action actionTransition[10];
    enum Requirement requirementTransition[10];

public:
    /* Constructor */
    State(void);

    enum Action GetAction(void);
    enum Requirement GetRequirement(void);//次アクションへ進む条件
    int GetActionNum(void);
    int sideOfStage;
    void Next(void);
    void Back(void);
};

#endif /* STATE_H */