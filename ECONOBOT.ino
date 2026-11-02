/*
ECONO BOT - Tecnologia Primaria

*/


/* ---------------------------
CONFIGURACION DE PINES DE MOTORES 
--------------------------- */
#define IN1_R 2   //Motor derecho
#define IN2_R 3
#define IN3_R 4
#define IN4_R 5
#define IN1_L 6   //Motor izquierdo 
#define IN2_L 7
#define IN3_L 8
#define IN4_L 9

/* ---------------------------
CONFIGURACION DE PINES DEL SENSOR Y DEL BUZZER
--------------------------- */

#define TRIG_PIN 12
#define ECHO_PIN 13

#define BUZZER_PIN 10

long distance = 0;

/* ---------------------------
PARAMETROS DE AVANCE Y GIRO 
--------------------------- */
#define STEPS_LINE 869  // Pasos para avanzar un cuadro 
#define STEPS_TURN 945  // Pasos para realizar giro de 90°

#define STEPPER_STEPS_PER_REV 2048   // típico 28BYJ-48 - Pasos para un giro completo 



/*************************************************
NOTAS MUSICALES 
*************************************************/
#define NOTE_SILENCE 0
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

/* ---------------------------
   FUNCIÓN: RPM → microsegundos por paso
   --------------------------- */
uint32_t rpmToDelayUs(float rpm) {
  if (rpm <= 0) rpm = 1;
  float stepsPerSecond = (rpm * STEPPER_STEPS_PER_REV) / 60.0;
  return (uint32_t)(1000000.0 / stepsPerSecond);
}

/* ---------------------------
   SECUENCIA FULL-STEP - 
   --------------------------- */
const uint8_t step_pattern[4][4] = {
  {1,1,0,0},
  {0,1,1,0},
  {0,0,1,1},
  {1,0,0,1}
};
const uint8_t numSteps = 4;

/* ---------------------------
   BAJO NIVEL
   --------------------------- */
void motorStepLeft(uint8_t p) {
  digitalWrite(IN1_L, step_pattern[p][0]);
  digitalWrite(IN2_L, step_pattern[p][1]);
  digitalWrite(IN3_L, step_pattern[p][2]);
  digitalWrite(IN4_L, step_pattern[p][3]);
}

void motorStepRight(uint8_t p) {
  digitalWrite(IN1_R, step_pattern[p][0]);
  digitalWrite(IN2_R, step_pattern[p][1]);
  digitalWrite(IN3_R, step_pattern[p][2]);
  digitalWrite(IN4_R, step_pattern[p][3]);
}

void apagarMotores() {
  digitalWrite(IN1_L, LOW); digitalWrite(IN2_L, LOW);
  digitalWrite(IN3_L, LOW); digitalWrite(IN4_L, LOW);
  digitalWrite(IN1_R, LOW); digitalWrite(IN2_R, LOW);
  digitalWrite(IN3_R, LOW); digitalWrite(IN4_R, LOW);
}

/* ---------------------------
   MOVIMIENTO MEJORADO
   --------------------------- */

void turnRight(int grados, float rpm) {
  if (grados <= 0) return;

  uint32_t delayUs = rpmToDelayUs(rpm);
  int32_t pasos = (int32_t)STEPS_TURN * grados / 90;

  uint8_t s = 0;
  for (int32_t i = 0; i < pasos; i++) {
    if (s >= numSteps) s = 0;
    motorStepLeft(s);
    motorStepRight(s);
    delayMicroseconds(delayUs);
    s++;
  }
  apagarMotores();
}

void turnLeft(int grados, float rpm) {
  if (grados <= 0) return;

  uint32_t delayUs = rpmToDelayUs(rpm);
  int32_t pasos = (int32_t)STEPS_TURN * grados / 90;

  int8_t s = numSteps - 1;
  for (int32_t i = 0; i < pasos; i++) {
    if (s < 0) s = numSteps - 1;
    motorStepLeft(s);
    motorStepRight(s);
    delayMicroseconds(delayUs);
    s--;
  }
  apagarMotores();
}

void moveForward(int cuadros, float rpm) {
  if (cuadros <= 0) return;

  uint32_t delayUs = rpmToDelayUs(rpm);

  for (int c = 0; c < cuadros; c++) {
    for (int32_t i = 0; i < STEPS_LINE; i++) {
      uint8_t a = i % numSteps;
      motorStepLeft(a);
      motorStepRight(numSteps - 1 - a);
      delayMicroseconds(delayUs);
    }
  }
  apagarMotores();
}

void moveBackward(int cuadros, float rpm) {
  if (cuadros <= 0) return;

  uint32_t delayUs = rpmToDelayUs(rpm);

  for (int c = 0; c < cuadros; c++) {
    for (int32_t i = 0; i < STEPS_LINE; i++) {
      uint8_t a = i % numSteps;
      motorStepLeft(numSteps - 1 - a);
      motorStepRight(a);
      delayMicroseconds(delayUs);
    }
  }
  apagarMotores();
}

/* ---------------------------
   ALIAS EN ESPAÑOL
   --------------------------- */
void girarDerecha(int grados, float rpm)  { turnRight(grados, rpm); }
void girarIzquierda(int grados, float rpm){ turnLeft(grados, rpm); }
void avanzar(int cuadros, float rpm)      { moveForward(cuadros, rpm); }
void retroceder(int cuadros, float rpm)   { moveBackward(cuadros, rpm); }


/*************************************************
 *  ULTRASONIDO
 *************************************************/
long getDistance(){
  long duration;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(4);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  long dist = duration / (29.2 * 2);
  if(dist == 0) dist = 500;

  return dist;
}

/*************************************************
 *  MELODIAS ROBOTS STAR WARS (cortas)
 *************************************************/

#define NOTE_SILENCE 0

/* ---- REPRODUCTOR GENERAL ---- */
void playMelody(const int *notes, const int *dur, int len){
  for (int i = 0; i < len; i++){
    if(notes[i] == NOTE_SILENCE){
      noTone(BUZZER_PIN);
    } else {
      tone(BUZZER_PIN, notes[i], dur[i]);
    }
    delay(dur[i] + 20);
  }
  noTone(BUZZER_PIN);
}

/*************************************************
 *  R2-D2 – HAPPY (versión corta)
 *************************************************/
const int r2d2_happy_notes[] = {
  NOTE_E6, NOTE_G6, NOTE_B6, NOTE_E7
};
const int r2d2_happy_dur[] = {
  80, 70, 80, 120
};

/*************************************************
 *  R2-D2 – ALARM (corta)
 *************************************************/
const int r2d2_alarm_notes[] = {
  NOTE_A6, NOTE_G6, NOTE_FS6, NOTE_E6
};
const int r2d2_alarm_dur[] = {
  120,120,120,150
};

/*************************************************
 *  R2-D2 – RANDOM BEEPS (muy corto)
 *************************************************/
const int r2d2_random_notes[] = {
  NOTE_A6, NOTE_E7, NOTE_G6, NOTE_B6, NOTE_E6
};
const int r2d2_random_dur[] = {
  70, 70, 70, 70, 100
};

/*************************************************
 *  BB-8 – BEEPS (corto)
 *************************************************/
const int bb8_notes[] = {
  NOTE_G5, NOTE_A5, NOTE_E6, NOTE_C6
};
const int bb8_dur[] = {
  90, 70, 90, 120
};

/*************************************************
 *  DROIDE IMPERIAL – BEEP (corto)
 *************************************************/
const int imperial_bot_notes[] = {
  NOTE_FS4, NOTE_A4, NOTE_CS5
};
const int imperial_bot_dur[] = {
  120,120,180
};

/*************************************************
 *  DROID ALARM – MELÓDIA 6 (corta)
 *************************************************/
const int droid_alarm_notes[] = {
  NOTE_E7, NOTE_SILENCE, NOTE_E7,
  NOTE_SILENCE, NOTE_G6, NOTE_G6
};

const int droid_alarm_dur[] = {
  150, 60, 150,
  60, 180, 200
};

/*************************************************
 *  SELECTOR
 *************************************************/
void melody(int m){
  switch(m){
    case 1: playMelody(r2d2_happy_notes, r2d2_happy_dur, sizeof(r2d2_happy_notes)/2); break;
    case 2: playMelody(r2d2_alarm_notes, r2d2_alarm_dur, sizeof(r2d2_alarm_notes)/2); break;
    case 3: playMelody(r2d2_random_notes, r2d2_random_dur, sizeof(r2d2_random_notes)/2); break;
    case 4: playMelody(bb8_notes, bb8_dur, sizeof(bb8_notes)/2); break;
    case 5: playMelody(imperial_bot_notes, imperial_bot_dur, sizeof(imperial_bot_notes)/2); break;
    case 6: playMelody(droid_alarm_notes, droid_alarm_dur, sizeof(droid_alarm_notes)/2); break;
  }
}

/* ---------------------------
   SETUP
   --------------------------- */
void setup() {
  pinMode(IN1_L, OUTPUT);
  pinMode(IN2_L, OUTPUT);
  pinMode(IN3_L, OUTPUT);
  pinMode(IN4_L, OUTPUT);
  pinMode(IN1_R, OUTPUT);
  pinMode(IN2_R, OUTPUT);
  pinMode(IN3_R, OUTPUT);
  pinMode(IN4_R, OUTPUT);

  pinMode(TRIG_PIN,OUTPUT);
  pinMode(ECHO_PIN,INPUT);
  pinMode(BUZZER_PIN,OUTPUT);

  apagarMotores();
  Serial.begin(9600);
}

/* ---------------------------
   LOOP
   --------------------------- */
void loop() {

  distance = getDistance();
  Serial.println(distance);

  if(distance < 20){
    melody(6);              // sonido Pac-Man
    retroceder(1,17);
    melody(5);              // muerte Pac-Man
    girarDerecha(90,15);
  }
}
