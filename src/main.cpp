#include <mbed.h>
#include "motor.h"
#include "wheel.h"
#include "state.h"
#include <VL53L0X.h>
#include <BNO055.h>
#include <cstdlib>  // abs() for integer

InterruptIn UB(USER_BUTTON);
DigitalOut led(LED2);
Serial pc(USBTX, USBRX,115200); 

//サーボモーターピン プリント基板配列
PwmOut SR(PB_7);
PwmOut SL(PB_3);

//エンコーダーピン
/*
PA_7  //kuro encoder
PC_7  //siro
PA_9  //kuro encoder
PB_4  //siro
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

//BNOピン
BNO055 bno(PB_9, PB_8); 

//距離センサ
Timer timer;
I2C i2c(PB_9,PB_8);
VL53L0X vlR = VL53L0X(&i2c,&timer);
uint16_t distance_to_object;

extern double pos[2];
extern double yaw;
void encoder_init();  // エンコーダの初期化
void bno_init();      // BNOの初期化
void calc_position(); // 自己位置の計算

State st;//自動機の状態
int num = 1;//自動機のActionNum
bool act = false;//Go以外のAction中かどうか
bool isOn = false;
int DEFAULT_DISTANCE = 400;//自動機の感知距離
int DEFAULT_SPEED = 4000;
int rotateSpeed = 1;
int receiveCount = 0;
int BORDER_OF_STRAIGHT = 6;
double prepos[2] = {0,0};//x,y

//額縁取得(サーボ)
void ArmCatch(){
    if(act == 0){
      SR.pulsewidth_us(1500);//下げるとき
    SL.pulsewidth_us(1100);
    }
    SR.pulsewidth_us(1500);//下げるとき
    SL.pulsewidth_us(1100);
    if(isOn){
      while(act == 1){
        if((pos[0] - prepos[0]) > st.GetMoveDistance()){
            Whe.Brake();
            prepos[0] = pos[0];
            prepos[1] = pos[1];
            SR.pulsewidth_us(1100);//上げるとき
            SL.pulsewidth_us(1500);
            wait(1);
            act = 0; //ArmCatchモード解除
            st.Next();
          }else{
            Whe.North(DEFAULT_SPEED);
          }
      }
    }
}

void changeSwitch(){
  isOn = !isOn;
  led = !led;
  bno_init();
}
bool isFaced(uint16_t target_distance,uint16_t faced_distance){
  if (target_distance < faced_distance){
    pc.printf("distance:%d",target_distance);
    return true;
  }else{
    return false;
  }
}
int main()
{
  led = 1;

  //距離センサの初期化
  wait(0.5);
  pc.printf("start");
  vlR.init();//初期化
  wait(0.15);
  vlR.startContinuous(30);
  /*VL53L0Xの使用個数に応じてループの回数を変更すること
  存在しないVL53L0Xをいじろうとするとたぶんそこでエラーはいてとまる
  */
    

  bno_init();
  encoder_init();
  led = 0;//初期化完了
  

  while(1)
  {
    
    //距離取得
    distance_to_object = vlR.readRangeContinuousMillimeters();
    pc.printf("Distance: %d\r\n",distance_to_object);
    pc.printf("x : %.1f,y:%.1f  ",pos[0],pos[1]);
    pc.printf("rotation : %.1f  ",yaw);
    //bno.get_angles();
    //pc.printf("[roll,pitch,yaw] = [%.2f  %.2f  %.2f]\r\n", bno.euler.roll, bno.euler.pitch, bno.euler.yaw);
    
    // 自己位置の計算
    calc_position();
    /*
    if(std::abs(yaw > BORDER_OF_STRAIGHT)){//角度補正
      Whe.Brake();
      wait(3);
      double speed = DEFAULT_SPEED * yaw;
      rotateSpeed = (int)speed;
      if(yaw > 0){
        Whe.RotateLeft(rotateSpeed);
        wait(0.5);
      }else if(yaw < 0){
        Whe.RotateRight(rotateSpeed);
        wait(0.5);
      }
            
    }

    //double pos[2]
    //double yaw
  

    switch(st.GetAction()){

      case GoFront :
        if(isOn){
          if((pos[1] - prepos[1]) > st.GetMoveDistance()){
            Whe.Brake();
            st.Next();
            prepos[0] = pos[0];
            prepos[1] = pos[1];
          }else{
            Whe.North(DEFAULT_SPEED);
          }
        }
        break;
      
      case GoBack :
        if(isOn){
          if((prepos[1] - pos[1]) > st.GetMoveDistance()){
            Whe.Brake();
            st.Next();
            prepos[0] = pos[0];
            prepos[1] = pos[1];
          }else{
            Whe.South(DEFAULT_SPEED);
          }
          
        }
        break;

      case GoRight :

        if(isOn){
          if((pos[0] - prepos[0]) > st.GetMoveDistance()){
            Whe.Brake();
            st.Next();
            prepos[0] = pos[0];
            prepos[1] = pos[1];
          }else{
            Whe.East(DEFAULT_SPEED);
          }
          
        }
        break;

      case GoLeft :
        if(isOn){
          if((prepos[0] - pos[0]) > st.GetMoveDistance()){
            Whe.Brake();
            st.Next();
            prepos[0] = pos[0];
            prepos[1] = pos[1];
          }else{
            Whe.West(DEFAULT_SPEED);
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
        break;
      case Receive :
        pc.printf("Action::Receive\r\n");
        Whe.Brake();
        if(!act){
          act = 1;
          receiveCount = 0;
        }
        
        if(isFaced(distance_to_object,DEFAULT_DISTANCE)){
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
        isOn = false;
        break;
    
      default:
        pc.printf("Action::NoAction\r\n");
        break;
  
    
    }
    */
    wait(0.0166);

    UB.rise(&changeSwitch);//動かなくする or 動くようにする
  }
}