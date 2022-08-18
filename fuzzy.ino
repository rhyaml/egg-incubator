// paramameter suhu
float FiN() {
  if (suhu <= 35) {
    return 1;
  }
  else if (suhu >= 35 && suhu <= 37) {
    return (37 - suhu) / (2);
  }
  else if (suhu >= 37) {
    return 0;
  }
}
float FiZ() {
  if (suhu <= 35) {
    return 0;
  }
  else if (suhu >= 35 && suhu <= 37) {
    return (suhu - 35) / (2);
  }
  else if (suhu >= 40 && suhu <= 42) {
    return (42 - suhu) / (2);
  }
  else if (suhu >= 37 && suhu <= 40) {
    return 1;
  }
}
float FiP() {
  if (suhu <= 40 ) {
    return 0;
  }
  else if (suhu >= 40 && suhu <= 42) {
    return (suhu - 40) / (2);
  }
  else if (suhu >= 42) {
    return 1;
  }
}


void implikasi () {
  //sesuai dengan rule
  // if dingin then lambat
  a1 = 85 - (FiN() * (43));
  // if hangat then sedang
  b1a = 42 + (FiZ() * (43));
  b1b = 170 - (FiZ() * (43));
  // if panas then cepat
  c1 = 127 + (FiP() * (43));
}

void luas_deffuzzy() {
  implikasi ();
  A1 = ((85 - a1) * FiN()) / 2;
  A2 = (a1 - 43) * FiN();
  A3 = ((b1a - 42) * FiZ()) / 2;
  A4 = ((170 - b1b) * FiZ()) / 2;
  A5 = (b1b - b1a) * FiZ();
  A6 = ((c1 - 42) * FiP()) / 2;
  A7 = (127 - c1) * FiP();
}

float f(float x) {
  if (B > 0 && sel_ == 0) {
    return ((x - A) / B) * x;
  }
  else if (B > 0 && sel_ == 1) {
    return ((A - x) / B) * x;
  }
  else {
    return A * x;
  }
}
/*Function definition to perform integration by Simpson's 1/3rd Rule */
float simpsons(float f(float x), float a, float b, float n) {
  float h, integral, x, sum = 0;
  int i;
  h = fabs(b - a) / n;
  for (i = 1; i < n; i++) {
    x = a + i * h;
    if (i % 2 == 0) {
      sum = sum + 2 * f(x);
    }
    else {
      sum = sum + 4 * f(x);
    }
  }
  integral = (h / 3) * (f(a) + f(b) + sum);
  return integral;
}
float fx(float limd, float limu, float a, float b, int sel) {
  int n, i = 2;
  float h, x, integral, eps = 0.1, integral_new;
  A = a;
  B = b;
  sel_ = sel;
  integral_new = simpsons(f, limd, limu, i);
  do {
    integral = integral_new;
    i = i + 2;
    integral_new = simpsons(f, limd, limu, i);
  } while (fabs(integral_new - integral) >= eps);
  /*Print the answer */
  return integral_new;
}

void moment() {
  luas_deffuzzy();
  //M1 = ∫ ((85-x)/43)x dx ==================> limd a1 dan limup 100
  M1 = fx(a1, 85, 85, (43), 1);
  //M2 = ∫ 0.555556x dx ==================> limd 0 dan limup a1
  M2 = fx(42, a1, FiN(), 0, 0);
  //M3 = ∫ ((x-42)/43)x dx ==================> limd 20 dan limup b1a
  M3 = fx(42, b1a, 42, (43), 0);
  //M4 = ∫ ((170-x)/43)x dx ==================> limd b1b dan limup 235
  M4 = fx (b1b, 170, 170, (43), 1);
  //M5 = ∫ 0 dx ==================> limd b1a dan limup b1b
  M5 = fx (b1a, b1b, FiZ(), 0, 0);
  //M6 = ∫ ((x-127)/43)x dx ==================> limd 127  dan limup c1
  M6 = fx(127, c1, 127, (43), 0);
  //M7 = ∫ 0 dx ==================> limd c1 dan limu p170
  M7 = fx(c1, 170, FiP(), 0, 0);
}
float deffuzzyfikasi() {
  return (M1 + M2 + M3 + M4 + M5 + M6 + M7) / (A1 + A2 + A3 + A4 + A5 + A6 + A7);
}

void mamdaniDisplay() {
  Serial.print("DHT Temperature : ");
  Serial.println(suhu);
  Serial.print("Keanggotaan Suhu Dingin : ");
  Serial.println(FiN());
  Serial.print("Keanggotaan Suhu Hangat : ");
  Serial.println(FiZ());
  Serial.print("Keanggotaan Suhu Panas : ");
  Serial.println(FiP());
  Serial.print("a1 : ");
  Serial.println(a1);
  Serial.print("b1a : ");
  Serial.println(b1a);
  Serial.print("b1b : ");
  Serial.println(b1b);
  Serial.print("C1 : ");
  Serial.println(c1);
  Serial.print("A1 : ");
  Serial.println(A1);
  Serial.print("A2 : ");
  Serial.println(A2);
  Serial.print("A3 : ");
  Serial.println(A3);
  Serial.print("A4 : ");
  Serial.println(A4);
  Serial.print("A5 : ");
  Serial.println(A5);
  Serial.print("A6 : ");
  Serial.println(A6);
  Serial.print("A7 : ");
  Serial.println(A7);
  Serial.print("M1 : ");
  Serial.println(M1);
  Serial.print("M2 : ");
  Serial.println(M2);
  Serial.print("M3 : ");
  Serial.println(M3);
  Serial.print("M4 : ");
  Serial.println(M4);
  Serial.print("M5: ");
  Serial.println(M5);
  Serial.print("M6 : ");
  Serial.println(M6);
  Serial.print("M7 : ");
  Serial.println(M7);
  pwmMamdani = int(deffuzzyfikasi());
  Serial.print("OutDefuzzyfikasi : ");
  Serial.print(pwmMamdani);
  Serial.println(" PWM");
  fanFixSpeed = map(pwmMamdani,0,255,500,1023);
  if(fanFixSpeed >= 500 && fanFixSpeed <= 700){
    fanStrSpeed = "Lambat";
  }else if(fanFixSpeed >= 701 && fanFixSpeed <= 900){
    fanStrSpeed = "Sedang";
  }else if(fanFixSpeed >= 901 && fanFixSpeed <= 1023){
    fanStrSpeed = "Cepat";
  }
  
}
