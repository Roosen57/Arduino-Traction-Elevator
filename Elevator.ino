#include <Deque.h>

using namespace std;

//Limit Switches
#define lim11 3
#define lim12 4
#define lim21 5
#define lim22 12
#define lim31 A0
#define lim32 A1


//Driver Pins
#define Enable_door 6
#define Enable_motor 11
#define door_1 7 
#define door_2 8
#define motor_1 9
#define motor_2 10


//Led Lights
#define floor_1_led  A6
#define floor_2_led A7
#define floor_3_led A2

//Buttons
#define floor_level_1 2
#define floor_level_2 6
#define floor_level_3 A3

// Door Limit Switches
#define closed_door A4
#define opened_door A5

int t;
unsigned long time_now;

int LS[3][2] = {{lim11,lim12},{lim21,lim22},{lim31,lim32}}; //Limit Switch Pins

void update();

void wait(){
  time_now = millis();
    while(millis() < time_now + 4000){
        update();
        Serial.println("Stuck here");
    }

}

class Door {
    public:
    void close(){
        digitalWrite(door_1,1);digitalWrite(door_2,0); //Driver Input Forward
        while (digitalRead(closed_door)) update(); //Wait for Door Limit Switch
        digitalWrite(door_1,0);digitalWrite(door_2,0); //Stop Motor
    }
    void open(){
        digitalWrite(door_1,0);digitalWrite(door_2,1); //Driver input Backward
        while (digitalRead(opened_door)) update(); //Wait for Door Limit Switch
        digitalWrite(door_1,0);digitalWrite(door_2,0); //Stop Motor
    }
};
Door door;

class Motor {
    public:
    void down(int floor){
        door.close();
        Serial.println("Going Down");
        digitalWrite(motor_1,1);digitalWrite(motor_2,0); //Motor Move Down
        //delay(1000);
        while (digitalRead(LS[floor-1][0]) || digitalRead(LS[floor-1][1])) update(); //Wait for 2 Limit Switches of floor
        digitalWrite(motor_1,0);digitalWrite(motor_2,0); // Stop Motor
        door.open();
        
    }
    void up(int floor){
        door.close();
        Serial.println("Going UP");
        digitalWrite(motor_1,0);digitalWrite(motor_2,1); //Motor Move Up
        while (digitalRead(LS[floor-1][0]) || digitalRead(LS[floor-1][1])) update(); //Wait for 2 Limit Switches of floor
        digitalWrite(motor_1,0);digitalWrite(motor_2,0); //Stop Motor
        door.open();
    }
};
Motor motor;

class Elevator{
    public:
    int direction = 0;
    int current_floor = 1;
    Deque<int> queue;

    void request_floor(int floor){
        if(floor != current_floor && floor != queue[0] && floor != queue[1] && floor != queue[2])
        //Serial.println(floor);
        queue.push_back(floor);
    }
    int door(){
        return 1;
    }
    void move(){
        //Serial.print(queue[0]);Serial.print(".");Serial.println(current_floor);
        if(queue.count() == 0){
            return;
        }
        
        if(current_floor < queue[0]){
            current_floor = queue[0];
            motor.up(queue[0]);
            queue.pop_front();
            wait();

        } else {
            current_floor = queue[0];
            motor.down(queue[0]);
            queue.pop_front();
            wait();
            }
        }

};
Elevator elv;

void declaration(){ // Declare Inputs and Outputs
    pinMode(lim11, INPUT_PULLUP);pinMode(lim12, INPUT_PULLUP);pinMode(lim21, INPUT_PULLUP);pinMode(lim22, INPUT_PULLUP);pinMode(lim31, INPUT_PULLUP);pinMode(lim32, INPUT_PULLUP);
    pinMode(Enable_door, OUTPUT); pinMode(Enable_motor, OUTPUT); pinMode(door_1, OUTPUT); pinMode(door_2, OUTPUT); pinMode(motor_1, OUTPUT); pinMode(motor_2, OUTPUT);
    pinMode(floor_1_led, OUTPUT);pinMode(floor_2_led, OUTPUT);pinMode(floor_3_led, OUTPUT);
    pinMode(floor_level_1, INPUT_PULLUP); pinMode(floor_level_2, INPUT_PULLUP); pinMode(floor_level_3, INPUT_PULLUP);
    pinMode(closed_door, INPUT_PULLUP); pinMode(opened_door, INPUT_PULLUP);
}

void update(){ //Make requests
    if(!digitalRead(floor_level_1)) elv.request_floor(1);
    if(!digitalRead(floor_level_2)) elv.request_floor(2);
    if(!digitalRead(floor_level_3)) elv.request_floor(3);
    Serial.print(elv.queue[0]);Serial.print(".");Serial.print(elv.queue[1]);Serial.print(".");Serial.println(elv.queue[2]);
    
}

void setup(){
    declaration();
    Serial.begin(9600);
    Serial.println("Code is working");
    analogWrite(Enable_motor, 255);
    analogWrite(Enable_door, 255);
}

void loop(){
    update();
    elv.move();
    //                Serial.println(digitalRead(floor_level_2));Serial.println(digitalRead(floor_level_2));
}

