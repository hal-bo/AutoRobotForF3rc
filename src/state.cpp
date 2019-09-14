#include "state.h"

State::State(void)
{
    actionNum = 0;
    sideOfStage = 1;//1・・右ステージ、-1・・左ステージ
    //test用
    actionTransition[0] = GoFront;
    actionTransition[1] = GoRight;
    actionTransition[2] = GoBack;
    actionTransition[3] = GoLeft;
    requirementTransition[0] = DistanceFront;
    requirementTransition[1] = DistanceRight;
    requirementTransition[2] = DistanceBack;
    requirementTransition[3] = DistanceLeft;

    moveDistance[0]  = 3430; //North
    moveDistance[1]  = 117; //East
    moveDistance[2]  = 82; //Catch//必要なし
    moveDistance[3]  = 3430;  //South
    moveDistance[4]  = 953; //East
    moveDistance[5]  = 0; //Receive//必要なし
    moveDistance[6]  = 953;  //West
    moveDistance[7]  = 3430; //North
    moveDistance[8]  = 1495; //East
    moveDistance[9]  = 795; //North
    moveDistance[10] = 0; //Finish//必要なし
    moveDistance[11] = 1490;  //West

    /*本番用
    if(sideOfStage > 0){
        //右ステージ用
        actionTransition[0]  = GoFront;    //North
        actionTransition[1]  = GoRight;    //East
        actionTransition[2]  = Catch; //Catch
        actionTransition[3]  = GoBack;    //South
        actionTransition[4]  = GoRight;    //East
        actionTransition[5]  = Receive;    //Receive
        actionTransition[6]  = GoLeft;    //West
        actionTransition[7]  = GoFront;    //North
        actionTransition[8]  = GoRight;    //East
        actionTransition[9]  = GoFront;    //North
        actionTransition[10] = Finish;    //Finish
        actionTransition[11] = GoLeft;    //West

        requirementTransition[0]  = DistanceFront; //North
        requirementTransition[1]  = DistanceLeft; //East
        requirementTransition[2]  = NoRequirement; //Catch//必要なし
        requirementTransition[3]  = DistanceBack;  //South
        requirementTransition[4]  = DistanceLeft; //East
        requirementTransition[5]  = DistanceRight; //Receive//必要なし
        requirementTransition[6]  = DistanceLeft;  //West
        requirementTransition[7]  = DistanceFront; //North
        requirementTransition[8]  = DistanceRight; //East
        requirementTransition[9]  = DistanceFront; //North
        requirementTransition[10] = Norequirement; //Finish//必要なし
        requirementTransition[11] = DistanceLeft;  //West

    

    }else if(sideOfStage < 0){

        //左ステージ用
        actionTransition[0]  = GoFront;    //North
        actionTransition[1]  = GoLeft;    //West
        actionTransition[2]  = Catch; //Catch
        actionTransition[3]  = GoBack;    //South
        actionTransition[4]  = GoLeft;    //West
        actionTransition[5]  = Receive;    //Receive
        actionTransition[6]  = GoRight;    //East
        actionTransition[7]  = GoFront;    //North
        actionTransition[8]  = GoLeft;    //West
        actionTransition[9]  = GoFront;    //North
        actionTransition[10] = Finish;    //Finish
        actionTransition[11] = GoRight;    //East

        requirementTransition[0]  = DistanceFront;  //North
        requirementTransition[1]  = DistanceLeft;   //West
        requirementTransition[2]  = Norequirement;  //Catch//必要なし
        requirementTransition[3]  = DistanceBack;   //South
        requirementTransition[4]  = DistanceLeft;   //West
        requirementTransition[5]  = DistanceLeft;   //Receive
        requirementTransition[6]  = DistanceRight;  //East
        requirementTransition[7]  = DistanceFront;  //North
        requirementTransition[8]  = DistanceLeft;   //West
        requirementTransition[9]  = DistanceFront;  //North
        requirementTransition[10] = Norequirement; //Finish//必要なし
        requirementTransition[11] = DistanceLeft;  //East
    }
    
    */
}

enum Action State::GetAction(void){
    return actionTransition[actionNum];
}

enum Requirement State::GetRequirement(void){
    return requirementTransition[actionNum];
}

int State::GetMoveDistance(void){
    return moveDistance[actionNum];
}

int State::GetActionNum(void) 
{
    return actionNum;
}

void State::Next(void)
{
    actionNum++;

    //test用
    if(actionNum == 4){
        actionNum = 0;
    }
}

void State::Back(void) // Clockwise
{
    actionNum--;

    //test用
    if(actionNum == -1){
        actionNum = 3;
    }
}

