#include <mbed.h>
#include "motor.h"
#include "wheel.h"
#include "state.h"
#include "VL53L0X.h"

InterruptIn UB(USER_BUTTON);
DigitalOut led(LED2);
Serial pc(USBTX, USBRX); 


//サーボモーターピン ユニバーサル基板配列
//PwmOut SR(PB_14);
//PwmOut SL(PA_15);

//サーボモーターピン プリント基板配列
PwmOut SR(PB_7);
PwmOut SL(PA_15);


//エンコーダーピン
/*
PA_7  //kuro encoder
PC_7  //siro
PA_9  //kuro encoder
PB_4  //siro
*/


//MDピン ユニバーサル基板配列
/*
PwmOut PWM_FR(PA_8);
PwmOut PWM_FL(PA_6);
PwmOut PWM_RR(PA_0);
PwmOut PWM_RL(PB_6);
DigitalOut PHASE_FR(PC_11);
DigitalOut PHASE_FL(PC_9);
DigitalOut PHASE_RR(PD_2);
DigitalOut PHASE_RL(PC_1);
*/

//MDピン プリント基板配列
PwmOut PWM_FR(PA_8);
PwmOut PWM_FL(PA_0);
PwmOut PWM_RR(PA_6);
PwmOut PWM_RL(PB_6);
DigitalOut PHASE_FR(PC_11);
DigitalOut PHASE_FL(PD_2);
DigitalOut PHASE_RR(PC_9);
DigitalOut PHASE_RL(PB_9);


//足回り
Motor FR(PWM_FR, PHASE_FR, 100, true);
Motor FL(PWM_FL, PHASE_FL, 100, true);
Motor RR(PWM_RR, PHASE_RR, 100, true);
Motor RL(PWM_RL, PHASE_RL, 100, true);
Wheel Whe(FR,FL,RR,RL,100);


//距離センサ(VL53L0X * 4)
Timer timer_vl[4];
I2C i2c(PB_9,PB_8);
VL53L0X vl[4] = {VL53L0X(&i2c,&timer_vl[0]),VL53L0X(&i2c,&timer_vl[1]),VL53L0X(&i2c,&timer_vl[2]),VL53L0X(&i2c,&timer_vl[3])};
DigitalInOut Xshut[4] = {DigitalInOut(PC_8),DigitalInOut(PB_14),DigitalInOut(PB_13),DigitalInOut(PC_5)};//FRBL
uint16_t distance_to_object[4];
int sensorNum = 4;


State st;//自動機の状態
int num = 1;//自動機のActionNum
bool act = false;//Go以外のAction中かどうか



//**メモ**//
//Action{Go,Catch,Wait,Receive,Finish,Stop};
//Requirement{DistanceFront,DistanceRight,DistanceLeft,DistanceBack,RotateRight,RotateLeft};
//**メモ**//



//障害確認
bool isFaced(uint16_t distance){
  if (distance < 100){
    pc.printf("distance:%d",distance);
    return true;
  }else{
    return false;
  }
}


//額縁取得(サーボ)
void ArmCatch(){
    SR.pulsewidth_us(500);
    SL.pulsewidth_us(500);
    wait(3);
    SR.pulsewidth_us(1200);
    SL.pulsewidth_us(1200);
    wait(3);
    act = 0; //ArmCatchモード解除
    st.Next();
}


//ActionNum操作
void light() {
    for(int i = 0;i < num; i++){
        pc.printf("*");
        led = 1;
        wait(0.15);
        led = 0;
        wait(0.15);
    }
    pc.printf("%d Ltika\r\n",num);
    num++;
    if(num == 10){
        num = 1;
    }
    st.Next();
}

int main()
{
  led = 1;

  //距離センサの初期化
    for(int i = 0; i < sensorNum; i++){
        Xshut[i].output();
        Xshut[i] = 0;//Xshutピンを０VにするとVL53L0Xをシャットダウンできる
    }
    wait(0.5);
    for(int i = 0; i < sensorNum; i++){
        pc.printf("b");
        Xshut[i].input();//Xshutピンに接続されたGPIOピンをインプットにするとVL53L0Xが起動
        wait(0.15);
        vl[i].init();//初期化
        wait(0.15);
        vl[i].setAddress((uint8_t) 44+i*2);//アドレスをデフォルトから変更しないと複数のVL53L0Xを使えない
        wait(0.15);
        vl[i].startContinuous(30);
        /*VL53L0Xの使用個数に応じてループの回数を変更すること
          存在しないVL53L0Xをいじろうとするとたぶんそこでエラーはいてとまる
        */
    }
    
    led = 0;//初期化完了
  

  while(1)
  {
    
    //距離取得
    for(int i = 0; i < sensorNum; i++){
              distance_to_object[i] = vl[i].readRangeContinuousMillimeters();
              //pc.printf("Distance[%d]: %d\r\n",i,distance_to_object[i]);
    }
    
    switch(st.GetAction()){
      case Go :
        pc.printf("*********Action::Go*************\r\n");
        switch(st.GetRequirement()){
          case DistanceFront :
            pc.printf("Requirement::DistanceFront\r\n");
            Whe.North(80);
            if(isFaced(distance_to_object[0])){
              st.Next();
              pc.printf("Faced!");
            }
            break;
          case DistanceRight :
            pc.printf("Requirement::DistanceRight\r\n");
            Whe.East(80);
            if(isFaced(distance_to_object[1])){
              st.Next();
              pc.printf("Faced!");
            }
            break;
          case DistanceBack :
            pc.printf("Requirement::DistanceBack\r\n");
            Whe.South(80);
            if(isFaced(distance_to_object[2])){
              st.Next();
              pc.printf("Faced!");
            }
            break;
          case DistanceLeft :
            pc.printf("Requirement::DistanceLeft\r\n");
            Whe.West(80);
            if(isFaced(distance_to_object[3])){
              st.Next();
              pc.printf("Faced!");
            }
            break;
          default:
            pc.printf("Requirement::NoRequirement\r\n");
            break;
        }
        break;
      case Catch :
        pc.printf("Action::Catch\r\n");
        if(!act){
          ArmCatch();
          act = 1;
        }
        
        break;
      case Wait :
        pc.printf("Action::Wait\r\n");
        Whe.Brake();
        /*test用*/
        if(isFaced(distance_to_object[0])){
              st.Next();
              pc.printf("Faced!");
        }
        break;
      case Receive :
        pc.printf("Action::Receive\r\n");
        Whe.Brake();
        //***要検討***//
        if(!act){
          act = 1;
        }
        wait(10);
        act = 0;
        st.Next();
        break;
      case Finish :
        pc.printf("Action::Finish\r\n");
        Whe.Brake();
        SR.pulsewidth_us(1200);
        SL.pulsewidth_us(1200);
        break;
      case Stop : //使わない
        pc.printf("Action::Stop\r\n");
        Whe.Brake();
        break;
    
      default:
        pc.printf("Action::NoAction\r\n");
        break;
    }


    /*test*/
    st.Next();
    //Whe.Brake();
    wait(0.5);
    //Whe.North(80);
    /*test*/

    UB.rise(&light); //actionNum操作
    }
}