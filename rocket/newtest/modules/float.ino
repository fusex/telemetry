#include <Arduino.h>
#include <stdfix.h>
#include <stdint.h>

void setup()
{
    Serial.begin(115200);
    while (!Serial) ; // Wait for DEBUG serial port to be available
    Serial.println("Rocket float serializer");

    float f1 = 1.1111;
#if 1
    short _Accum s1 = 2.2222;

    char* c1 = (char*) &s1; 
    char* c2 = c1 + 1;

    Serial.println("short storage: ");
    Serial.println(*c1, HEX);
    Serial.println(*c2, HEX);
#endif

    char* c1 = (char*) &f1; 
    char* c2 = c1 + 1;
    char* c3 = c2 + 1;
    char* c4 = c3 + 1;

    Serial.println("float sizeof: ");
    Serial.println(sizeof(float));
    Serial.println("float storage: ");
    Serial.println(*c1, HEX);
    Serial.println(*c2, HEX);
    Serial.println(*c3, HEX);
    Serial.println(*c4, HEX);
}

void loop()
{}

