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

//BNOピン
BNO055 bno(PB_9, PB_8); 

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
Timer timer;
I2C i2c(PB_9,PB_8);
VL53L0X vlx = VL53L0X(&i2c,&timer);
//DigitalInOut Xshut[4] = {DigitalInOut(PC_8),DigitalInOut(PB_14),DigitalInOut(PB_13),DigitalInOut(PC_5)};//universal FRBL
//DigitalInOut Xshut[4] = {DigitalInOut(PC_8),DigitalInOut(PA_1),DigitalInOut(PC_6),DigitalInOut(PC_5)};//print FRBL
DigitalInOut(PC_8);
uint16_t distance_to_object;
int sensorNum = 6;

extern double pos[2];
extern double yaw;
void encoder_init();  // エンコーダの初期化
void bno_init();      // BNOの初期化
void calc_position(); // 自己位置の計算

State st;//自動機の状態
int num = 1;//自動機のActionNum
bool act = false;//Go以外のAction中かどうか
bool isOn = false;
int DEFAULT_DISTANCE = 100;
int DEFAULT_SPEAD = 10000;
int rotateSpeed = 1;
int receiveCount = 0;
int BORDER_OF_STRAIGHT = 6;
double prepos[2] = {0,0}//x,y



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
    if(isOn){
          if((pos[0] - prepos[0]) > st.GetMoveDistance){
            Whe.Brake();
            prepos = pos;
            SR.pulsewidth_us(1100);//上げるとき
            SL.pulsewidth_us(1500);
            wait(3);
            act = 0; //ArmCatchモード解除
            st.Next();
          }else{
            Whe.North(DEFAULT_SPEAD);
          }
          
    }
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
  bno_init();
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

    bno_init();
    encoder_init();
    led = 0;//初期化完了
  

  while(1)
  {
    
    //距離取得
    
    for(int i = 0; i < sensorNum; i++){
              distance_to_object[i] = vl[i].readRangeContinuousMillimeters();
              //pc.printf("Distance[%d]: %d\r\n",i,distance_to_object[i]);
    }
    //bno.get_angles();
    //pc.printf("[roll,pitch,yaw] = [%.2f  %.2f  %.2f]\r\n", bno.euler.roll, bno.euler.pitch, bno.euler.yaw);
    
    // 自己位置の計算
    calc_position();
    
    if(std::abs(yaw > BORDER_OF_STRAIGHT)){//角度補正
      Brake();
      wait(3);
      double speed = DEFAULT_SPEED * yaw
      rotateSpeed = (int)speed;
      if(yaw > 0){
        RotateLeft(rotateSpeed);
        wait(3);
      }else if(yaw < 0){
        RotateRight(rotateSpeed);
        wait(3);
      }
            
    }

    //double pos[2]
    //double yaw
  

    switch(st.GetAction()){

      case GoFront :
        if(isOn){
          if((pos[1] - prepos[1]) > st.GetMoveDistance){
            Whe.Brake();
            st.Next();
            prepos = pos;
          }else{
            Whe.North(DEFAULT_SPEAD);
          }
        }
        break;
      
      case GoBack :
        if(isOn){
          if((prepos[1] - pos[1]) > st.GetMoveDistance){
            Whe.Brake();
            st.Next();
            prepos = pos;
          }else{
            Whe.South(DEFAULT_SPEAD);
          }
          
        }
        break;

      case GoRight :

        if(isOn){
          if((pos[0] - prepos[0]) > st.GetMoveDistance){
            Whe.Brake();
            st.Next();
            prepos = pos;
          }else{
            Whe.East(DEFAULT_SPEAD);
          }
          
        }
        break;

      case GoLeft :
        if(isOn){
          if((prepos[0] - pos[0]) > st.GetMoveDistance){
            Whe.Brake();
            st.Next();
            prepos = pos;
          }else{
            Whe.West(DEFAULT_SPEAD);
          }
          
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