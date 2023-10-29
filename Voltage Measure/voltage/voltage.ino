void setup() {
  // Set the reference voltage to DEFAULT (5V), which is used as the reference for analog readings.
  ADMUX = 0; 

  // Enable the ADC and set the prescaler to 128, which gives an ADC clock of 16MHz / 128 = 125kHz (max 200kHz).
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);

  // Initialize the serial communication for output (optional).
  Serial.begin(9600);
}

void loop() {
  // Select the ADC channel (analog pin). For example, to read from A0:
  ADMUX = (ADMUX & 0xF0) | 0x00; // Set the channel to 0 (A0).

  // Start the ADC conversion.
  ADCSRA |= (1 << ADSC);

  // Wait for the conversion to complete.
  while (ADCSRA & (1 << ADSC));

  // Read the ADC result (a 10-bit value) from the registers.
  int adcValue = ADC;

  // Print the ADC value to the serial monitor (optional).
  Serial.println(adcValue);

  // Delay for some time before the next reading (optional).
  delay(1000);
}
