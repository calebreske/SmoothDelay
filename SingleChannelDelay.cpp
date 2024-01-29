//
//  SingleChannelDelay.cpp
//  SmoothDelay
//
//  Created by Caleb Reske on 1/28/24.
//

#include <JuceHeader.h>
#include "SingleChannelDelay.h"

SingleChannelDelay::SingleChannelDelay()
{
    writeHead = 0;
    readHead = 0;
}

SingleChannelDelay::~SingleChannelDelay()
{
    
}

void SingleChannelDelay::prepare(int sampleRate)
{
    sr = sampleRate;
    
    maxDelaySamples = maxDelayLength * sr;
    if (sr != 0)
    {
        timeConstantPerSample = timeConstant / sr;
    }
    
    // Prepare ring buffer
    buffer.setSize(1, maxDelaySamples);
}

void SingleChannelDelay::process(dsp::AudioBlock<float>& block)
{
    state = updatingState;
    
    // We assume this block is one channel.
    auto numSamples = block.getNumSamples();
    
    if (state.enable)
    {
        for (int i = 0; i < numSamples; ++i)
        {
            incrementReadHead();
            
            // Read samples from block and buffer
            float readSample = buffer.getSample(0, (int)readHead);
            float blockSample = block.getSample(0, i);
            
            // Shape the result
            block.setSample(0, i, (state.mix * (blockSample + readSample)) + (1.f - state.mix) * (blockSample));
            
            // Write sample to buffer (never write a sample above 10.f)
            float sampleToWrite = ((block.getSample(0, i) * state.feedback)) + (blockSample * (1.f - state.feedback));
            if (isnan(sampleToWrite) || isinf(sampleToWrite)) sampleToWrite = 0.f;
            buffer.setSample(0, writeHead, sampleToWrite);
            
            incrementWriteHead();
        }
    }
    else
    {
        for (int i = 0; i < numSamples; ++i)
        {
            incrementReadHead();
            
            // Write current block sample to buffer
            buffer.setSample(0, writeHead, block.getSample(0, i));
            
            incrementWriteHead();
        }
    }
}

inline void SingleChannelDelay::incrementReadHead()
{
    // Increment read head (could likely improve this performance, will leave for now)
    readHead++;
    if ((readHead) >= maxDelaySamples) readHead -= maxDelaySamples;
    
    // Target Distance is the distance the read head needs to move to catch up with where it needs to be.
    long double targetDistance = writeHead - state.delaySamples - readHead;
    if (writeHead < readHead) targetDistance += maxDelaySamples;
    
    // Smoothly move readHead toward target value
    if (abs(targetDistance) > 0.5f)
    {
        // Figure out the distance you need to go towards the target delay
        double distanceToTravel = targetDistance * timeConstantPerSample;
        if (1 + distanceToTravel < minReadHeadSpeed) distanceToTravel = -1 + minReadHeadSpeed;
        else if (1 + distanceToTravel > maxReadHeadSpeed) distanceToTravel = -1 + maxReadHeadSpeed;
        
        readHead += distanceToTravel;
        if (readHead >= maxDelaySamples) readHead -= maxDelaySamples;
    }
}

inline void SingleChannelDelay::incrementWriteHead()
{
    // Increment write head
    writeHead = (writeHead + 1) % maxDelaySamples;
}

// Public parameter setters
void SingleChannelDelay::setDelayMix(float newDelayMix)
{
    updatingState.mix = newDelayMix;
}

void SingleChannelDelay::setDelayTime(float newDelayTimeSeconds)
{
    updatingState.delayTimeSeconds = newDelayTimeSeconds;
    updatingState.delaySamples = newDelayTimeSeconds * sr;
}

void SingleChannelDelay::setFeedback(float newFeedback)
{
    updatingState.feedback = newFeedback;
}

void SingleChannelDelay::setEnable(bool newEnable)
{
    updatingState.enable = newEnable;
}

// MUST be called from the audio thread
void SingleChannelDelay::setDelayTimeInstant(float newDelayTimeSeconds)
{
    // This is technically a race condition, but setDelayTimeInstant is not really meant to be used during real-time audio processing. Use it, for example, upon construction of your delay object to set the initial delay time
    updatingState.delayTimeSeconds = newDelayTimeSeconds;
    updatingState.delaySamples = newDelayTimeSeconds * sr;
    
    readHead = writeHead - updatingState.delaySamples;
    if (readHead < 0) readHead += maxDelaySamples;
}
