#ifdef ARDUINO_AVR_MEGA2560
#define PIN_F_DIR      55   //X
#define PIN_F_STEP     54
#define PIN_F_DISABLE  38

#define PIN_B_DIR      61   //Y
#define PIN_B_STEP     60
#define PIN_B_DISABLE  56

#define PIN_L_DIR      48   //Z
#define PIN_L_STEP     46
#define PIN_L_DISABLE  62

#define PIN_R_DIR      28   //E (E0)
#define PIN_R_STEP     26
#define PIN_R_DISABLE  24

#define PIN_D_DIR      34   //Q (E1)
#define PIN_D_STEP     36
#define PIN_D_DISABLE  30

#define PIN_U_DIR      45   //AUX
#define PIN_U_STEP     47
#define PIN_U_DISABLE  32


//from RAMPS code (for reference)
#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define E_STEP_PIN         26
#define E_DIR_PIN          28
#define E_ENABLE_PIN       24

#define Q_STEP_PIN         36
#define Q_DIR_PIN          34
#define Q_ENABLE_PIN       30
#else
//UNO config
#define PIN_F_DIR      2
#define PIN_F_STEP     3
#define PIN_B_DIR      4
#define PIN_B_STEP     5
#define PIN_L_DIR      6
#define PIN_L_STEP     7
#define PIN_R_DIR      8
#define PIN_R_STEP     9
#define PIN_D_DIR      10
#define PIN_D_STEP     11
#define PIN_U_DIR      12
#define PIN_U_STEP     13
#define PIN_DISABLE    A0
#endif

/**/
#define M_F 0
#define M_B 1
#define M_L 2
#define M_R 3
#define M_D 4
#define M_U 5

#define CCW     0
#define CW      1

/*
udlrfbBFRLDU
udlrfbudlrfbudlrfbudlrfbudlrfbudlrfbBFRLDUBFRLDUBFRLDUBFRLDUBFRLDUBFRLDU
udlrfbudlrfbBFRLDUBFRLDU
Cross pattern - RlRlUdUdfBfB
scramble/unscramble 
''.join(map( lambda a : random.choice( "udlrfbUDLRFB" ), range(25) ))
rLrddFlbdUUDfFudfbFUBRruf
FURrbufBFDUfFduuDBLfDDRlR
rLrddFlbdUUDfFudfbFUBRrufFURrbufBFDUfFduuDBLfDDRlR

scramble/descramble
blDbrrDbRBdffUfddFRurDFLbUrd
DRuBlfdRUrfDDFuFFDbrBdRRBdLB

both in one line
blDbrrDbRBdffUfddFRurDFLbUrd-DRuBlfdRUrfDDFuFFDbrBdRRBdLB

uf
udlrfb

uUdDlLrRfFbB
fluLF
fuF
buB
luL
ruR
buB
fl
fF
blDbrrDbRBd

fuluburuRuBuLuF
flbrRBLF
*/

int dirPinFromMotor( int motor ) {
    switch( motor ) {
        case M_F: return PIN_F_DIR;
        case M_B: return PIN_B_DIR;
        case M_L: return PIN_L_DIR;
        case M_R: return PIN_R_DIR;
        case M_D: return PIN_D_DIR;
        case M_U: return PIN_U_DIR;
    }
    return -1;
}

int stepPinFromMotor( int motor ) {
    switch( motor ) {
        case M_F: return PIN_F_STEP;
        case M_B: return PIN_B_STEP;
        case M_L: return PIN_L_STEP;
        case M_R: return PIN_R_STEP;
        case M_D: return PIN_D_STEP;
        case M_U: return PIN_U_STEP;
    }
    return -1;
}

bool motorsOnEh;

void motorsOn() {
#ifdef ARDUINO_AVR_MEGA2560
    digitalWrite(PIN_F_DISABLE, 0);
    digitalWrite(PIN_B_DISABLE, 0);
    digitalWrite(PIN_L_DISABLE, 0);
    digitalWrite(PIN_R_DISABLE, 0);
    digitalWrite(PIN_D_DISABLE, 0);
    digitalWrite(PIN_U_DISABLE, 0);
#else
    digitalWrite(PIN_DISABLE, 0);
#endif
}

void motorsOff() {
#ifdef ARDUINO_AVR_MEGA2560
    digitalWrite(PIN_F_DISABLE, 1);
    digitalWrite(PIN_B_DISABLE, 1);
    digitalWrite(PIN_L_DISABLE, 1);
    digitalWrite(PIN_R_DISABLE, 1);
    digitalWrite(PIN_D_DISABLE, 1);
    digitalWrite(PIN_U_DISABLE, 1);
#else
    digitalWrite(PIN_DISABLE, 1);
#endif
}

void motorsInit() {
    motorsOff();
#ifdef ARDUINO_AVR_MEGA2560
    pinMode(PIN_F_DISABLE, OUTPUT);
    pinMode(PIN_B_DISABLE, OUTPUT);
    pinMode(PIN_L_DISABLE, OUTPUT);
    pinMode(PIN_R_DISABLE, OUTPUT);
    pinMode(PIN_D_DISABLE, OUTPUT);
    pinMode(PIN_U_DISABLE, OUTPUT);
#else
    pinMode(PIN_DISABLE, OUTPUT);
#endif
}

int speed = 1;

#define STEPS_PER_090    50
#define STEPS_PER_180   100
#define INTER_STEP_DELAY    0

#include "delay_table.h"


void turn090( int motor, int dir ) {
    digitalWrite( dirPinFromMotor( motor ), dir );
    int stepPin = stepPinFromMotor( motor );
    for( int i = 0; i < STEPS_PER_090; i++ ) {
        digitalWrite( stepPin, 1 );
        digitalWrite( stepPin, 0 );
        delayMicroseconds( getDelay90(i) );
    }
    delay( getInterDelay() );
}

void turn180( int motor, int dir ) {
    digitalWrite( dirPinFromMotor( motor ), dir );
    int stepPin = stepPinFromMotor( motor );
    for( int i = 0; i < STEPS_PER_180; i++ ) {
        digitalWrite( stepPin, 1 );
        digitalWrite( stepPin, 0 );
        delayMicroseconds( getDelay180(i) );
    }
    delay( getInterDelay() );
}

void turn090_180( int motor1, int dir1, int motor2, int dir2 ) {
    digitalWrite( dirPinFromMotor( motor1 ), dir1 );
    digitalWrite( dirPinFromMotor( motor2 ), dir2 );

    int stepPin1 = stepPinFromMotor( motor1 );
    int stepPin2 = stepPinFromMotor( motor2 );
    for( int i = 0; i < STEPS_PER_180; i++ ) {
        if( i % 2 == 0 ) {//tricky part, only step half the time
            digitalWrite( stepPin1, 1 );
            digitalWrite( stepPin1, 0 );
        }
        digitalWrite( stepPin2, 1 );
        digitalWrite( stepPin2, 0 );
        delayMicroseconds( getDelay180(i) );
    }
    delay( getInterDelay() );
}

void turn180_090( int motor1, int dir1, int motor2, int dir2 ) {
    turn090_180( motor2, dir2, motor1, dir1 );
}

void turn180_180( int motor1, int dir1, int motor2, int dir2 ) {
    digitalWrite( dirPinFromMotor( motor1 ), dir1 );
    digitalWrite( dirPinFromMotor( motor2 ), dir2 );

    int stepPin1 = stepPinFromMotor( motor1 );
    int stepPin2 = stepPinFromMotor( motor2 );
    for( int i = 0; i < STEPS_PER_180; i++ ) {
        digitalWrite( stepPin1, 1 );
        digitalWrite( stepPin1, 0 );
        digitalWrite( stepPin2, 1 );
        digitalWrite( stepPin2, 0 );
        delayMicroseconds( getDelay180(i) );
    }
    delay( getInterDelay() );
}

void turn090_090( int motor1, int dir1, int motor2, int dir2 ) {
    digitalWrite( dirPinFromMotor( motor1 ), dir1 );
    digitalWrite( dirPinFromMotor( motor2 ), dir2 );

    int stepPin1 = stepPinFromMotor( motor1 );
    int stepPin2 = stepPinFromMotor( motor2 );
    for( int i = 0; i < STEPS_PER_090; i++ ) {
        digitalWrite( stepPin1, 1 );
        digitalWrite( stepPin1, 0 );
        digitalWrite( stepPin2, 1 );
        digitalWrite( stepPin2, 0 );
        delayMicroseconds( getDelay90(i) );
    }
    delay( getInterDelay() );
}


void setup() {
    //digitalWrite(PIN_U_STUPID_RESET_N, 1);
    //pinMode(PIN_U_STUPID_RESET_N,  OUTPUT);

    digitalWrite(PIN_F_DIR,  0);
    digitalWrite(PIN_F_STEP, 0);
    digitalWrite(PIN_B_DIR,  0);
    digitalWrite(PIN_B_STEP, 0);
    digitalWrite(PIN_L_DIR,  0);
    digitalWrite(PIN_L_STEP, 0);
    digitalWrite(PIN_R_DIR,  0);
    digitalWrite(PIN_R_STEP, 0);
    digitalWrite(PIN_D_DIR,  0);
    digitalWrite(PIN_D_STEP, 0);
    digitalWrite(PIN_U_DIR,  0);
    digitalWrite(PIN_U_STEP, 0);

    motorsInit();

    pinMode(PIN_F_DIR,  OUTPUT);
    pinMode(PIN_F_STEP, OUTPUT);
    pinMode(PIN_B_DIR,  OUTPUT);
    pinMode(PIN_B_STEP, OUTPUT);
    pinMode(PIN_L_DIR,  OUTPUT);
    pinMode(PIN_L_STEP, OUTPUT);
    pinMode(PIN_R_DIR,  OUTPUT);
    pinMode(PIN_R_STEP, OUTPUT);
    pinMode(PIN_D_DIR,  OUTPUT);
    pinMode(PIN_D_STEP, OUTPUT);
    pinMode(PIN_U_DIR,  OUTPUT);
    pinMode(PIN_U_STEP, OUTPUT);
    motorsOnEh = false;

    Serial.begin(115200);
    Serial.print( "+");
}



void loop() {
    char c;

    c = Serial.read();
    if( c < 0 ) return;

    switch( c ) {
        case 27:
            Serial.println( "Shutdown!");
            motorsOff();
            break;
        case ' ':
            if( motorsOnEh ) {
                Serial.println( "Off!");
                motorsOff();
                motorsOnEh = false;
            }
            else {
                Serial.println( "On!");
                motorsOn();
                motorsOnEh = true;
            }
            break;
        case '[':
            Serial.print( "[");
            motorsOn();
            motorsOnEh = true;
            break;
        case ']':
            Serial.print( "]");
            motorsOff();
            motorsOnEh = false;
            break;
#ifdef ARDUINO_AVR_MEGA2560
        case '!': Serial.println( "F motor enabled"); digitalWrite(PIN_F_DISABLE, 0); break;
        case '@': Serial.println( "B motor enabled"); digitalWrite(PIN_B_DISABLE, 0); break;
        case '#': Serial.println( "L motor enabled"); digitalWrite(PIN_L_DISABLE, 0); break;
        case '$': Serial.println( "R motor enabled"); digitalWrite(PIN_R_DISABLE, 0); break;
        case '%': Serial.println( "D motor enabled"); digitalWrite(PIN_D_DISABLE, 0); break;
        case '^': Serial.println( "U motor enabled"); digitalWrite(PIN_U_DISABLE, 0); break;
#endif

        case '1': speed = 1; Serial.print('1'); /*reportSpeedInfo();*/ break;
        case '2': speed = 2; Serial.print('2'); /*reportSpeedInfo();*/ break;
        case '3': speed = 3; Serial.print('3'); /*reportSpeedInfo();*/ break;
        case '4': speed = 4; Serial.print('4'); /*reportSpeedInfo();*/ break;
        case '5': speed = 5; Serial.print('5'); /*reportSpeedInfo();*/ break;
        case '6': speed = 6; Serial.print('6'); /*reportSpeedInfo();*/ break;
        case '7': speed = 7; Serial.print('7'); /*reportSpeedInfo();*/ break;
        case '8': speed = 8; Serial.print('8'); /*reportSpeedInfo();*/ break;
        case '9': speed = 9; Serial.print('9'); /*reportSpeedInfo();*/ break;


        //F/B
/*F   */case 'a': turn090       ( M_F,  CW           ); Serial.print('a'); break;
/*F2  */case 'b': turn180       ( M_F,  CW           ); Serial.print('b'); break;
/*F'  */case 'c': turn090       ( M_F, CCW           ); Serial.print('c'); break;
/*F B */case 'd': turn090_090   ( M_F,  CW, M_B,  CW ); Serial.print('d'); break;
/*F2B */case 'e': turn180_090   ( M_F,  CW, M_B,  CW ); Serial.print('e'); break;
/*F'B */case 'f': turn090_090   ( M_F, CCW, M_B,  CW ); Serial.print('f'); break;
/*  B */case 'g': turn090       (           M_B,  CW ); Serial.print('g'); break;
/*F B2*/case 'h': turn090_180   ( M_F,  CW, M_B,  CW ); Serial.print('h'); break;
/*F2B2*/case 'i': turn180_180   ( M_F,  CW, M_B,  CW ); Serial.print('i'); break;
/*F'B2*/case 'j': turn090_180   ( M_F, CCW, M_B,  CW ); Serial.print('j'); break;
/*  B2*/case 'k': turn180       (           M_B, CCW ); Serial.print('k'); break;
/*F B'*/case 'l': turn090_090   ( M_F,  CW, M_B, CCW ); Serial.print('l'); break;
/*F2B'*/case 'm': turn180_090   ( M_F,  CW, M_B, CCW ); Serial.print('m'); break;
/*F'B'*/case 'n': turn090_090   ( M_F, CCW, M_B, CCW ); Serial.print('n'); break;
/*  B'*/case 'o': turn090       (           M_B, CCW ); Serial.print('o'); break;

        //L/R
/*L   */case 'p': turn090       ( M_L,  CW           ); Serial.print('p'); break;
/*L2  */case 'q': turn180       ( M_L,  CW           ); Serial.print('q'); break;
/*L'  */case 'r': turn090       ( M_L, CCW           ); Serial.print('r'); break;
/*L R */case 's': turn090_090   ( M_L,  CW, M_R,  CW ); Serial.print('s'); break;
/*L2R */case 't': turn180_090   ( M_L,  CW, M_R,  CW ); Serial.print('t'); break;
/*L'R */case 'u': turn090_090   ( M_L, CCW, M_R,  CW ); Serial.print('u'); break;
/*  R */case 'v': turn090       (           M_R,  CW ); Serial.print('v'); break;
/*L R2*/case 'w': turn090_180   ( M_L,  CW, M_R,  CW ); Serial.print('w'); break;
/*L2R2*/case 'x': turn180_180   ( M_L,  CW, M_R,  CW ); Serial.print('x'); break;
/*L'R2*/case 'y': turn090_180   ( M_L, CCW, M_R,  CW ); Serial.print('y'); break;
/*  R2*/case 'z': turn180       (           M_R, CCW ); Serial.print('z'); break;
/*L R'*/case 'A': turn090_090   ( M_L,  CW, M_R, CCW ); Serial.print('A'); break;
/*L2R'*/case 'B': turn180_090   ( M_L,  CW, M_R, CCW ); Serial.print('B'); break;
/*L'R'*/case 'C': turn090_090   ( M_L, CCW, M_R, CCW ); Serial.print('C'); break;
/*  R'*/case 'D': turn090       (           M_R, CCW ); Serial.print('D'); break;

        //U/D
/*U   */case 'E': turn090       ( M_U,  CW           ); Serial.print('E'); break;
/*U2  */case 'F': turn180       ( M_U,  CW           ); Serial.print('F'); break;
/*U'  */case 'G': turn090       ( M_U, CCW           ); Serial.print('G'); break;
/*U D */case 'H': turn090_090   ( M_U,  CW, M_D,  CW ); Serial.print('H'); break;
/*U2D */case 'I': turn180_090   ( M_U,  CW, M_D,  CW ); Serial.print('I'); break;
/*U'D */case 'J': turn090_090   ( M_U, CCW, M_D,  CW ); Serial.print('J'); break;
/*  D */case 'K': turn090       (           M_D,  CW ); Serial.print('K'); break;
/*U D2*/case 'L': turn090_180   ( M_U,  CW, M_D,  CW ); Serial.print('L'); break;
/*U2D2*/case 'M': turn180_180   ( M_U,  CW, M_D,  CW ); Serial.print('M'); break;
/*U'D2*/case 'N': turn090_180   ( M_U, CCW, M_D,  CW ); Serial.print('N'); break;
/*  D2*/case 'O': turn180       (           M_D, CCW ); Serial.print('O'); break;
/*U D'*/case 'P': turn090_090   ( M_U,  CW, M_D, CCW ); Serial.print('P'); break;
/*U2D'*/case 'Q': turn180_090   ( M_U,  CW, M_D, CCW ); Serial.print('Q'); break;
/*U'D'*/case 'R': turn090_090   ( M_U, CCW, M_D, CCW ); Serial.print('R'); break;
/*  D'*/case 'S': turn090       (           M_D, CCW ); Serial.print('S'); break;


        default:
            break;
    }

}
