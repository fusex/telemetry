void setup()
{
    Serial.begin(115200);
    while (!Serial) ; // Wait for DEBUG serial port to be available

    Serial1.begin(9600);
    while (!Serial1) {
        Serial.println("init failed ! Retrying !");
        delay(2000);
    }
    Serial.println("Rocket GPS test ");
}

void loop()
{
    int c = 0;
    int f = 0;

    while (f == 0) {
        while (Serial1.available()) {
            Serial.write(Serial1.read());
            //if (++c>1000) f = 1;
        }
    }
}
