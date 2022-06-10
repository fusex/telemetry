#include <SPI.h>
#include <BGC_Lora.h>

#if 1
BGC_Lora lora;           // BGC Sens board.
#else
BGC_Lora lora(10, true); //Lora shield on DUE.
#endif

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  if (!lora.init()) {
    Serial.println("init failed");
    while(1) {
      delay(100000);
    }
  }
  Serial.println("Lora Init OK");
}

void loop()
{
  uint8_t data[] = "Hello World!";
  Serial.println("Sending on BGC Lora");
  lora.send(data, sizeof(data));
           
  delay(400);
}
