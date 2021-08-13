// for I2C Communication
#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <NfcAdapter.h>
#include <RCSwitch.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);
RCSwitch mySwitch = RCSwitch();

String tagId = "None";
byte nuidPICC[4];

void setup(void) {
  Serial.begin(9600);
  Serial.println("System initialized");

  pinMode(LED_BUILTIN, OUTPUT);
  
  nfc.begin();

  mySwitch.enableTransmit(12);
  mySwitch.setRepeatTransmit(5);
}

void loop() {
  if (nfc.tagPresent()) {
    NfcTag tag = nfc.read();
    //tag.print();
    tagId = tag.getUidString();
    Serial.println("Tag: " + tagId);

    mySwitch.send(555, 16);
    Serial.println("START");
    
    byte* tagUidBytes = tag.getUidBytes();
    uint8_t tagUidLength = tag.getUidLength();
    long uidInteger = 0;
    long factor = 1;
    for (int i = 0; i < tagUidLength; i++) {
        //factor = 1;
        //for (int j = 1; j < (tagUidLength - i); j++) {
        //  factor = factor * 256;
        //}
        //uidInteger = uidInteger + ((uint8_t)tagUidBytes[i]) * factor;
        mySwitch.send(553, 16);
        //delay(1);
        mySwitch.send((uint8_t)tagUidBytes[i], 8);
        //Serial.println((uint8_t)tagUidBytes[i]);
        //delay(1);
        //mySwitch.send(554, 16);
        
        //delay(1);
    }

    digitalWrite(LED_BUILTIN, HIGH);

    //mySwitch.send(uidInteger, 8 * sizeof(uidInteger));
    //Serial.println(uidInteger);

    mySwitch.send(556, 16);
    Serial.println("END");

    digitalWrite(LED_BUILTIN, LOW);
  }
  delay(1000);
}
