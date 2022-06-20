void setup()
{
    Serial2.begin(115200);
    while (!Serial2) ; // Wait for DEBUG serial port to be available
    Serial1.begin(9600);

    while (!Serial1) {
        Serial2.println("init failed ! Retrying !");
        delay(2000);
    }
    Serial2.println("Rocket GPS test ");
}

void loop()
{
    int c = 0;
    int f = 0;

    while (f == 0) {
        while (Serial1.available()) {
            Serial2.write(Serial1.read());
            //if (++c>1000) f = 1;
        }
    }
}
