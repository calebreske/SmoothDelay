/*
 SmoothDelay.h
 
 Copyright (c) 2023 Caleb Reske

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#ifndef SmoothDelay_h
#define SmoothDelay_h

#include <JuceHeader.h>

using namespace juce;
using namespace dsp;

// A simple, lightweight delay-line class providing for smooth tape-like transition between delay times
template<class T>
class SmoothDelay
{
public:
    SmoothDelay()
    {
        // Default time and pitch constants
        timeConstant = 3.072;
        minReadHeadSpeed = 0.2;
        maxReadHeadSpeed = 5.f;
        feedback = 0.f;
    }
    
    ~SmoothDelay()
    {
        
    }
    
    void prepare(size_t sampleRate, int blockSize, int numChannels, float maxDelayTimeSeconds)
    {
        // Set parameters
        sr = sampleRate;
        maxDelaySamples = maxDelayTimeSeconds * sr;
        
        // Prepare ring buffer and feedback buffer
        buffer.setSize(numChannels, maxDelaySamples);
        feedbackBuffer.setSize(numChannels, blockSize);
        
        // Initialize audio-rate variables
        writeHead = 0;
        readHead = 0;
    }
    
    void process(AudioBlock<T>& block)
    {
        for (int c = 0; c < block.getNumChannels(); ++c)
        {
            feedbackBuffer.copyFrom(c, 0, block.getChannelPointer(c), (int)block.getNumSamples());
        }
        
        readSamples(block);
        
        for (int c = 0; c < block.getNumChannels(); ++c)
        {
            feedbackBuffer.addFromWithRamp(c, 0, block.getChannelPointer(c), (int)block.getNumSamples(), feedback, feedback);
        }
        auto feedbackBlock = dsp::AudioBlock<float>(feedbackBuffer);
        
        writeSamples(feedbackBlock);
    }
    
    void writeSamples(AudioBlock<T>& block)
    {
        numChannels = block.getNumChannels();
        numSamples = block.getNumSamples();
        
        // Write all samples to the write head
        writeHead = (writeHead + numSamples) % maxDelaySamples;
        for (int c = 0; c < numChannels; ++c)
        {
            for (int i = 0; i < numSamples; ++i)
            {
                buffer.setSample(c, writeHead, block.getSample(c, i));
                writeHead = (writeHead + 1) % maxDelaySamples;
            }
            writeHead = (writeHead - numSamples + maxDelaySamples) % maxDelaySamples;
        }
    }
    
    void readSamples(AudioBlock<T>& block)
    {
        numChannels = block.getNumChannels();
        numSamples = block.getNumSamples();
        
        // Read all samples from the read head, while moving it smoothly from its current location targetDistance samples forward
        for (int i = 0; i < numSamples; ++i)
        {
            // Read from the read head
            for (int c = 0; c < numChannels; ++c)
            {
                block.setSample(c, i, buffer.getSample(c, (int)readHead));
            }
            
            // Exponentially go towards where you need to be.
            readHead = fmod((readHead + 1), (float)maxDelaySamples);
            if (abs(targetDistance) > 0.5f)
            {
                double distanceToTravel = targetDistance * timeConstantPerSample;
                if (1 + distanceToTravel < minReadHeadSpeed) distanceToTravel = -1 + minReadHeadSpeed;
                else if (1 + distanceToTravel > maxReadHeadSpeed) distanceToTravel = -1 + maxReadHeadSpeed;
                
                readHead = fmod(readHead + distanceToTravel + maxDelaySamples, (float)maxDelaySamples);
                targetDistance -= distanceToTravel;
            }
        }
    }
    
    void setDelayTime(float newDelayTimeSeconds)
    {
        int newDelaySamples = newDelayTimeSeconds * sr;
        if (delaySamples != newDelaySamples)
        {
            delaySamples = newDelaySamples;
            
            // Set the target distance the read head needs to travel based on the new delay time
            targetDistance = writeHead - delaySamples - readHead;
            if (writeHead < readHead) targetDistance += maxDelaySamples;
        }
    }
    
    void setTimeConstant(float newTimeConstant)
    {
        timeConstant = newTimeConstant;
        timeConstantPerSample = timeConstant / sr;
    }
    
    void setMinReadHeadSpeed(double newMinReadHeadSpeed)
    {
        minReadHeadSpeed = newMinReadHeadSpeed;
    }
    
    void setMaxReadHeadSpeed(double newMaxReadHeadSpeed)
    {
        maxReadHeadSpeed = newMaxReadHeadSpeed;
    }
    
    void setFeedback(float newFeedback)
    {
        feedback = newFeedback;
    }
    
    float getDelayTimeSeconds()
    {
        return (float)delaySamples / sr;
    }
    
    float getFeedback()
    {
        return feedback;
    }
private:
    size_t sr;
    
    // Parameters
    int delaySamples;
    int maxDelaySamples;
    float feedback;
    
    // Buffers
    AudioBuffer<T> buffer;
    AudioBuffer<T> feedbackBuffer;
    
    // Used at audio rate
    size_t numChannels;
    size_t numSamples;
    int writeHead;
    long double readHead;  // Extra precision is needed for the read head
    long double targetDistance;
    
    // Constants
    double minReadHeadSpeed;
    double maxReadHeadSpeed;
    double timeConstant;
    double timeConstantPerSample;
};

#endif /* SmoothDelay_h */
