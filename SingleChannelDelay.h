//
//  SingleChannelDelay.h
//  SmoothDelay
//
//  Created by Caleb Reske on 1/28/24.
//

#ifndef SingleChannelDelay_h
#define SingleChannelDelay_h

#include <JuceHeader.h>

using namespace juce;

class SingleChannelDelay
{
public:
    SingleChannelDelay();
    ~SingleChannelDelay();
    
    void prepare(int sampleRate);
    void process(dsp::AudioBlock<float>& block);
    
    // Parameter setters
    void setDelayMix(float newDelayMix);
    void setDelayTime(float newDelayTimeSeconds);
    void setFeedback(float newFeedback);
    void setEnable(bool newEnable);
    
    void setDelayTimeInstant(float newDelayTimeSeconds); // MUST be called from the audio thread
private:
    int sr;
    
    // Thread-safe parameter state
    struct State
    {
        float mix;
        float abnormality;
        float delayTimeSeconds;
        int delaySamples;
        int algorithmChoice;
        float feedback;
        float wobble;
        bool enable;
    };
    
    State state;
    State updatingState;
    
    // Local variables
    long double readHead;
    int writeHead;
    int maxDelaySamples; // The size of the buffer
    
    inline void incrementReadHead();
    inline void incrementWriteHead();
    
    // Ring Buffer
    AudioBuffer<float> buffer;
    
    // Constants
    const float maxDelayLength = 12.f;
    const float minReadHeadSpeed = 0.2f;
    const float maxReadHeadSpeed = 5.f;
    const float timeConstant = 3.072;
    double timeConstantPerSample;
};


#endif /* SingleChannelDelay_h */
