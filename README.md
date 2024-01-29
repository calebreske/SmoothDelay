# License
This project is licensed under the terms of the MIT license.

# Description
This is a very simple and lightweight delay-line class for JUCE I built as part of a larger project I'm working on. I wanted the delay to have a smooth tape-like varispeed transition, so that changing the delay time wouldn't result in nasty clicking. I couldn't find anything like this online, so I decided to make one myself! Figured it could be useful to anyone looking for something similar.

If you have any questions just send me an email at calebreske@gmail.com.

# Usage
Simply include SmoothDelay.h in your processor, declare a SmoothDelay instance, prepare, and process!

ex.
```
#include <JuceHeader.h>
#include SmoothDelay.h
...

// In class definition:
SmoothDelay delay;
...

// In prepareToPlay():
delay.prepare(getTotalNumOutputChannels(), sampleRate, samplesPerBlock);
...

// In process loop:
auto block = juce::dsp::AudioBlock<float> (buffer);
delay.process(block);
...

// In GUI listeners:
delay.setFeedback(newFeedback);
```

# Other
Constants can be set in the SingleChannelDelay.h file:
- maxDelayLength: the maximum number of seconds the class will ever need to delay
- minReadHeadSpeed: the minimum speed of the read head when changing delay times (setting this to negative will allow the read head to play backwards to reach its target faster).
- maxReadHeadSpeed: the maximum speed the read head is allowed to move. For example, a value of 2 allows the read head to play an octave above the original signal.
- timeConstant: how fast the delay time will smoothly change. I've found a time constant around 3 sounds good to my ears.
