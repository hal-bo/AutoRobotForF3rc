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
    if(sideOfStage > 0){
        //右ステージ用
        actionTransition[0]  = Go;    //North
        actionTransition[1]  = Go;    //East
        actionTransition[2]  = Catch; //Catch
        actionTransition[3]  = Go;    //South
        actionTransition[4]  = Go;    //East
        actionTransition[5]  = Go;    //Receive
        actionTransition[6]  = Go;    //West
        actionTransition[7]  = Go;    //North
        actionTransition[8]  = Go;    //East
        actionTransition[9]  = Go;    //North
        actionTransition[10] = Go;    //Finish
        actionTransition[11] = Go;    //West

        requirementTransition[0]  = DistanceFront; //North
        requirementTransition[1]  = DistanceRight; //East
        requirementTransition[2]  = DistanceFront; //Catch//必要なし
        requirementTransition[3]  = DistanceBack;  //South
        requirementTransition[4]  = DistanceRight; //East
        requirementTransition[5]  = DistanceRight; //Receive//必要なし
        requirementTransition[6]  = DistanceLeft;  //West
        requirementTransition[7]  = DistanceFront; //North
        requirementTransition[8]  = DistanceRight; //East
        requirementTransition[9]  = DistanceFront; //North
        requirementTransition[10] = Norequirement; //Finish//必要なし
        requirementTransition[11] = DistanceLeft;  //West

    }else if(sideOfStage < 0){

        //左ステージ用
        actionTransition[0]  = Go;    //North
        actionTransition[1]  = Go;    //West
        actionTransition[2]  = Catch; //Catch
        actionTransition[3]  = Go;    //South
        actionTransition[4]  = Go;    //West
        actionTransition[5]  = Go;    //Receive
        actionTransition[6]  = Go;    //East
        actionTransition[7]  = Go;    //North
        actionTransition[8]  = Go;    //West
        actionTransition[9]  = Go;    //North
        actionTransition[10] = Go;    //Finish
        actionTransition[11] = Go;    //East

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

