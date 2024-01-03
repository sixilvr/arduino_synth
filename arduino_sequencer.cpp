/*
Sawtooth Sequencer for Arduino
By Shahir Latif
Board: Arduino MKR Zero
Pins:
	Potentiometers: A1, A2, A3, A4, A5, A6
	LEDs (with resistors): 11, 10, 9, 8, 7, 6
	Speaker output: A0
*/

const float PI = 3.14159265;
const int microSeconds = 1000000;

const int numSteps = 6;
// Note inputs
const int pots[numSteps] = { A1, A2, A3, A4, A5, A6 };
// Step LEDs
const int leds[numSteps] = { 11, 10, 9, 8, 7, 6 };

// Speaker pin
const int speaker = A0;

// Scale (in MIDI notes)
const int numNotes = 6;
const int scale[numNotes] = { 60, 62, 64, 67, 69, 72 };

// Tempo
float tempo = 120;

// Read potentiometer to MIDI note
int getNote(int potNumber)
{
    int value = analogRead(potNumber);
    int noteNumber = map(value, 0, 1023, 0, numNotes - 1);
    return scale[noteNumber];
}

// MIDI note to frequency
float noteToFreq(int note)
{
    return pow(2.0, (note - 69.0) / 12.0) * 440.0;
}

// Sine wave at time
float sineWave(float time, float frequency, float amplitude)
{
    return sin(2.0 * PI * frequency * time) * amplitude;
}

float sawtoothWave(float time, float frequency, float amplitude)
{
    return (2 * fmod(frequency * time - 0.5, 1.0) - 1.0) * amplitude;
}

// [-1, 1] to [0, 1023]
int scaleSpeaker(float value)
{
    return (int)((value + 1) * 511);
}

void setup()
{
    analogWriteResolution(10);
    for (int i = 0; i < numSteps; i++)
    {
        pinMode(leds[i], OUTPUT);
    }
}

void loop()
{
    for (int beat = 0; beat < numSteps; beat++)
    {
        // Turn step LED on
        digitalWrite(leds[beat], HIGH);
        
        // Get frequency
        int note = getNote(beat);
        float frequency = noteToFreq(note);
        
        // Calculate timing
        int beatLength = (60.0 / tempo) * microSeconds;
        int t0 = micros();
        int t = 0;
        
        // Generate each sample
        while (t < beatLength)
        {
            t = micros() - t0;
            float amplitude = (float)(beatLength - t) / beatLength;
            float value = sawtoothWave((float)t / microSeconds, frequency, amplitude);
            analogWrite(speaker, scaleSpeaker(value));
        }
        
        // Turn step LED off
        digitalWrite(leds[beat], LOW);
    }
}
