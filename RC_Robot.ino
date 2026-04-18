#include <Stepper.h>

// --- 1. STEP MOTOR TANIMLAMALARI ---
const int stepsPerRevolution = 200; 
// Pinler: IN1, IN3, IN2, IN4 (Senin verdiğin sıra: 8, 11, 10, 12)
Stepper myStepper(stepsPerRevolution, 8, 10, 11, 12);

// --- 2. DC MOTOR SÜRÜCÜ PİNLERİ (PWM DESTEKLİ) ---
int onSolIleri = 3;  int onSolGeri  = 5;
int onSagIleri = 6;  int onSagGeri  = 9;
// Arka motorlar için (Pin yetersizse önlere paralel bağlayabilirsin)
int arkaSolIleri = 2; 
int arkaSolGeri = 4;

// --- 3. JOYSTICK VE HIZ AYARLARI ---
const int joyXPin = A0;  
const int joyYPin = A1;  
const int center = 512;
const int deadzone = 100;

float mevcutHiz = 0;        
int hedefHizLimit = 191;    // %75 Maksimum Hız
float ivmeMiktari = 0.82;   // 3.5 Saniyede hızlanma/yavaşlama
int aktifYon = 0;
int sonYon = 1;

void setup() {
  // Pin modları
  pinMode(onSolIleri, OUTPUT); pinMode(onSolGeri, OUTPUT);
  pinMode(onSagIleri, OUTPUT); pinMode(onSagGeri, OUTPUT);
  pinMode(arkaSolIleri, OUTPUT); pinMode(arkaSolGeri, OUTPUT);
  
  // Step motor hızı (Full güç/hız hissi için 60-100 RPM idealdir)
  myStepper.setSpeed(100); 

  Serial.begin(9600);
}

void loop() {
  // --- ADIM 1: STEP MOTORU DÖNDÜR ---
  // Her döngüde sadece 1 adım atar, böylece kod takılmaz.
  // Sürekli dönmesi için yönü hep pozitif (1) tutuyoruz.
  myStepper.step(1);

  // --- ADIM 2: JOYSTICK OKUMA ---
  int xVal = analogRead(joyXPin);
  int yVal = analogRead(joyYPin);

  // Yön tespiti
  if (yVal > (center + deadzone))      { aktifYon = 1; sonYon = 1; } 
  else if (yVal < (center - deadzone)) { aktifYon = 2; sonYon = 2; } 
  else if (xVal > (center + deadzone)) { aktifYon = 3; sonYon = 3; } 
  else if (xVal < (center - deadzone)) { aktifYon = 4; sonYon = 4; } 
  else                                 { aktifYon = 0; }

  // --- ADIM 3: IVMELİ SÜRÜŞ HESABI ---
  if (aktifYon != 0) {
    if (mevcutHiz < hedefHizLimit) mevcutHiz += ivmeMiktari;
    if (mevcutHiz > hedefHizLimit) mevcutHiz = hedefHizLimit;
  } else {
    if (mevcutHiz > 0) mevcutHiz -= ivmeMiktari;
    if (mevcutHiz < 0) mevcutHiz = 0;
  }

  // --- ADIM 4: MOTORLARI SÜR ---
  if (mevcutHiz > 0) {
    int kullanilacakYon = (aktifYon != 0) ? aktifYon : sonYon;
    motoruGorevlendir(kullanilacakYon, (int)mevcutHiz);
  } else {
    stopAll();
  }

  // Delay süresini 15'ten 5'e düşürdüm çünkü stepper.step(1) de 
  // kendi içinde çok küçük bir gecikme yaratır. Toplamda akıcılık bozulmaz.
  delay(5); 
}

void motoruGorevlendir(int yon, int hiz) {
  switch (yon) {
    case 1: hareket(hiz, 0, hiz, 0); break; // İleri
    case 2: hareket(0, hiz, 0, hiz); break; // Geri
    case 3: hareket(hiz, 0, 0, hiz); break; // Sağ
    case 4: hareket(0, hiz, hiz, 0); break; // Sol
  }
}

void hareket(int sI, int sG, int rI, int rG) {
  analogWrite(onSolIleri, sI);   analogWrite(onSolGeri, sG);
  analogWrite(onSagIleri, rI);   analogWrite(onSagGeri, rG);
  // Arka motorlar (PWM pinin yetmiyorsa digitalWrite ile tam güç verilir)
  digitalWrite(arkaSolIleri, sI > 0 ? HIGH : LOW);
  digitalWrite(arkaSolGeri, sG > 0 ? HIGH : LOW);
}

void stopAll() {
  analogWrite(onSolIleri, 0); analogWrite(onSolGeri, 0);
  analogWrite(onSagIleri, 0); analogWrite(onSagGeri, 0);
  digitalWrite(arkaSolIleri, LOW); digitalWrite(arkaSolGeri, LOW);
}