//
//  SmoothDelay.cpp
//  SmoothDelay
//
//  Created by Caleb Reske on 1/28/24.
//

#include "SmoothDelay.h"

SmoothDelay::SmoothDelay()
{
    
}

SmoothDelay::~SmoothDelay()
{
    
}

void SmoothDelay::prepare(int numChannels, int sampleRate, int blockSize)
{
    this->sr = sampleRate;
    
    delays.clear();
    for (int c = 0; c < numChannels; ++c)
    {
        delays.push_back(std::make_unique<SingleChannelDelay>());
        delays.back()->prepare(sampleRate);
    }
    
    state = updatingState;
}

void SmoothDelay::process(dsp::AudioBlock<float>& block)
{
    if (block.getNumChannels() != delays.size()) prepare(block.getNumChannels(), sr, block.getNumSamples());
    
    state = updatingState;
    
    for (int c = 0; c < block.getNumChannels(); ++c)
    {
        dsp::AudioBlock<float> channelBlock = block.getSingleChannelBlock(c);
        delays[c]->process(channelBlock);
    }
}

// Parameter setters
void SmoothDelay::setDelayMix(float newDelayMix)
{
    for (int c = 0; c < delays.size(); ++c)
    {
        delays[c]->setDelayMix(newDelayMix);
    }
}

void SmoothDelay::setDelayTime(float newDelayTimeSeconds)
{
    for (int c = 0; c < delays.size(); ++c)
    {
        delays[c]->setDelayTime(newDelayTimeSeconds + (c * updatingState.stereoOffsetSeconds));
    }
    updatingState.delayTimeSeconds = newDelayTimeSeconds;
}

void SmoothDelay::setFeedback(float newFeedback)
{
    for (int c = 0; c < delays.size(); ++c)
    {
        delays[c]->setFeedback(newFeedback);
    }
}

void SmoothDelay::setStereoOffsetSeconds(float newStereoOffsetSeconds)
{
    updatingState.stereoOffsetSeconds = newStereoOffsetSeconds;
    setDelayTime(updatingState.delayTimeSeconds);
}

void SmoothDelay::setEnable(bool newEnable)
{
    for (int c = 0; c < delays.size(); ++c)
    {
        delays[c]->setEnable(newEnable);
    }
}

// MUST be called from the audio thread
void SmoothDelay::setDelayTimeInstant(float newDelayTimeSeconds)
{
    for (int c = 0; c < delays.size(); ++c)
    {
        delays[c]->setDelayTimeInstant(newDelayTimeSeconds + (c * updatingState.stereoOffsetSeconds));
    }
    updatingState.delayTimeSeconds = newDelayTimeSeconds;
}
