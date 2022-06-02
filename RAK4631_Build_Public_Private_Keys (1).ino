#include <Adafruit_TinyUSB.h> // for Serial
#include "Adafruit_nRFCrypto.h"
#include "nRF52840_DHKE.h"

void hexDump(char *buf, int len) {
  char alphabet[17] = "0123456789abcdef";
  printf("%s\n", "   +------------------------------------------------+ +----------------+");
  printf("%s\n", "   |.0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .a .b .c .d .e .f | |      ASCII     |");
  for (int i = 0; i < len; i += 16) {
    if (i % 128 == 0)
      printf("%s\n", "   +------------------------------------------------+ +----------------+");
    char s[] = "|                                                | |................|";
    char ix = 1, iy = 52;
    for (char j = 0; j < 16; j++) {
      if (i + j < len) {
        char c = buf[i + j];
        s[ix++] = alphabet[(c >> 4) & 0x0F];
        s[ix++] = alphabet[c & 0x0F];
        ix++;
        if ((int)c > 31 && (int)c < 128) s[iy++] = c;
        else s[iy++] = '.';
      }
    }
    char index = i / 16;
    if (i < 256) printf("%s", " ");
    printf("%x.", index);
    printf("%s\n", s);
  }
  printf("%s\n", "   +------------------------------------------------+ +----------------+");
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  while (!Serial) delay(10);
  delay(1000);
  nRFCrypto.begin();
  nRFCrypto_AES aes;
  if (!aes.begin()) {
    Serial.println("Could not start AES!");
    return;
  }
  Serial.println("{");
  uint8_t ix, jx, rawUUID[4], rawPubKey[16];
  char myUUID[9], myPubKey[65];
  char alphabet[17] = "0123456789abcdef";
  String pvt;
  for (ix = 0; ix < 4; ix++) {
    nrf52_DHKE_Set Alice;
    if (!Alice.begin()) {
      Serial.println("Error initing Alice!");
      return;
    }
    nRFCrypto.Random.generate(rawUUID, 4);
    uint8_t x = 0;
    for (jx = 0; jx < 4; jx++) {
      char c = rawUUID[jx];
      myUUID[x++] = alphabet[(c >> 4) & 0x0F];
      myUUID[x++] = alphabet[c & 0x0F];
    } myUUID[x] = 0;
    Serial.print("\"");
    Serial.print(myUUID);
    Serial.print("\":\"");
    memcpy(rawPubKey, Alice.getPub().oneChunk, 16);
    x = 0;
    for (jx = 0; jx < 16; jx++) {
      char c = rawPubKey[jx];
      myPubKey[x++] = alphabet[(c >> 4) & 0x0F];
      myPubKey[x++] = alphabet[c & 0x0F];
    } myPubKey[x] = 0;
    Serial.print(myPubKey);
    if (ix < 3) Serial.println("\",");
    else Serial.println("\"");
    memcpy(rawPubKey, Alice.getPvt().oneChunk, 16);
    x = 0;
    for (jx = 0; jx < 16; jx++) {
      char c = rawPubKey[jx];
      myPubKey[x++] = alphabet[(c >> 4) & 0x0F];
      myPubKey[x++] = alphabet[c & 0x0F];
    } myPubKey[x] = '\0';
    pvt = pvt + String(myUUID) + "\t" + String(myPubKey) + "\n";
  }
  Serial.println("}");
  Serial.println("Private Keys:");
  Serial.println(pvt);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
}
