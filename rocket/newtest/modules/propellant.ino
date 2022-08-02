#define BGC_SOUND_SENSOR   A13

const static int thresholdvalue=490; //The threshold

void InitADC()
{
    // Select Vref=AVcc
    ADMUX |= (1<<REFS1)| (1<<REFS0);
    //set prescaller to 128 and enable ADC 
    ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0)|(1<<ADEN);    
}

uint16_t ReadADC(uint8_t ADCchannel)
{
    //select ADC channel with safety mask
   //ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F);
   ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((ADCchannel >> 3) & 0x01) << MUX5);
   ADMUX |= (ADCchannel & 0x07);

    //single conversion mode
    ADCSRA |= (1<<ADSC);
    // wait until ADC conversion is complete
    while( ADCSRA & (1<<ADSC) );

    uint8_t low  = ADCL;
    uint8_t high = ADCH;

   return (high << 8) | low;
}

void setup()
{
    Serial.begin(115200);
    while (!Serial) ; // Wait for DEBUG serial port to be available
    Serial.println("Rocket sound test ");
    analogReference(INTERNAL2V56);
    pinMode (BGC_SOUND_SENSOR, OUTPUT);
    analogWrite(BGC_SOUND_SENSOR, 150);
    pinMode (A14, OUTPUT);
    analogWrite(A14, 255);
    //digitalWrite(A13,LOW);
    //analogReference(EXTERNAL);
    //InitADC();
    //while (1);
}

void loop()
{
    pinMode (A14, INPUT);
    pinMode (BGC_SOUND_SENSOR, INPUT);
    //digitalWrite(A13,LOW);
    //analogReference(DEFAULT);
    //pinMode (13, INPUT);
    //int sensorValue = ReadADC(BGC_SOUND_SENSOR);

    int sensorValue = analogRead(BGC_SOUND_SENSOR);
    Serial.print("Propellant detect:");Serial.println(sensorValue);
    delay(5000);

    sensorValue = analogRead(A14);
    Serial.print("Propellant detect:");Serial.println(sensorValue);
    delay(5000);

    //pinMode (A13, OUTPUT);
    //digitalWrite(A13,HIGH);
    //pinMode (A13, INPUT);
    //sensorValue = analogRead(BGC_SOUND_SENSOR);
    //Serial.print("Propellant detect:");Serial.println(sensorValue);
    //delay(500);
}
