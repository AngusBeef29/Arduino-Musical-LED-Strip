/* Geoffrey Angus
 * LED logic built on top of David Wang's
 * frequency parser. Separates frequencies
 * into high, medium, and low "buckets."
 * The bucket with the highest amplitude
 * has its color displayed. In its current form,
 * it works primarily on electronic music. Enjoy!
 */

/* THIS SECTION IS NOT MY CODE*/
/* David Wang
 * Code that takes audio input from a 3.5mm cable
 * and flashes an LED strip based on the frequency
 * of the music.
 *
 * HUGE thanks to the arduino community
 * If you see your code here, I owe you my gratitude
 *
 */
 
 #include <math.h>

int analogPin = 0; // MSGEQ7 OUT
int strobePin = 2; // MSGEQ7 STROBE
int resetPin = 4; // MSGEQ7 RESET
int spectrumValue[7];

// MSGEQ7 OUT pin produces values around 50-80
// when there is no input, so use this value to
// filter out a lot of the chaff.
int filterValue = 80;

// LED pins connected to the PWM pins on the Arduino

int ledPinR = 9;
int ledPinG = 10;
int ledPinB = 11;

//int timeOn;
int count = 0;

int volume;
int scale;
int r;
int g;
int b;

int treble;
int mid;
int bass;

int trebCount;
int midCount;
int bassCount;

int avgVolume;

boolean trebOn;
boolean midOn;
boolean bassOn;

void setup()
{
  Serial.begin(9600);
  // Read from MSGEQ7 OUT
  pinMode(analogPin, INPUT);
  // Write to MSGEQ7 STROBE and RESET
  pinMode(strobePin, OUTPUT);
  pinMode(resetPin, OUTPUT);

  // Set analogPin's reference voltage
  analogReference(DEFAULT); // 5V

  // Set startup values for pins
  digitalWrite(resetPin, LOW);
  digitalWrite(strobePin, HIGH);
}

void loop()
{
  // Set reset pin low to enable strobe
  digitalWrite(resetPin, HIGH);
  digitalWrite(resetPin, LOW);

  // Get all 7 spectrum values from the MSGEQ7
  for (int i = 0; i < 7; i++)
  {
    digitalWrite(strobePin, LOW);
    delayMicroseconds(40); // Allow output to settle

    spectrumValue[i] = analogRead(analogPin);

    // Constrain any value above 1023 or below filterValue
    spectrumValue[i] = constrain(spectrumValue[i], filterValue, 1023);


    // Remap the value to a number between 0 and 255
    spectrumValue[i] = map(spectrumValue[i], filterValue, 1023, 0, 255);

    // Remove serial stuff after debugging
    Serial.print(spectrumValue[i]);
    Serial.print(" ");
    digitalWrite(strobePin, HIGH);
  }
  
  Serial.println();

  if (count % 20 == 0) {
    count = 0;
    volume = totalAmplitude(spectrumValue);
    scale = volume / 7; // the average amplitude across all 7 bins
  }

  if (trebOn && trebCount < 1) {
    trebCount++;
  }
  else {
    trebOn = false;
    trebCount = 0;
  }

  if (midOn && midCount < 1) {
    midCount++;
  }
  else {
    midOn = false;
    midCount = 0;
  }
  
  if (bassOn && bassCount < 1) {
    bassCount++;
  }
  else {
    bassOn = false;
    bassCount = 0;
  }

  bass = spectrumValue[0];
  mid = spectrumValue[4] + spectrumValue[3];
  treble = spectrumValue[6] + spectrumValue[5];

  if (bass < 20 && spectrumValue[0] < 50 && spectrumValue[2] < 30 && !bassOn && !midOn && !trebOn) { //white
    r = 0;
    g = 0;
    b = 0;
  }
  else if (bass > 205 || bassOn) { // purple
    r = 240; //+ bass / 2;
    g = 0;
    b = 240; //+ bass / 2;
    bassOn = true;
  }
  else {
    r = 0; //+ treble / 4;
    g = 80; //+ treble / 4;
    b = 24; //+ treble / 4;
    trebOn = true;
  }

  // Write the PWM values to the LEDs
  // I find that with three LEDs, these three spectrum values work the best
  analogWrite(ledPinR, r);
  analogWrite(ledPinG, g);
  analogWrite(ledPinB, b);

  count++;
}

int totalAmplitude(int spectrumValue[]) {
  int total = 0;
  for (int i = 0; i < 7; i++) {
    total += spectrumValue[i];
  }
  return total;
}
