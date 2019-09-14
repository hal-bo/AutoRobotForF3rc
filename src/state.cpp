#include "state.h"

State::State(void)
{
    actionNum = 0;
    sideOfStage = 1;//1・・右ステージ、-1・・左ステージ
    //test用
    actionTransition[0] = GoFront;
    actionTransition[1] = GoFront;
    actionTransition[2] = GoFront;
    actionTransition[3] = GoFront;
    rotateDirection[0]  = 90; //North
    rotateDirection[1]  = 90; //East
    rotateDirection[2]  = 90; //Catch//必要なし
    rotateDirection[3]  = 90;  //South

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
        actionTransition[12] = Stop;   //End

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
        actionTransition[12] = Stop;   //End

    }
    
    */
}

enum Action State::GetAction(void){
    return actionTransition[actionNum];
}


int State::GetRotateDirection(void){
    return rotateDirection[actionNum];
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

