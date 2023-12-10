#include <Arduino.h>

const int NUM_SPEAKERS = 20;  
const int speakerButtons[NUM_SPEAKERS] = {6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44};
const int ledPins[NUM_SPEAKERS] = {7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45};
const int startButton = 2;
const int stopButton = 4;
const int startButtonLED = 3;
const int stopButtonLED = 5;
int group2[NUM_SPEAKERS / 2];
int group5[NUM_SPEAKERS / 4];
int group10[NUM_SPEAKERS / 10];
int group20[NUM_SPEAKERS / 20];

int speakerOrder[NUM_SPEAKERS];
int soundOrder[NUM_SPEAKERS];


int currentIndex = 0;
bool soundSent = false;
bool gameCompleted = false;
bool ledStates[NUM_SPEAKERS] = {0}; // Tracks which LEDs should be lit

unsigned long lastBlinkTime = 0;
const int blinkInterval = 400; // Time in ms between blinks

void setup() {
    Serial.begin(9600);
    initializeGroups();

    // Initialize speaker buttons as INPUT_PULLUP and LEDs as OUTPUT
    for (int i = 0; i < NUM_SPEAKERS; i++) {
        pinMode(speakerButtons[i], INPUT_PULLUP);
        pinMode(ledPins[i], OUTPUT);
        digitalWrite(ledPins[i], LOW); // Turn off all LEDs initially
    }

    pinMode(startButton, INPUT_PULLUP);
    pinMode(stopButton, INPUT_PULLUP);
    pinMode(startButtonLED, OUTPUT);
    pinMode(stopButtonLED, OUTPUT);

    // Turn on start and stop button LEDs
    digitalWrite(startButtonLED, HIGH);
    digitalWrite(stopButtonLED, HIGH);

    // A short delay to ensure the system settles before starting a new game
    delay(100);
}

void loop() {
    // Turn off LED when button is pressed, otherwise keep it on
    digitalWrite(startButtonLED, digitalRead(startButton));
    digitalWrite(stopButtonLED, digitalRead(stopButton));


    // Game logic loop
    if (!gameCompleted) {
        // If start button is pressed, initialize a new game
        if (digitalRead(startButton) == LOW) {
            startNewGame();
        }

        int currentSpeaker = speakerOrder[currentIndex];
        int currentSound = soundOrder[currentIndex];

        // Notify which speaker and sound should play
        if (!soundSent) {
            Serial.print(currentSound);
            Serial.print(" ");
            Serial.print(currentSpeaker);
            Serial.print(" ");
            Serial.println("1");
            soundSent = true;
        }

        // Check all speaker buttons to see which is pressed
        for (int i = 0; i < NUM_SPEAKERS; i++) {
            if (digitalRead(speakerButtons[i]) == LOW) {
                // Stop the current sound
                Serial.print(currentSound);
                Serial.print(" ");
                Serial.print(currentSpeaker);
                Serial.print(" ");
                Serial.println("0");

                // If the correct button is pressed, move to the next speaker
                if (i == currentSpeaker) {
                    currentIndex++;
                    soundSent = false;

                    // Update LED state for the speaker button that was correctly pressed
                    ledStates[currentSpeaker] = true;
                    digitalWrite(ledPins[currentSpeaker], HIGH); // Turn on the LED

                    // If all speakers have been checked, complete the game
                    if (currentIndex == NUM_SPEAKERS) {
                        gameCompleted = true;
                        playComplete();
                        return;
                    }
                } else {  // Wrong button was pressed; stop game
                    stopGame();
                    return;
                }
                delay(500); // Debounce delay
            }
        }

        // If stop button is pressed before game completion, stop the current speaker and sound
        if (digitalRead(stopButton) == LOW) {
            stopGame();
            delay(500); // Debounce delay
        }
    } else {  // Game has been completed
        // If start button is pressed after game completion, initialize a new game
        if (digitalRead(startButton) == LOW) {
            startNewGame();
        }

        // If stop button is pressed after game completion, stop the game
        if (digitalRead(stopButton) == LOW) {
            stopGame();
            delay(500); // Debounce delay
        }
    }
    
    // Blink LEDs logic
    if (millis() - lastBlinkTime >= blinkInterval) {
        // blinkLEDs();
        randomBlinkLEDs();
        lastBlinkTime = millis();
    }
}
void randomBlinkLEDs() {
    for (int i = 0; i < NUM_SPEAKERS; i++) {
        if (ledStates[i]) { // Only blink LEDs that are supposed to be on
            // Randomly decide whether to turn this LED on or off
            digitalWrite(ledPins[i], random(2) == 0 ? HIGH : LOW);
        }
    }
}

void initializeGroups() {
    // Populate the group arrays
    // This is a simple example; adjust according to your specific needs
    for (int i = 0; i < NUM_SPEAKERS / 2; i++) {
        group2[i] = 2 * i;
    }

    for (int i = 0; i < NUM_SPEAKERS / 4; i++) {
        group5[i] = 4 * i;
    }

    for (int i = 0; i < NUM_SPEAKERS / 10; i++) {
        group10[i] = 10 * i;
    }

    for (int i = 0; i < NUM_SPEAKERS / 20; i++) {
        group20[i] = 20 * i; // Assuming NUM_SPEAKERS is at least 20
    }
}
// Modify the blinkLEDs function to blink all LEDs
void blinkLEDs() {
    static bool ledState = LOW;

    for (int i = 0; i < NUM_SPEAKERS; i++) {
        if (ledStates[i]) { // Only blink LEDs that are supposed to be on
            digitalWrite(ledPins[i], ledState);
        }
    }

    ledState = !ledState; // Toggle LED state
}

// Function to initialize the game
void startNewGame() {
    // Reset the game logic flags
    currentIndex = 0;
    soundSent = false;
    gameCompleted = false;

    // Generate random orders for speakers and sounds
    generateOrder(speakerOrder, NUM_SPEAKERS);
    generateOrder(soundOrder, NUM_SPEAKERS);

    // Turn off all LEDs
    for (int i = 0; i < NUM_SPEAKERS; i++) {
        ledStates[i] = false;
        digitalWrite(ledPins[i], LOW);
    }

    // Clear any game state in Max/MSP
    Serial.println("clear");

    // A short delay to ensure the system settles before starting a new game
    delay(100);
}

// Function to generate a random order for the speakers or sounds
void generateOrder(int list[], int size) {
    for (int i = 0; i < size; i++) {
        list[i] = i;
    }
    for (int i = size - 1; i > 0; i--) {
        int j = random(i + 1);
        int temp = list[i];
        list[i] = list[j];
        list[j] = temp;
    }
}


void playGroup(int groupsize) {
    // Start the sound for each speaker in the group of 2
    for (int j = 0; j < 20; j += groupsize) {
        for (int i = j; i < j+groupsize; i++){ 
          Serial.print(soundOrder[i]);
          Serial.print(" ");
          Serial.print(i);
          Serial.print(" ");
          Serial.print("1");
          Serial.print(" ");
        }
       Serial.println();
       // Wait for 10 seconds
        delay(5000);

        // Stop the sound for the speakers in the group
        Serial.println("clear");
    }

    // Wait for 1 second before starting the next group (if any)
    delay(1000);
}

void playComplete() {
    // Play grouped speakers, pairs, and all speakers in sequence
    playGroup(2);
    playGroup(5);
    playGroup(10);
    playGroup(20);
}

// Function to stop all sounds and send "clear" to Max/MSP
void stopGame() {
    Serial.println("clear"); // Send "clear" to Max/MSP
    playFailSound(); // Play fail sound on all speakers
    delay(1000);
    Serial.println("clear"); // Send "clear" to Max/MSP
}

void playFailSound() {
    for (int i = 0; i < NUM_SPEAKERS; i++) {
        Serial.print("20"); // Play sound 21 (fail sound) on all speakers
        Serial.print(" ");
        Serial.print(i);
        Serial.print(" ");
        Serial.print("1");
        Serial.print(" ");
    }
    Serial.println();
}
