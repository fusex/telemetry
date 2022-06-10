void setup() {
    Serial.begin(115200);
    Serial.println("Starting default Serial");
    delay(1000);
    Serial2.begin(115200);
    Serial2.println("Starting Serial2 through hc-06");
}

void loop() {
    if (Serial2.available() > 0) {
        // read the incoming byte:
        int incomingByte = Serial2.read();

        // say what you got:
        Serial.print("I received: ");
        Serial.println(incomingByte, DEC);
    }
    if (Serial.available() > 0) {
        // read the incoming byte:
        int incomingByte = Serial.read();

        // say what you got:
        Serial2.print("I received: ");
        Serial2.println(incomingByte, DEC);
    }
}
