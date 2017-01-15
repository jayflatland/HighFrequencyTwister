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

#define SOLENOID_EQ         9
#define SOLENOID_XY         8

//enum Side {
#define X       1
#define Y       2
#define E       4
#define Q       8
//};

#define CCW     0
#define CW      1

//enum Side { U, D, L, R, F, B };
//enum Dir { CW, CCW, CW2 };

//enum GrabAxis {
#define XY      1
#define EQ      2
#define BOTH    (XY|EQ)
//};

bool isEngaged;

bool debug = true;

unsigned char delays[400] = {
#include "delay_table.h"
};

#define DELAY_TBL_CNT   sizeof(delays)/sizeof(delays[0])

static int speed_scale=16;

template <int CNT>
int getDelay( int i ) {
    enum {
        RAMP_UP_END = DELAY_TBL_CNT,
        RAMP_DOWN_START = CNT - DELAY_TBL_CNT,
    };
    if( i < RAMP_UP_END ) {
        return delays[i];
    }
    else if( i < RAMP_DOWN_START ) {
        return delays[DELAY_TBL_CNT-1];
    }
    else {
        return delays[CNT-1-i];
    }
    return 50;
    //DELAY_TBL_CNT
}



void safetyShutdown() {
    digitalWrite(X_ENABLE_PIN, HIGH);
    digitalWrite(Y_ENABLE_PIN, HIGH);
    digitalWrite(X_ENABLE_PIN, HIGH);
    digitalWrite(E_ENABLE_PIN, HIGH);
    digitalWrite(Q_ENABLE_PIN, HIGH);
    digitalWrite( SOLENOID_EQ, LOW );
    digitalWrite( SOLENOID_XY, LOW );

    isEngaged = false;
    if( debug ) Serial.println("SHUTTIN DOWN!!");
}

int dirPin( int s ) {
    switch( s ) {
    case X: return X_DIR_PIN;
    case Y: return Y_DIR_PIN;
    case E: return E_DIR_PIN;
    case Q: return Q_DIR_PIN;
    }
    return -1;
}

int stepPin( int s ) {
    switch( s ) {
    case X: return X_STEP_PIN;
    case Y: return Y_STEP_PIN;
    case E: return E_STEP_PIN;
    case Q: return Q_STEP_PIN;
    }
    return -1;
}

void motorsOn(int sides = X | Y | E | Q ) {
    if( sides & X ) digitalWrite(X_ENABLE_PIN, LOW);
    if( sides & Y ) digitalWrite(Y_ENABLE_PIN, LOW);
    if( sides & E ) digitalWrite(E_ENABLE_PIN, LOW);
    if( sides & Q ) digitalWrite(Q_ENABLE_PIN, LOW);
    if( debug ) Serial.println("motorsOn()");
}

void motorsOff(int sides = X | Y | E | Q ) {
    if( sides & X ) digitalWrite(X_ENABLE_PIN, HIGH);
    if( sides & Y ) digitalWrite(Y_ENABLE_PIN, HIGH);
    if( sides & E ) digitalWrite(E_ENABLE_PIN, HIGH);
    if( sides & Q ) digitalWrite(Q_ENABLE_PIN, HIGH);
    if( debug ) Serial.println("motorsOff()");
}

void grab( int axis=BOTH) {
    //delay(100);
    if( axis & XY ) {
        digitalWrite( SOLENOID_XY, HIGH );
    }
    if( axis & EQ ) {
        digitalWrite( SOLENOID_EQ, HIGH );
    }
    //delay(100);
    if( debug ) Serial.println("grab()");
}

void ungrab( int axis=BOTH) {
    //delay(100);
    if( axis & XY ) {
        digitalWrite( SOLENOID_XY, LOW );
    }
    if( axis & EQ ) {
        digitalWrite( SOLENOID_EQ, LOW );
    }
    //delay(100);
    if( debug ) Serial.println("ungrab()");
}

void toggleEngage() {
    if( isEngaged ) {
        ungrab();
        motorsOff();
        isEngaged = false;
    }
    else {
        grab();
        delay( 50 );
        motorsOn();
        isEngaged = true;
    }
    if( debug ) Serial.println("toggleEngage()");
}


#define STEP_N(deg) (int)(50.0*16.0*(float)deg/90.0)
#define STEP_45     STEP_N(45)
#define STEP_90     STEP_N(90)
#define STEP_180    STEP_N(180)

void stepMotors( int sides, int cnt ) {
    int sidesVec[4];
    int sidesCnt;
    sidesCnt = 0;
    if( sides & X ) { sidesVec[ sidesCnt++ ] = X_STEP_PIN; }
    if( sides & Y ) { sidesVec[ sidesCnt++ ] = Y_STEP_PIN; }
    if( sides & E ) { sidesVec[ sidesCnt++ ] = E_STEP_PIN; }
    if( sides & Q ) { sidesVec[ sidesCnt++ ] = Q_STEP_PIN; }
    for( int i = 0; i < cnt; i++ ) {
        int idx = i < 800 ? i : 799;

        for( int j = 0; j < sidesCnt; j++ ) {
            digitalWrite(sidesVec[j], HIGH);
        }
        for( int j = 0; j < sidesCnt; j++ ) {
            digitalWrite(sidesVec[j], LOW);
        }
        //delayMicroseconds( delays[idx] );
        delayMicroseconds( 100 );
    }
    if( debug ) Serial.println("stepMotors()");
}


void setDirs( int sides, int ccwDirSides ) {
    if( sides & X ) { if( ccwDirSides & X ) { digitalWrite( X_DIR_PIN, CCW ); } else { digitalWrite( X_DIR_PIN, CW ); } }
    if( sides & Y ) { if( ccwDirSides & Y ) { digitalWrite( Y_DIR_PIN, CCW ); } else { digitalWrite( Y_DIR_PIN, CW ); } }
    if( sides & E ) { if( ccwDirSides & E ) { digitalWrite( E_DIR_PIN, CCW ); } else { digitalWrite( E_DIR_PIN, CW ); } }
    if( sides & Q ) { if( ccwDirSides & Q ) { digitalWrite( Q_DIR_PIN, CCW ); } else { digitalWrite( Q_DIR_PIN, CW ); } }
}

#define OVERSHOOTCNT        400
#define TURN90CNT           1600
#define TURN90WOVERSHOT     (TURN90CNT+OVERSHOOTCNT)

void fast90Motors( int sides, int ccwDirSides ) {
    int sidesVec[4];
    int sidesCnt;
    sidesCnt = 0;

    setDirs( sides, ccwDirSides );
    if( sides & X ) { sidesVec[ sidesCnt++ ] = X_STEP_PIN; }
    if( sides & Y ) { sidesVec[ sidesCnt++ ] = Y_STEP_PIN; }
    if( sides & E ) { sidesVec[ sidesCnt++ ] = E_STEP_PIN; }
    if( sides & Q ) { sidesVec[ sidesCnt++ ] = Q_STEP_PIN; }

    for( int i = 0; i < TURN90CNT; i++ ) {
        for( int j = 0; j < sidesCnt; j++ ) { digitalWrite(sidesVec[j], HIGH ); }
        for( int j = 0; j < sidesCnt; j++ ) { digitalWrite(sidesVec[j], LOW  ); }
        delayMicroseconds( getDelay<TURN90CNT>(i)*speed_scale );
    }
    if( debug ) Serial.println("fast90Motors()");
}


void fast90MotorsWOvershoot( int sides, int ccwDirSides ) {
    int sidesVec[4];
    int sidesCnt;
    sidesCnt = 0;

    setDirs( sides, ccwDirSides );
    if( sides & X ) { sidesVec[ sidesCnt++ ] = X_STEP_PIN; }
    if( sides & Y ) { sidesVec[ sidesCnt++ ] = Y_STEP_PIN; }
    if( sides & E ) { sidesVec[ sidesCnt++ ] = E_STEP_PIN; }
    if( sides & Q ) { sidesVec[ sidesCnt++ ] = Q_STEP_PIN; }

    for( int i = 0; i < TURN90WOVERSHOT; i++ ) {
        for( int j = 0; j < sidesCnt; j++ ) { digitalWrite(sidesVec[j], HIGH ); }
        for( int j = 0; j < sidesCnt; j++ ) { digitalWrite(sidesVec[j], LOW  ); }
        delayMicroseconds( getDelay<TURN90WOVERSHOT>(i)*speed_scale );
    }
    
    setDirs( sides, ~ccwDirSides );
    for( int i = 0; i < OVERSHOOTCNT; i++ ) {
        for( int j = 0; j < sidesCnt; j++ ) { digitalWrite(sidesVec[j], HIGH ); }
        for( int j = 0; j < sidesCnt; j++ ) { digitalWrite(sidesVec[j], LOW  ); }
        delayMicroseconds( getDelay<OVERSHOOTCNT>(i)*speed_scale );
    }
    if( debug ) Serial.println("fast90Motors()");
}




void setup() 
{
    Serial.begin(115200);
    pinMode(SOLENOID_EQ, OUTPUT);
    pinMode(SOLENOID_XY, OUTPUT);

    pinMode(X_ENABLE_PIN, OUTPUT);
    pinMode(Y_ENABLE_PIN, OUTPUT);
    pinMode(X_ENABLE_PIN, OUTPUT);
    pinMode(E_ENABLE_PIN, OUTPUT);
    pinMode(Q_ENABLE_PIN, OUTPUT);

    motorsOff();
    ungrab();
    isEngaged = false;

    pinMode(X_STEP_PIN, OUTPUT);
    pinMode(Y_STEP_PIN, OUTPUT);
    pinMode(X_STEP_PIN, OUTPUT);
    pinMode(E_STEP_PIN, OUTPUT);
    pinMode(Q_STEP_PIN, OUTPUT);

    pinMode(X_DIR_PIN, OUTPUT);   
    pinMode(Y_DIR_PIN, OUTPUT);   
    pinMode(X_DIR_PIN, OUTPUT);   
    pinMode(E_DIR_PIN, OUTPUT);   
    pinMode(Q_DIR_PIN, OUTPUT);   

    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(E_STEP_PIN, LOW);
    digitalWrite(Q_STEP_PIN, LOW);

    digitalWrite(X_DIR_PIN, LOW);
    digitalWrite(Y_DIR_PIN, LOW);
    digitalWrite(X_DIR_PIN, LOW);
    digitalWrite(E_DIR_PIN, LOW);
    digitalWrite(Q_DIR_PIN, LOW);
}

void loop()
{
    char c;
    c = Serial.read();
    if( c < 0 ) return;

    switch( c ) {
    case 27: //escape
        safetyShutdown();
        break;
//    case 'd':
//        if( isEngaged ) {
//            ungrab();
//            digitalWrite( X_DIR_PIN, 0 );
//            digitalWrite( Y_DIR_PIN, 0 );
//            digitalWrite( E_DIR_PIN, 0 );
//            digitalWrite( Q_DIR_PIN, 0 );
//            stepMotors( X | Y | E | Q, STEP_N(70) );
//            delay( 20 );
//            motorsOff();
//            isEngaged = false;
//            if( debug ) Serial.println( "Drop!");
//        }
//        break;
//
//    case 'g':
//        if( !isEngaged ) {
//            motorsOn();
//            digitalWrite( X_DIR_PIN, 1 );
//            digitalWrite( Y_DIR_PIN, 1 );
//            digitalWrite( E_DIR_PIN, 1 );
//            digitalWrite( Q_DIR_PIN, 1 );
//            stepMotors( X | Y | E | Q, STEP_N(70) );
//            delay( 20 );
//            grab();
//            isEngaged = true;
//            if( debug ) Serial.println( "Grab!");
//        }
        break;

    case ' ':
        toggleEngage();
        break;

    //scramble XQEyXQEXEXQeXQeyx
    //solve    XYEqxEqxexeqxYeqx
    //xXxXxXxXxXxXxXxXxXxXxXxXxXxXxXxX

    #define GRAB_STOP_DELAY     1
    #define UNGRAB_START_DELAY  1
    #define STOP_GRAB_DELAY     1
    #define NEXT_MOVE_DELAY     1
    case 'x': fast90MotorsWOvershoot( X, X ); delay(GRAB_STOP_DELAY); ungrab(XY); delay(UNGRAB_START_DELAY); fast90Motors( X, 0 ); delay(STOP_GRAB_DELAY); grab(XY); delay(NEXT_MOVE_DELAY); break;
    case 'X': fast90MotorsWOvershoot( X, 0 ); delay(GRAB_STOP_DELAY); ungrab(XY); delay(UNGRAB_START_DELAY); fast90Motors( X, X ); delay(STOP_GRAB_DELAY); grab(XY); delay(NEXT_MOVE_DELAY); break;
    case 'y': fast90MotorsWOvershoot( Y, Y ); delay(GRAB_STOP_DELAY); ungrab(XY); delay(UNGRAB_START_DELAY); fast90Motors( Y, 0 ); delay(STOP_GRAB_DELAY); grab(XY); delay(NEXT_MOVE_DELAY); break;
    case 'Y': fast90MotorsWOvershoot( Y, 0 ); delay(GRAB_STOP_DELAY); ungrab(XY); delay(UNGRAB_START_DELAY); fast90Motors( Y, Y ); delay(STOP_GRAB_DELAY); grab(XY); delay(NEXT_MOVE_DELAY); break;
    case 'e': fast90MotorsWOvershoot( E, E ); delay(GRAB_STOP_DELAY); ungrab(EQ); delay(UNGRAB_START_DELAY); fast90Motors( E, 0 ); delay(STOP_GRAB_DELAY); grab(EQ); delay(NEXT_MOVE_DELAY); break;
    case 'E': fast90MotorsWOvershoot( E, 0 ); delay(GRAB_STOP_DELAY); ungrab(EQ); delay(UNGRAB_START_DELAY); fast90Motors( E, E ); delay(STOP_GRAB_DELAY); grab(EQ); delay(NEXT_MOVE_DELAY); break;
    case 'q': fast90MotorsWOvershoot( Q, Q ); delay(GRAB_STOP_DELAY); ungrab(EQ); delay(UNGRAB_START_DELAY); fast90Motors( Q, 0 ); delay(STOP_GRAB_DELAY); grab(EQ); delay(NEXT_MOVE_DELAY); break;
    case 'Q': fast90MotorsWOvershoot( Q, 0 ); delay(GRAB_STOP_DELAY); ungrab(EQ); delay(UNGRAB_START_DELAY); fast90Motors( Q, Q ); delay(STOP_GRAB_DELAY); grab(EQ); delay(NEXT_MOVE_DELAY); break;

    case 'a': ungrab(EQ); fast90Motors(X|Y,X); grab(EQ); ungrab(XY); fast90Motors(X|Y,Y); grab(XY); break;
    case 'd': ungrab(EQ); fast90Motors(X|Y,Y); grab(EQ); ungrab(XY); fast90Motors(X|Y,X); grab(XY); break;
    case 's': ungrab(XY); fast90Motors(E|Q,E); grab(XY); ungrab(EQ); fast90Motors(E|Q,Q); grab(EQ); break;
    case 'w': ungrab(XY); fast90Motors(E|Q,Q); grab(XY); ungrab(EQ); fast90Motors(E|Q,E); grab(EQ); break;

    case '5': motorsOn( X ); Serial.println( "motorsOn( X )" ); break;
    case '6': motorsOn( Y ); Serial.println( "motorsOn( Y )" ); break;
    case '7': motorsOn( E ); Serial.println( "motorsOn( E )" ); break;
    case '8': motorsOn( Q ); Serial.println( "motorsOn( Q )" ); break;

    case '1': speed_scale = (1<<0); Serial.print( "Speed scale: "); Serial.println( speed_scale ); break;
    case '2': speed_scale = (1<<1); Serial.print( "Speed scale: "); Serial.println( speed_scale ); break;
    case '3': speed_scale = (1<<2); Serial.print( "Speed scale: "); Serial.println( speed_scale ); break;
    case '4': speed_scale = (1<<4); Serial.print( "Speed scale: "); Serial.println( speed_scale ); break;
    }
}




