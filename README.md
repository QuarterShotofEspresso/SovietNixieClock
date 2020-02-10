Martin is short for:
Musical 
ARTificial 
INtelligence


Martin is designed to function on the ATMEGA1284. Written in C. It recieves analog signals from a speaker.
These analog signals are converted to digital, unsigned, 8-bit, integer values by an (internal/external)
ADC. These values are then fed into a (GRU/LSTM) which can process the data and spit out the corresponding note.

Beside the board lies another switch which holds the ability to set the model in Observation or Compose mode.

Observation Mode:
Observation mode functions by listening to a sample of music, storing, and operating on consective audio samples

Compose Mode:
Compose mode starts by working on a (provided note/random number generator to function as the starting note), n. From
here the consequent note, n+1, is generated and then fed into the LSTM to produce the consequent note, n+2. These are
also unsigned, 8-bit values that are fed into a DAC which connects to a (monoaudio jack/speaker).


