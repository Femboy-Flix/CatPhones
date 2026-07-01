#ifndef PSYCHOACOUSTIC_PROCESSOR_H
#define PSYCHOACOUSTIC_PROCESSOR_H

#include <Arduino.h>

/**
 * Psychoacoustic Audio Processor
 * Implements perceptual audio enhancement techniques:
 * - Dynamic range compression for consistent loudness
 * - Bass boost (loudness enhancement at low frequencies)
 * - Presence peak (perceived clarity/brightness)
 * - Psychoacoustic loudness normalization
 */

class PsychoacousticProcessor {
private:
  // Compressor state
  float compressorThreshold = -14.0f;  // Lower threshold for more compression
  float compressorRatio = 5.0f;        // Stronger compression
  float compressorAttack = 8.0f;       // Faster attack
  float compressorRelease = 80.0f;     // Faster release
  float compressorGain = 0.0f;         // current gain reduction
  float sampleRate = 48000.0f;         // Sample rate for time-based parameters

  // Filter state for bass boost (low-pass)
  float bassBoostBias = 0.0f;
  float bassBoostGain = 2.7f;  // Aggressive bass boost

  // Presence peak (peaking EQ around 2-4 kHz)
  float presenceGain = 2.2f;   // More noticeable presence/clarity

  // Loudness curve parameters
  float loudnessReference = 1.0f;

  // Master output gain for overall loudness
  float masterGain = 2.0f;  // Louder output

  // Simple low-pass filter coefficients for bass
  float lpfAlpha = 0.07f;  // Lower alpha = less lows, tighter bass
  float prevSampleL = 0.0f;
  float prevSampleR = 0.0f;

public:
  PsychoacousticProcessor(float rate = 48000.0f);
  
  /**
   * Process a stereo sample pair with psychoacoustic enhancement
   * @param left Input left channel sample (-1.0 to 1.0)
   * @param right Input right channel sample (-1.0 to 1.0)
   * @param outLeft Output left channel sample
   * @param outRight Output right channel sample
   */
  void processStereo(float left, float right, float& outLeft, float& outRight);
  
  /**
   * Set bass boost amount (0.0 = off, 1.0 = normal, >1.0 = boost)
   */
  void setBassBoost(float boost);
  
  /**
   * Set presence peak amount (0.0 = off, 1.0 = normal, >1.0 = boost)
   */
  void setPresencePeak(float boost);
  
  /**
   * Set master output gain (>1.0 = louder, <1.0 = quieter)
   */
  void setMasterGain(float gain);
  
  /**
   * Set compressor threshold in dB
   */
  void setCompressorThreshold(float dB);
  
  /**
   * Set compressor ratio
   */
  void setCompressorRatio(float ratio);
  
  /**
   * Enable/disable psychoacoustic processing
   */
  void setEnabled(bool enabled);
  
  /**
   * Reset internal state (filters, compressor envelope)
   */
  void reset();

private:
  bool enabled = true;
  
  // Simple compressor with envelope follower
  float compressorEnvelopeFollower(float sample);
  
  // Approximate bass boost using simple filter
  float applyBassBoost(float sample, float& prevSample);
  
  // Convert linear amplitude to dB
  float linearTodB(float linear);
  
  // Convert dB to linear amplitude
  float dBtoLinear(float dB);
};

#endif // PSYCHOACOUSTIC_PROCESSOR_H
