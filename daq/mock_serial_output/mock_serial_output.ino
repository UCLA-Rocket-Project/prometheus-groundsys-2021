void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

float pt0_data = 0.00;
float pt1_data = 1.11;
float lc_data = -2.37;
float checksum = pt0_data + pt1_data + lc_data;

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(millis());
  Serial.print(',');
  Serial.print(pt0_data);
  Serial.print(',');
  Serial.print(pt1_data);
  Serial.print(',');
  Serial.print(lc_data);
  Serial.print(',');
  Serial.println(checksum);
}
