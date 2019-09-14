// localization.cpp
#include <mbed.h>
#include <BNO055.h>

#define EN_XA PA_7
#define EN_XB PC_7
#define EN_YA PA_9
#define EN_YB PB_4

#define WHEEL_RADIUS 30.0 //[mm]
double en_distance[2]={10.0,10.0};

#define PI 3.141592

extern InterruptIn UB;
extern DigitalOut led;
extern Serial pc;

// global
double pos[2] = {0,0};
double yaw = 0;
double oldyaw = 0;

int nowcount[2] = {0,0};
int oldcount[2]={0,0};
double pos_delta[2]={0,0};

// BNO
extern BNO055 bno;  //(PB_9, PB_8) 

// encoder
DigitalIn encoder_x[2]={DigitalIn(EN_XA),DigitalIn(EN_XB)};
DigitalIn encoder_y[2]={DigitalIn(EN_YA),DigitalIn(EN_YB)};

// A rise
InterruptIn switchevent_x(EN_XA);
InterruptIn switchevent_y(EN_YA);

void enx_rise(){
    if(encoder_x[0]==1)
        nowcount[0]++;
    else
        nowcount[0]--;
}

void eny_rise(){
    if(encoder_y[0]==1)
        nowcount[1]++;
    else
        nowcount[1]--;
}

// initialize encoder
void encoder_init(){
    switchevent_x.rise(enx_rise);
    switchevent_y.rise(eny_rise);
}

// initialize BNO
void bno_init(){
  bno.reset();
  // connection error
  led = 1;
  if (!bno.check())
  {
    pc.printf("!!BNO_ERROR!!\n");
    while (1)
    {
      led = !led;
      wait(0.05);
    }
  }
  led = 0;
  pc.printf("BNO_CONNECTED\n");

  bno.setmode(OPERATION_MODE_IMUPLUS);
}

void readbno(){
    bno.get_angles();
    if(yaw<bno.euler.yaw-180)
        yaw=bno.euler.yaw+360;
    if(yaw>bno.euler.yaw+180)
        yaw=bno.euler.yaw+360;
}

void calc_position(){
    readbno();
    for(int i=0;i<2;i++){
        pos_delta[i]=(double)(nowcount[i]-oldcount[i])*2*PI/360*WHEEL_RADIUS;
    }
    // trans coordinate(robot_center)
    pos_delta[0]+=en_distance[0]*(yaw-oldyaw)*PI/180;
    pos_delta[1]-=en_distance[1]*(yaw-oldyaw)*PI/180;

    pos[0]+=(pos_delta[0]*cos(yaw*PI/180)+pos_delta[1]*sin(yaw*PI/180));
    pos[1]+=(-pos_delta[0]*sin(yaw*PI/180)+pos_delta[1]*cos(yaw*PI/180));
    
    // trans coordinate(field)
    for(int i=0;i<2;i++){
        oldcount[i]=nowcount[i];
    }
    oldyaw = yaw;
}