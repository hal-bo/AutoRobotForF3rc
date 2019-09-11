#include "state.h"

State::State(void)
{
    actionNum = 0;
    actionTransition[0] = Go;
    actionTransition[1] = Go;
    actionTransition[2] = Go;
    actionTransition[3] = Go;


    currentRequirement = DistanceFront;
}

enum Action State::GetAction(void){
    return actionTransition[actionNum];
}

enum Requirement State::GetRequirement(void){
    return currentRequirement;
}

int State::GetActionNum(void) 
{
    return actionNum;
}

void State::Next(void)
{
    actionNum++;
    if(actionNum == 1){
        currentRequirement = DistanceRight;
    }
    else if(actionNum == 2){
        currentRequirement = DistanceBack;
    }
    else if(actionNum == 3){
        currentRequirement = DistanceLeft;
    }
    else if(actionNum == 4){
        actionNum = 0;
        currentRequirement = DistanceFront;
    }
}

void State::Back(void) // Clockwise
{
    actionNum--;
    if(actionNum == 0){
        currentRequirement = DistanceFront;
    }else if(actionNum == 1){
        currentRequirement = DistanceRight;
    }
    else if(actionNum == 2){
        currentRequirement = DistanceBack;
    }
    else if(actionNum == 3){
        currentRequirement = DistanceLeft;
    }else if(actionNum == -1){
        actionNum = 3;
        currentRequirement = DistanceLeft;
    }
}

