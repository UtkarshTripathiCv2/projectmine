#define TRIG_PIN 9
#define ECHO_PIN 10

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // Transmit a 10-microsecond pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the bounce-back time
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Convert time to distance (cm)
  int distance = duration * 0.034 / 2;

  // Print result
  Serial.print(distance);
  Serial.println(" cm");

  delay(200); // Simple delay for stability
}
