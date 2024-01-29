//
//  SmoothDelay.h
//  SmoothDelay
//
//  Created by Caleb Reske on 1/28/24.
//

#ifndef SmoothDelay_h
#define SmoothDelay_h

#include <JuceHeader.h>
#include "SingleChannelDelay.h"

using namespace juce;

class SmoothDelay
{
public:
    SmoothDelay();
    ~SmoothDelay();
    
    void prepare(int numChannels, int sampleRate, int blockSize);
    
    void process(dsp::AudioBlock<float>& block);
    
    void setDelayMix(float newDelayMix);
    void setDelayTime(float newDelayTimeSeconds);
    void setFeedback(float newFeedback);
    void setStereoOffsetSeconds(float newStereoOffsetSeconds);
    void setEnable(bool newEnable);
    
    void setDelayTimeInstant(float newDelayTimeSeconds); // MUST be called from the audio thread
    
private:
    int sr;
    
    std::vector<std::unique_ptr<SingleChannelDelay>> delays;
    
    // Thread-safe parameter state
    struct State
    {
        float stereoOffsetSeconds;
        float delayTimeSeconds;
    };
    State state;
    State updatingState;
};

#endif /* SmoothDelay_h */
