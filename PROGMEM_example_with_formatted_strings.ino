
const char signalString[] PROGMEM = "Serial started";
const char howdyString[] PROGMEM = "Howdy there, %s!\n";
const char luckyString[] PROGMEM = "Your lucky number today is %d. Use it wisely.\n";
char buf[50];

void setup() {
  // the entire sketch runs in the setup() procedure

  // disconnect the pullup from pin 0
  // we need it to float as a source of randomness
  pinMode(0, OUTPUT);
  Serial.begin(115200);
  // allow time for user to open Serial Monitor
  delay(10000);
  // initialize the random number generator
  randomSeed(analogRead(0));
  // announce serial has started
  Serial.println();
  // copy the string into the buffer
  strcpy_P(buf, signalString);
  Serial.println(buf);
  // copy a formatted string into the buffer
  strcpy_P(buf, howdyString);
  // print it with parameter substitution
  Serial.printf(buf, "Dave");
  // do that again, this time for a number
  strcpy_P(buf, luckyString);
  Serial.printf(buf, random(100));

}

void loop() {/* nothing happening here */}
