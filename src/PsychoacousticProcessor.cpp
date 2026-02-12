#include "PsychoacousticProcessor.h"
#include <cmath>

PsychoacousticProcessor::PsychoacousticProcessor(float rate)
    : sampleRate(rate)
{
  // Calculate filter coefficient based on sample rate
  // For a ~100Hz cutoff frequency
  float cutoffFreq = 100.0f;
  lpfAlpha = 2.0f * PI * cutoffFreq / sampleRate / (2.0f * PI * cutoffFreq / sampleRate + 1.0f);
}

void PsychoacousticProcessor::processStereo(float left, float right, float& outLeft, float& outRight)
{
  if (!enabled) {
    outLeft = left;
    outRight = right;
    return;
  }
  
  // Step 1: Apply dynamic range compression
  float compGainReduction = compressorEnvelopeFollower(fabs(left) > fabs(right) ? left : right);
  
  // Step 2: Apply bass boost (enhances perceived loudness at low frequencies)
  float bassLeft = applyBassBoost(left, prevSampleL);
  float bassRight = applyBassBoost(right, prevSampleR);
  
  // Step 3: Apply compression gain
  outLeft = bassLeft * compGainReduction;
  outRight = bassRight * compGainReduction;
  
  // Step 4: Apply presence peak (enhance clarity around 2-4kHz conceptually)
  // Simple approximation: add a small amount of mid-range emphasis
  outLeft *= presenceGain;
  outRight *= presenceGain;
  
  // Step 5: Apply master gain
  outLeft *= masterGain;
  outRight *= masterGain;
  
  // Soft clipping to prevent distortion
  const float softClipThreshold = 0.95f;
  if (outLeft > softClipThreshold) {
    outLeft = softClipThreshold + (1.0f - softClipThreshold) * tanh((outLeft - softClipThreshold) / (1.0f - softClipThreshold));
  } else if (outLeft < -softClipThreshold) {
    outLeft = -softClipThreshold - (1.0f - softClipThreshold) * tanh((fabs(outLeft) - softClipThreshold) / (1.0f - softClipThreshold));
  }
  
  if (outRight > softClipThreshold) {
    outRight = softClipThreshold + (1.0f - softClipThreshold) * tanh((outRight - softClipThreshold) / (1.0f - softClipThreshold));
  } else if (outRight < -softClipThreshold) {
    outRight = -softClipThreshold - (1.0f - softClipThreshold) * tanh((fabs(outRight) - softClipThreshold) / (1.0f - softClipThreshold));
  }
}

void PsychoacousticProcessor::setBassBoost(float boost)
{
  bassBoostGain = fmax(0.0f, boost);
}

void PsychoacousticProcessor::setPresencePeak(float boost)
{
  presenceGain = fmax(0.0f, boost);
}

void PsychoacousticProcessor::setMasterGain(float gain)
{
  masterGain = fmax(0.0f, gain);
}

void PsychoacousticProcessor::setCompressorThreshold(float dB)
{
  compressorThreshold = dB;
}

void PsychoacousticProcessor::setCompressorRatio(float ratio)
{
  compressorRatio = fmax(1.0f, ratio);
}

void PsychoacousticProcessor::setEnabled(bool en)
{
  enabled = en;
}

void PsychoacousticProcessor::reset()
{
  compressorGain = 0.0f;
  prevSampleL = 0.0f;
  prevSampleR = 0.0f;
  bassBoostBias = 0.0f;
}

float PsychoacousticProcessor::compressorEnvelopeFollower(float sample)
{
  float absSample = fabs(sample);
  float sampleDB = linearTodB(absSample + 1e-6f);  // avoid log(0)
  
  // Simple envelope follower with attack/release
  float coeff = (sampleDB > compressorGain) ? 0.1f : 0.05f;  // attack faster than release
  compressorGain = compressorGain * (1.0f - coeff) + sampleDB * coeff;
  
  // Calculate gain reduction
  if (compressorGain > compressorThreshold) {
    float gainReduction = (compressorGain - compressorThreshold) / compressorRatio + compressorThreshold - compressorGain;
    return dBtoLinear(gainReduction);
  }
  
  return 1.0f;  // No compression
}

float PsychoacousticProcessor::applyBassBoost(float sample, float& prevSample)
{
  // Simple low-pass filter for bass detection
  float filtered = prevSample * (1.0f - lpfAlpha) + sample * lpfAlpha;
  prevSample = filtered;
  
  // Boost the low-frequency component
  float boost = filtered * (bassBoostGain - 1.0f);
  return sample + boost * 0.5f;  // Mix in half of the boost to avoid excessive boost
}

float PsychoacousticProcessor::linearTodB(float linear)
{
  return 20.0f * log10f(fmax(linear, 1e-6f));
}

float PsychoacousticProcessor::dBtoLinear(float dB)
{
  return powf(10.0f, dB / 20.0f);
}
