#include <SimpleTimer.h>

// the timer object
SimpleTimer timer;

int myNum = 5;          //Number of times to call the repeatMe function
int myInterval = 1000;  //time between funciton calls in millis

// a function to be executed periodically
void repeatMe() {
    Serial.print("Uptime (s): ");
    Serial.println(millis() / 1000);
}

void setup() {
    Serial.begin(115200);
    timer.setTimer(myInterval, repeatMe, timer.RUN_FOREVER);
}

void loop() {
    timer.run();
}
