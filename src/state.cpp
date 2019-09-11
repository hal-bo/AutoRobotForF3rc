#include "state.h"

State::State(void)
{
    actionNum = 0;
    sideOfStage = 1;//1・・右ステージ、-1・・左ステージ
    //test用
    actionTransition[0] = Go;
    actionTransition[1] = Go;
    actionTransition[2] = Go;
    actionTransition[3] = Go;
    requirementTransition[0] = DistanceFront;
    requirementTransition[1] = DistanceRight;
    requirementTransition[2] = DistanceBack;
    requirementTransition[3] = DistanceLeft;

    /*本番用
    actionTransition[0] = Go;//North
    actionTransition[1] = Go;//East
    actionTransition[2] = Catch;//Catch
    actionTransition[3] = Go;//West
    actionTransition[4] = Go;//South
    actionTransition[5] = Go;//East
    actionTransition[6] = Go;//Receive
    actionTransition[7] = Go;//West
    actionTransition[8] = Go;//North
    actionTransition[9] = Go;//East
    actionTransition[10] = Go;//North
    actionTransition[11] = Go;//Wait
    actionTransition[12] = Go;//Finish

    requirementTransition[0] = DistanceFront;
    requirementTransition[1] = DistanceRight;
    requirementTransition[2] = DistanceBack;
    requirementTransition[3] = DistanceLeft;
    requirementTransition[4] = DistanceRight;
    requirementTransition[5] = DistanceBack;
    requirementTransition[6] = DistanceLeft;
    requirementTransition[7] = DistanceRight;
    requirementTransition[8] = DistanceBack;
    requirementTransition[9] = DistanceLeft;
    requirementTransition[10] = DistanceRight;
    requirementTransition[11] = DistanceBack;
    requirementTransition[11] = DistanceBack;
    */
}

enum Action State::GetAction(void){
    return actionTransition[actionNum];
}

enum Requirement State::GetRequirement(void){
    return requirementTransition[actionNum];
}

int State::GetActionNum(void) 
{
    return actionNum;
}

void State::Next(void)
{
    actionNum++;
    if(actionNum == 4){
        actionNum = 0;
    }
}

void State::Back(void) // Clockwise
{
    actionNum--;
    if(actionNum == -1){
        actionNum = 3;
    }
}

