#ifndef AUDIO_PROCESSING_CHAIN_H
#define AUDIO_PROCESSING_CHAIN_H
#include <math.h>

#include "AudioTools.h"
#include "PsychoacousticProcessor.h"

/**
 * Audio Processing Chain that wraps the I2S stream with psychoacoustic processing
 * This class intercepts audio data before it goes to the I2S output
 */
class AudioProcessingChain : public AudioStream {
private:
  I2SStream& i2sStream;
  PsychoacousticProcessor& processor;
  uint8_t buffer[4096];
  size_t bufferPos = 0;
  // Beep state
  bool beepActive = false;
  float beepPhase = 0.0f;
  float beepFreq = 1200.0f;
  int beepSamplesLeft = 0;

  bool startupBeepPending = true;


public:
  void triggerBeep(float freq = 1200.0f, int durationMs = 30) {
    beepFreq = freq;
    beepSamplesLeft = (48000 * durationMs) / 1000;
    beepPhase = 0.0f;
    beepActive = true;
  }

public:
  AudioProcessingChain(I2SStream& i2s, PsychoacousticProcessor& proc)
      : i2sStream(i2s), processor(proc) {}
  
  // Override write to apply processing
  virtual size_t write(const uint8_t* data, size_t len) override {

    if (startupBeepPending) {
  triggerBeep(1200.0f, 40);   // startup tone
  startupBeepPending = false;
}

    // Process 16-bit PCM stereo audio
    const int16_t* samples = (const int16_t*)data;
    size_t numSamples = len / sizeof(int16_t);
    
    int16_t processedBuffer[numSamples];
    
    // Process samples in stereo pairs
    for (size_t i = 0; i < numSamples; i += 2) {
  float left = samples[i] / 32768.0f;
  float right = samples[i + 1] / 32768.0f;

  // ---------- BEEP OVERLAY (SCHRITT 3) ----------
  if (beepActive && beepSamplesLeft > 0) {
    float beep = sinf(2.0f * PI * beepPhase) * 0.2f;

    beepPhase += beepFreq / 48000.0f;
    if (beepPhase >= 1.0f) beepPhase -= 1.0f;

    left += beep;
    right += beep;

    beepSamplesLeft--;
    if (beepSamplesLeft <= 0) beepActive = false;
  }
  // ---------------------------------------------

  float outLeft, outRight;
  processor.processStereo(left, right, outLeft, outRight);

  processedBuffer[i]     = (int16_t)(outLeft * 32767.0f);
  processedBuffer[i + 1] = (int16_t)(outRight * 32767.0f);
}
    
    // Write processed data to I2S
    return i2sStream.write((const uint8_t*)processedBuffer, len);
  }
  
  virtual int read() override {
    return i2sStream.read();
  }
  
  virtual int available() override {
    return i2sStream.available();
  }
  
  virtual void flush() override {
    i2sStream.flush();
  }
};

#endif // AUDIO_PROCESSING_CHAIN_H
