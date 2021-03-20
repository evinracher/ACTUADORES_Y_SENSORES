#define light_sensor A0

// --- LIGHT SENSOR ---
const double k = 5.0 / 1024;
const double luxFactor = 500000;
const double R2 = 10000;
const double LowLightLimit = 200;
const double B = 1.3 * pow(10.0, 7);
const double m = -1.4;

double getLightIntensity () {
  double V2 = k * analogRead(light_sensor);
  double R1 = (5.0 / V2 - 1) * R2;
  double lux = B * pow(R1, m);
  return lux;
}
// --- END LIGHT SENSOR ---
int numOfValues = 5;

double lightValuesSum = 0;

double fLightValue = 0;

void getData() {
  for (int i = 0; i < numOfValues; i++) {
    lightValuesSum += getLightIntensity();
    delay(500);
  }
  fLightValue = lightValuesSum / numOfValues;
  lightValuesSum = 0;
}

void setup() {
  pinMode(light_sensor, INPUT);
  Serial.begin(9600);
}

void loop() {
  getData();
  // TODO: send data
  Serial.print(fLightValue);
  Serial.println(" lux");
}
