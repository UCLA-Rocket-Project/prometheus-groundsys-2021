void setup() {
  Serial.begin(9600);           //  setup serial
}

void loop() {  
  unsigned long timenow = millis();
  int pt1 = analogRead(A2);
  int pt2 = analogRead(A3);
  int pt3 = analogRead(A4);
  int pt4 = analogRead(A5);

  //unsigned long summ = timenow + pt1 + pt2 + pt3 + pt4;
  Serial.println(String(timenow) + "," + String(pt1) + "," + String(pt2) + "," + String(pt3) + "," + String(pt4) + ",0,0,0,0,0,0,0,0,0," + "-42069");
}
