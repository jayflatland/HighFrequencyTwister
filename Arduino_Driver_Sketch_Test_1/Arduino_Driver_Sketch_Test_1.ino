/*
Testing...
Test commands
U R R' U'

U2 U2 U2 U2 U2 U2 U2 

Scramble (trimmed to under 64 chars)
R2 B U' F L' F' R' D R2 U L2 D2 R D F D' R' B U B2 L' D B'

Reverse of prior scramble
B D' L B2 U' B' R D F' D' R' D2 L2 U' R2 D' R F L F' U B' R2

*/

enum Side { U, D, L, R, F, B };
enum Dir { CW, CCW, CW2 };

int stepDelay = 2000;
int stepCntFwd = 96+10;
int stepCntBack = 10;

char b;
char b2;
int pos;

void turn( int s, int d ) {
  if( d == CW2 ) {
    turn( s, CW );
    turn( s, CW );
    return;
  }

  int dirpin = (int)s + 8;
  int steppin = (int)s + 2;
  switch( d ) {
  case CW:   digitalWrite(dirpin, HIGH); break;
  case CCW:  digitalWrite(dirpin, LOW ); break;
  default: return;
  }

  for( int i = 0; i < stepCntFwd;  i++ ) { digitalWrite(steppin, LOW); digitalWrite(steppin, HIGH); delayMicroseconds(stepDelay); }

  switch( d ) {
  case CW:   digitalWrite(dirpin, LOW ); break;
  case CCW:  digitalWrite(dirpin, HIGH); break;
  default: return;
  }
  //overshoot and return
  for( int i = 0; i < stepCntBack; i++ ) { digitalWrite(steppin, LOW); digitalWrite(steppin, HIGH); delayMicroseconds(stepDelay); }
}


void setup() 
{
  Serial.begin(115200);
  for( int i = 2; i <= 13; i++ ) {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
  b = 0;
  b2 = 0;
  pos = 0;
}


void loop()
{
  int slice;
  int dir;
  int valid;
  char c;
  c = Serial.read();
  if( c < 0 ) {
    return;
  }

  if( c == ' ' || c =='\n' ) {
    slice = U;
    dir = CW;
    valid = 1;
    switch( b ) {
      case 'U': slice = U; break;
      case 'D': slice = D; break;
      case 'L': slice = L; break;
      case 'R': slice = R; break;
      case 'F': slice = F; break;
      case 'B': slice = B; break;
      default:  valid = 0; break;
    }
    switch( b2 ) {
      case 0:     dir = CW;  break;
      case '2':   dir = CW2; break;
      case '\'':  dir = CCW; break;
      default:  valid = 0; break;
    }

    if( valid ) {
      turn( slice, dir );
      Serial.println( "OK" );
    }
    else {
      Serial.println( "INVALID" );
    }

    b = 0;
    b2 = 0;
    pos = 0;
  }
  else if( c >= 0 ) {
    if( pos == 0 ) {
      b = c;
      pos = 1;
    }
    else {
      b2 = c;
    }
  }
}


