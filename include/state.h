#ifndef STATE_H
#define STATE_H

#include <mbed.h>

    enum Action{GoFront,GoRight,GoLeft,GoBack,Catch,Wait,Receive,Finish,Stop};
    enum Requirement{DistanceFront,DistanceRight,DistanceLeft,DistanceBack,NoRequirement};

class State
{

private:

    int actionNum;
    enum Action actionTransition[12];
    int moveDistance[12];

public:
    /* Constructor */
    State(void);

    enum Action GetAction(void);
    int GetMoveDistance(void);
    int GetActionNum(void);
    int sideOfStage;
    void Next(void);
    void Back(void);
};

#endif /* STATE_H */