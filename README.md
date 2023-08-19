# LICENSE
This project is licensed under the terms of the MIT license.

# DESCRIPTION
This is a very simple and lightweight delay-line class for JUCE I built as part of a larger project I'm working on. I wanted the delay to have a smooth tape-like varispeed transition, so that changing the delay time wouldn't result in nasty clicking. I couldn't find anything like this online, so I decided to make one myself! Figured it could be useful to anyone looking for something similar.

If there is any demand whatsoever, I'm happy to make a version compatible with any framework, not just JUCE. If you have any questions just send me an email at calebreske@gmail.com.

# USAGE
Simply include SmoothDelay.h in your processor, declare a SmoothDelay instance, prepare, and process!

ex.
```
#include <JuceHeader.h>
#include SmoothDelay.h
...

// In class definition:
SmoothDelay<double> delay;
...

// In prepareToPlay():
delay.prepare(sampleRate, samplesPerBlock, 2, 2.f);
delay.setTimeConstant(2.5);
...

// In process loop:
auto block = juce::dsp::AudioBlock<double> (buffer);
delay.setFeedback(feedback);
delay.process(block);
...
```


Public member functions for the class SmoothDelay:

void prepare(size_t sampleRate, int blockSize, int numChannels, float maxDelayTimeSeconds)
- sampleRate - the desired audio sample rate
- blockSize - the maximum input block size
- numChannels - the number of channels in each block
- maxDelayTimeSeconds - the maximum number of seconds the delay will ever need to accomodate. This is needed to initialize the ring buffer.
prepare() must be called before any other functions.

void process(AudioBlock<T>& block)
- block - the input block of type T
Delay the signal by maxDelayTimeSeconds, incorporating feedback.
    
void readSamples(AudioBlock<T>& block)
void writeSamples(AudioBlock<T>& block)
If more complicated processing is required, use readSamples() and writeSamples(). Always call readSamples() before writeSamples() - if no delay feedback is needed, you should save the initial audio block before calling readSamples and call writeSamples on that block. Calling readSamples() and writeSamples on the same audio block will induce 100% feedback. The process() function is just a simple implementation of this.
    
void setDelayTime(float newDelayTimeSeconds)
Sets the new delay time, in seconds
    
void setTimeConstant(float newTimeConstant)
Sets the new time constant. I have found a value around ~3 to sound good, but you can experiment - the default value is 3.072.

void setMinReadHeadSpeed(double newMinReadHeadSpeed)
void setMaxReadHeadSpeed(double newMaxReadHeadSpeed)
Sets the min and max read-head speeds. The default values are 0.2 and 5.0 - this means that the read head is required to go at least 1/5th as fast as the write head, and at most 5x as fast as the write head. You may set these values to anything you want, as long as the maximum is greater than the minimum - negative values for the minimum read head speed can result in fun backwards travel.

void setFeedback(float newFeedback)
Sets the new feedback value - 0 is 0% feedback, 1 is 100% feedback.

float getDelayTimeSeconds()
Returns the current delay time, in seconds.
    
float getFeedback()
Returns the current feedback value.
