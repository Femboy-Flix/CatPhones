#include <Arduino.h>
#include "AudioTools.h"
#include "BluetoothA2DPSink.h"
#include "PsychoacousticProcessor.h"
#include "WS2812Rainbow.h"
#include <Adafruit_NeoPixel.h>
#include "AudioProcessingChain.h"

// Button GPIO pins
#define BUTTON_PLAY_PAUSE 5
#define BUTTON_PREVIOUS 18
#define BUTTON_NEXT 21
#define BUTTON_VOLUME_UP 22
#define BUTTON_VOLUME_DOWN 19

// I2S GPIO pins
#define I2S_LRC 26  // LRC pin (WS)
#define I2S_BCLK 27 // BCLK pin
#define I2S_DIN 25  // DIN pin (data in)

// WS2812 GPIO pins
#define WS2812_PIN 15
#define WS2812_NUM_LEDS 16 // Change to your number of LEDs
#define WS2812_BRIGHTNESS 10 // 0-255, user adjustable

// I2S and bluetooth sink
//I2SStream i2s;
//BluetoothA2DPSink a2dp_sink(i2s);
I2SStream i2s;
PsychoacousticProcessor audioProcessor(48000.0f);
AudioProcessingChain audioChain(i2s, audioProcessor);
BluetoothA2DPSink a2dp_sink(audioChain);

// Psychoacoustic audio processor
//PsychoacousticProcessor audioProcessor(48000.0f);

// WS2812 Rainbow effect
WS2812Rainbow ws2812(WS2812_NUM_LEDS, WS2812_PIN);

// Button states
bool lastPlayPauseState = HIGH;
bool lastPreviousState = HIGH;
bool lastNextState = HIGH;
bool lastVolumeUpState = HIGH;
bool lastVolumeDownState = HIGH;

// Playback state
bool isPlaying = false;


// Function to play a beep sound




void setup()
{
  Serial.begin(115200);

  // Configure buttons
  pinMode(BUTTON_PLAY_PAUSE, INPUT_PULLUP);
  pinMode(BUTTON_PREVIOUS, INPUT_PULLUP);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_VOLUME_UP, INPUT_PULLUP);
  pinMode(BUTTON_VOLUME_DOWN, INPUT_PULLUP);

  // Configure I2S pins
  auto cfg = i2s.defaultConfig();
  cfg.pin_bck = I2S_BCLK; // BCLK pin
  cfg.pin_ws = I2S_LRC;   // LRC pin
  cfg.pin_data = I2S_DIN; // DIN pin
  i2s.begin(cfg);

  // Start Bluetooth sink
  a2dp_sink.start("Cat-Phones");
  a2dp_sink.set_volume(100);


  // Configure psychoacoustic processing
  audioProcessor.setBassBoost(1.9f);           // +90% bass boost - very prominent
  audioProcessor.setPresencePeak(1.0f);       // +20% presence peak - strong clarity
  audioProcessor.setCompressorThreshold(0.5f);  // Very aggressive compression
  audioProcessor.setCompressorRatio(2.0f);     // High compression ratio
  audioProcessor.setMasterGain(2.4f);          // +140% overall gain boost
  audioProcessor.setEnabled(true);

  // Initialize WS2812
  ws2812.begin();
  ws2812.setBrightness(WS2812_BRIGHTNESS);

}



void loop()
{

  // Read button states
  bool currentPlayPauseState = digitalRead(BUTTON_PLAY_PAUSE);
  bool currentPreviousState = digitalRead(BUTTON_PREVIOUS);
  bool currentNextState = digitalRead(BUTTON_NEXT);
  bool currentVolumeUpState = digitalRead(BUTTON_VOLUME_UP);
  bool currentVolumeDownState = digitalRead(BUTTON_VOLUME_DOWN);
  Serial.println(a2dp_sink.get_volume());
  if (currentPlayPauseState == LOW && lastPlayPauseState == HIGH)
  {

    Serial.println("  Play/Pause Button Pressed");
    audioChain.triggerBeep(800);
    if (isPlaying)
    {
      a2dp_sink.pause(); // Call pause method
      isPlaying = false;
      Serial.println("  Paused");
      
    }
    else
    {
      a2dp_sink.play(); // Call play method
      isPlaying = true;
      Serial.println(" Playing");
      
    }
  }
  lastPlayPauseState = currentPlayPauseState;

  // Handle Previous Track button
  if (currentPreviousState == LOW && lastPreviousState == HIGH)
  {
    Serial.println("Previous Track Button Pressed");
    audioChain.triggerBeep(500);
    a2dp_sink.previous(); // Go to the previous track
  }
  lastPreviousState = currentPreviousState;

  // Handle Next Track button
  if (currentNextState == LOW && lastNextState == HIGH)
  {
    Serial.println("Next Track Button Pressed");
    audioChain.triggerBeep(500);
    a2dp_sink.next(); // Go to the next track
  }
  lastNextState = currentNextState;

  // Handle Volume Up button
  if (currentVolumeUpState == LOW && lastVolumeUpState == HIGH)
  {
    Serial.println("Volume Up Button Pressed");
    audioChain.triggerBeep(1200);
    int currentVol = a2dp_sink.get_volume();
    if (currentVol < 127) {
      a2dp_sink.set_volume(currentVol + 8); // step by 8 for more noticeable change
      Serial.print("Volume set to: ");
      Serial.println(a2dp_sink.get_volume());
    }
  }
  lastVolumeUpState = currentVolumeUpState;

  // Handle Volume Down button
  if (currentVolumeDownState == LOW && lastVolumeDownState == HIGH)
  {
    Serial.println("Volume Down Button Pressed");
    audioChain.triggerBeep(200);
    int currentVol = a2dp_sink.get_volume();
    if (currentVol > 0) {
      int newVol = currentVol - 8;
      if (newVol < 0) newVol = 0;
      a2dp_sink.set_volume(newVol); // ensure volume never goes below 0
      Serial.print("Volume set to: ");
      Serial.println(a2dp_sink.get_volume());
    }
  }
  lastVolumeDownState = currentVolumeDownState;

  // Update WS2812 rainbow effect
  ws2812.loop();

  // Short delay to debounce buttons
  delay(50);
}

