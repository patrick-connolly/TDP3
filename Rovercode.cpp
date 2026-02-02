#include "mbed.h"


// Blinking rate in milliseconds
#define BLINKING_RATE     5000ms
PwmOut Right_motor(PTD4);
PwmOut Left_motor(PTA12);
DigitalOut direction_left_1(PTA4);
DigitalOut direction_left_2(PTA5);
DigitalOut direction_right_1(PTC8);
DigitalOut direction_right_2(PTC9);
float period_motor = 0.001f; //Period of motor
float motor_r_power = 0.0f; //Duty cycle of right motor
float motor_l_power = 0.0f; //Duty cycle of right motor
float motor_r_power_c = 0.0f;
float motor_l_power_c = 0.0f;
int d_l_1 = 0;
int d_l_2 = 0;
int d_r_1 = 0;
int d_r_2 = 0;
volatile int mode = 0;
volatile int last_mode = 0;
int motor_counter = 0;
float t_m = 2500;
int m_movement_control;
float d_1;
float d_2;
int ultrasonic_mode;
float mode_movement_right_angle;
int s_1, s_2, s_3, s_4, s_5;

void movement(){
    switch (mode){
        case 1: //Stop
            motor_l_power_c = 1.0f;
            motor_r_power_c = 1.0f;
            d_l_1 = 1;
            d_l_2 = 1;
            d_r_1 = 1;
            d_r_2 = 1;
            break;
        case 2: //Forward
            motor_l_power_c = 1.0f;
            motor_r_power_c = 1.0f;
            d_l_1 = 1;
            d_l_2 = 0;
            d_r_1 = 1;
            d_r_2 = 0;
            break;
        case 3: //Backward
            motor_l_power_c = 1.0f;
            motor_r_power_c = 1.0f;
            d_l_1 = 0;
            d_l_2 = 1;
            d_r_1 = 0;
            d_r_2 = 1;
            break;
        case 4: //Left turn (curve)
            motor_l_power_c = 0.5f;
            motor_r_power_c = 1.0f;
            d_l_1 = 0;
            d_l_2 = 1;
            d_r_1 = 1;
            d_r_2 = 0;
            break;
        case 5: //Right turn (curve)
            motor_l_power_c = 1.0f;
            motor_r_power_c = 0.5f;
            d_l_1 = 1;
            d_l_2 = 0;
            d_r_1 = 0;
            d_r_2 = 1;
            break;
        case 6: //Left turn (still)
            motor_l_power_c = 0.75f;
            motor_r_power_c = 0.75f;
            d_l_1 = 0;
            d_l_2 = 1;
            d_r_1 = 1;
            d_r_2 = 0;
            break;
        case 7: //Right turn (still)
            motor_l_power_c = 0.75f;
            motor_r_power_c = 0.75f;
            d_l_1 = 1;
            d_l_2 = 0;
            d_r_1 = 0;
            d_r_2 = 1;
            break;
        case 0:
            motor_l_power_c = 0.0f;
            motor_r_power_c = 0.0f;
            d_l_1 = 0;
            d_l_2 = 0;
            d_r_1 = 0;
            d_r_2 = 0;
            break;
    }
}
void Output_motion(){
    motor_l_power = motor_l_power_c;
    motor_r_power = motor_r_power_c;
    Right_motor.write(motor_r_power);
    Left_motor.write(motor_l_power);
    direction_left_1 = d_l_1;
    direction_left_2 = d_l_2;
    direction_right_1 = d_r_1;
    direction_right_2 = d_r_2;
}
void movement_main(){
    if (mode != 0){
        if (last_mode != mode){
          last_mode = mode;
            mode = 0;
            movement();
            Output_motion();
            ThisThread::sleep_for(100ms);

            mode = last_mode;
            movement();
            Output_motion();
        }
        else{
            last_mode = mode;
            movement();
            Output_motion();
        }
    }
    else{
        mode = last_mode;
        movement();
        Output_motion();
    }
    
}

void sensor_movement_control(){
    if ((s_3 == 1 && s_4 == 1 && s_5 == 1) || (s_1 == 1 && s_2 == 1 && s_3 == 1)){
        mode_movement_right_angle = 1;
    }else if (s_3 == 1 && s_1 == 0 && s_2 == 0 && s_4 == 0 && s_5 == 0){
        mode = 2;
    }else if (s_2 == 1 && s_1 == 0 && s_3 == 0 && s_4 == 0 && s_5 == 0){
        mode = 6;
    }else if (s_4 == 1 && s_1 == 0 && s_2 == 0 && s_3 == 0 && s_5 == 0){
        mode = 7;
    }

}
void right_turn_movement(){
    mode = 2;
    movement();
    wait_us(100000);
    mode = 1;
    movement();
    mode = 7;
    movement();
    wait_us(100000);
}
void left_turn_movement(){
    mode = 2;
    movement();
    wait_us(100000);
    mode = 1;
    movement();
    mode = 6;
    movement();
    wait_us(100000);
}
void sensor_movement_control_right_angle(){
    mode_movement_right_angle = 1;
    if (s_3 == 1 && s_4 == 1 && s_5 == 1){
        right_turn_movement();
    }else if (s_3 == 1 && s_4 == 1 && s_5 == 1){
        left_turn_movement();
    }

}
void ultrasonic_movement_control(){
    if (ultrasonic_mode == 1){
        
    }else if(ultrasonic_mode == 0){
        if (d_1<5){
            ultrasonic_mode = 1;
        }
    }
}
void m_movement_control_switch(){
    if (d_1 < 5 || ultrasonic_mode != 0){
        m_movement_control = 3;
    }if (mode_movement_right_angle == 1){
        m_movement_control = 2;
    }
}
void mode_control(){
    switch (m_movement_control) {
        case 1:
            sensor_movement_control();
            break;
        case 2:
            sensor_movement_control_right_angle();
            break;
        case 3:
            ultrasonic_movement_control();
            break;
    }
}
int main()
{
    Right_motor.period(period_motor);
    Right_motor.write(0);
    Left_motor.period(period_motor);
    Left_motor.write(0);

    while (true) {
        movement_main();
    }
}