#include <mbed.h>
#include "motor.h"
#include "wheel.h"
#include "state.h"
#include "VL53L0X.h"
#include <cstdlib>  // abs() for integer

InterruptIn UB(USER_BUTTON);
DigitalOut led(LED2);
Serial pc(USBTX, USBRX); 


//サーボモーターピン ユニバーサル基板配列
//PwmOut SR(PB_14);
//PwmOut SL(PA_15);

//サーボモーターピン プリント基板配列
PwmOut SR(PB_7);
PwmOut SL(PA_2);


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
DigitalOut PHASE_RL(PA_12);


//足回り
Motor FR(PWM_FR, PHASE_FR, 100, true);
Motor FL(PWM_FL, PHASE_FL, 100, true);
Motor RR(PWM_RR, PHASE_RR, 100, true);
Motor RL(PWM_RL, PHASE_RL, 100, true);
Wheel Whe(FR,FL,RR,RL,100);


//距離センサ(VL53L0X * 4)
Timer timer_vl[6];
I2C i2c(PB_9,PB_8);
VL53L0X vl[6] = {VL53L0X(&i2c,&timer_vl[0]),
                 VL53L0X(&i2c,&timer_vl[1]),
                 VL53L0X(&i2c,&timer_vl[2]),
                 VL53L0X(&i2c,&timer_vl[3]),
                 VL53L0X(&i2c,&timer_vl[4]),
                 VL53L0X(&i2c,&timer_vl[5])};
//DigitalInOut Xshut[4] = {DigitalInOut(PC_8),DigitalInOut(PB_14),DigitalInOut(PB_13),DigitalInOut(PC_5)};//universal FRBL
//DigitalInOut Xshut[4] = {DigitalInOut(PC_8),DigitalInOut(PA_1),DigitalInOut(PC_6),DigitalInOut(PC_5)};//print FRBL
DigitalInOut Xshut[6] = {DigitalInOut(PC_8),
                         DigitalInOut(PA_1),
                         DigitalInOut(PC_6),
                         DigitalInOut(PC_5),
                         DigitalInOut(PC_12),
                         DigitalInOut(PC_10)};
uint16_t distance_to_object[6];
int sensorNum = 6;


State st;//自動機の状態
int num = 1;//自動機のActionNum
bool act = false;//Go以外のAction中かどうか
bool isOn = false;
int DEFAULT_DISTANCE = 100;
int DEFAULT_SPEAD = 20000;
int receiveCount = 0;
int BORDER_OF_STRAIGHT = 100;



//**メモ**//
//Action{Go,Catch,Wait,Receive,Finish,Stop};
//Requirement{DistanceFront,DistanceRight,DistanceLeft,DistanceBack,RotateRight,RotateLeft};
//**メモ**//



//障害確認
bool isFaced(uint16_t target_distance,uint16_t faced_distance){
  if (target_distance < faced_distance){
    pc.printf("distance:%d",target_distance);
    return true;
  }else{
    return false;
  }
}

bool isAway(uint16_t target_distance,uint16_t faced_distance){
  if (target_distance > faced_distance){
    pc.printf("distance:%d",target_distance);
    return true;
  }else{
    return false;
  }
}


//額縁取得(サーボ)
void ArmCatch(){
    SR.pulsewidth_us(1500);//下げるとき
    SL.pulsewidth_us(1100);
    Whe.North(80);
    wait(3);
    Whe.Brake();
    wait(3);
    SR.pulsewidth_us(1100);//上げるとき
    SL.pulsewidth_us(1500);
    wait(3);
    act = 0; //ArmCatchモード解除
    st.Next();
}


//ActionNum操作 使わない！
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

void changeSwitch(){
  isOn = !isOn;
  led = !led;
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

      case GoFront :
        if(isOn){
          if(distance_to_object[1] < distance_to_object[3]){//右に近いなら
              int gap = distance_to_object[1] - distance_to_object[4];//前方-後方
              if(std::abs(gap)>BORDER_OF_STRAIGHT){
                Whe.Brake();
                wait(3);
                if(gap > 0){
                  Whe.RotateRight(DEFAULT_SPEAD);
                  wait(3);
                }else if(gap < 0){
                  Whe.RotateLeft(DEFAULT_SPEAD);
                  wait(3);
                }
              }else{
                Whe.North(DEFAULT_SPEAD);
              }
          }else if(distance_to_object[1] > distance_to_object[3]){//左に近いなら
              int gap = distance_to_object[3] - distance_to_object[5];//前方-後方
              if(std::abs(gap)>BORDER_OF_STRAIGHT){
                Whe.Brake();
                wait(3);
                if(gap > 0){
                  Whe.RotateRight(DEFAULT_SPEAD);
                  wait(3);
                }else if(gap < 0){
                  Whe.RotateLeft(DEFAULT_SPEAD);
                  wait(3);
                }
              }else{
                Whe.North(DEFAULT_SPEAD);
              }
            }
          
          switch(st.GetRequirement()){
            
            case DistanceFront :
              //pc.printf("Requirement::DistanceFront\r\n");

              //test
              pc.printf("Distance[0]: %d\r\n",distance_to_object[0]);
              
              if(isFaced(distance_to_object[0],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;

            case DistanceBack :
              //pc.printf("Requirement::DistanceBack\r\n");
              
              //test
              pc.printf("Distance[2]: %d\r\n",distance_to_object[2]);
              
              if(isAway(distance_to_object[2],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Away!");
              }
              break;
            default:

              break;
          }
        }
        break;
      
      case GoBack :
        if(isOn){
          if(distance_to_object[1] < distance_to_object[3]){//右に近いなら
              int gap = distance_to_object[1] - distance_to_object[4];//前方-後方
              if(std::abs(gap)>BORDER_OF_STRAIGHT){
                Whe.Brake();
                wait(3);
                if(gap > 0){
                  Whe.RotateRight(DEFAULT_SPEAD);
                  wait(3);
                }else if(gap < 0){
                  Whe.RotateLeft(DEFAULT_SPEAD);
                  wait(3);
                }
              }else{
                Whe.South(DEFAULT_SPEAD);
              }
          }else if(distance_to_object[1] > distance_to_object[3]){//左に近いなら
              int gap = distance_to_object[3] - distance_to_object[5];//前方-後方
              if(std::abs(gap)>BORDER_OF_STRAIGHT){
                Whe.Brake();
                wait(3);
                if(gap > 0){
                  Whe.RotateLeft(DEFAULT_SPEAD);
                  wait(3);
                }else if(gap < 0){
                  Whe.RotateRight(DEFAULT_SPEAD);
                  wait(3);
                }
              }else{
                Whe.South(DEFAULT_SPEAD);
              }
            }
          switch(st.GetRequirement()){
            case DistanceFront :
              //pc.printf("Requirement::DistanceFront\r\n");

              //test
              pc.printf("Distance[0]: %d\r\n",distance_to_object[0]);
              
              if(isAway(distance_to_object[0],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;

            case DistanceBack :
              //pc.printf("Requirement::DistanceBack\r\n");
              
              //test
              pc.printf("Distance[2]: %d\r\n",distance_to_object[2]);
              
              if(isFaced(distance_to_object[2],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Away!");
              }
              break;
            default:

              break;
          }
        }
        break;

      case GoRight :

        if(isOn){
          Whe.East(DEFAULT_SPEAD);
          switch(st.GetRequirement()){
            case DistanceRight :
              //pc.printf("Requirement::DistanceRight\r\n");

              //test
              pc.printf("Distance[1]: %d\r\n",distance_to_object[1]);
              
              if(isFaced(distance_to_object[1],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;

            case DistanceLeft :
              //pc.printf("Requirement::DistanceLeft\r\n");
              
              //test
              pc.printf("Distance[3]: %d\r\n",distance_to_object[3]);
              
              if(isAway(distance_to_object[3],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Away!");
              }
              break;

            case DistanceFront :
              //pc.printf("Requirement::DistanceFront\r\n");

              //test
              pc.printf("Distance[0]: %d\r\n",distance_to_object[0]);
              
              if(isFaced(distance_to_object[0],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;
          }
        }
        break;

      case GoLeft :
        if(isOn){
          Whe.West(DEFAULT_SPEAD);
          switch(st.GetRequirement()){
            case DistanceRight :
              //pc.printf("Requirement::DistanceRight\r\n");

              //test
              pc.printf("Distance[1]: %d\r\n",distance_to_object[1]);
              
              if(isAway(distance_to_object[1],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;

            case DistanceLeft :
              //pc.printf("Requirement::DistanceLeft\r\n");
              
              //test
              pc.printf("Distance[3]: %d\r\n",distance_to_object[3]);
              
              if(isFaced(distance_to_object[3],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Away!");
              }
              break;
            case DistanceFront :
              //pc.printf("Requirement::DistanceFront\r\n");

              //test
              pc.printf("Distance[0]: %d\r\n",distance_to_object[0]);
              
              if(isFaced(distance_to_object[0],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;
          }
        }
        break;

      
      /*
      case Go :
        if(isOn){
          //pc.printf("Action::Go\r\n");
          switch(st.GetRequirement()){
            case DistanceFront :
              pc.printf("Requirement::DistanceFront\r\n");
              Whe.North(80);

              //test
              pc.printf("Distance[0]: %d\r\n",distance_to_object[0]);
              
              if(isFaced(distance_to_object[0],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;
            case DistanceRight :
              pc.printf("Requirement::DistanceRight\r\n");
              Whe.East(80);

              //test
              pc.printf("Distance[1]: %d\r\n",distance_to_object[1]);
              
              if(isFaced(distance_to_object[1],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;
            case DistanceBack :
              pc.printf("Requirement::DistanceBack\r\n");
              Whe.South(80);

              //test
              pc.printf("Distance[2]: %d\r\n",distance_to_object[2]);
              
              if(isFaced(distance_to_object[2],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;
            case DistanceLeft :
              pc.printf("Requirement::DistanceLeft\r\n");
              Whe.West(80);
              
              //test
              pc.printf("Distance[3]: %d\r\n",distance_to_object[3]);
              
              if(isFaced(distance_to_object[3],DEFAULT_DISTANCE)){
                Whe.Brake();
                wait(0.5);
                st.Next();
                pc.printf("Faced!");
              }
              break;
            default:
              pc.printf("Requirement::NoRequirement\r\n");
              break;
          }
        }
        break;*/
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
        if(isFaced(distance_to_object[0],DEFAULT_DISTANCE)){
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
          receiveCount = 0;
        }
        
        if(isFaced(distance_to_object[5],DEFAULT_DISTANCE)){
          receiveCount++;
          if(receiveCount > 100){
            act = 0;
            receiveCount = 0;
            st.Next();
          }
        }else{
          if(receiveCount > 0){
            receiveCount--;
          }
        }
        
        break;
      case Finish :
        pc.printf("Action::Finish\r\n");
        Whe.Brake();
        SR.pulsewidth_us(1200);
        SL.pulsewidth_us(1200);
        isOn = false;//userButton押されるの待ち
        led =  0;
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
    //st.Next();
    //Whe.Brake();
    wait(0.0166);
    //Whe.North(80);
    /*test*/

    UB.rise(&changeSwitch);//動かなくする or 動くようにする
    //UB.rise(&light); //actionNum操作 使わない
    }
}