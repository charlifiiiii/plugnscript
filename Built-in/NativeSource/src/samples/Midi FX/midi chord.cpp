// C++ scripting support-----------------------------
#include "dspapi.h"
#include "cpphelpers.h"

/** \file
*   MIDI chord builder.
*   Adds harmonies to incoming MIDI notes with selected pitch shift(s).
*/

#include "../library/Midi.h"

// metadata
DSP_EXPORT string name="MIDI Chord Maker";
DSP_EXPORT string description="Adds transposed MIDI events to create chords";

// parameters
DSP_EXPORT array<string> inputParametersNames={"Shift 1", "Shift 2", "Shift 3", "Shift 4"} ;
DSP_EXPORT array<double> inputParameters(inputParametersNames.length);

DSP_EXPORT array<double> inputParametersMin={-24,-24,-24,-24};
DSP_EXPORT array<double> inputParametersMax={24,24,24,24};
DSP_EXPORT array<double> inputParametersDefault={0,0,0,0};
DSP_EXPORT array<int> inputParametersSteps={49,49,49,49};

// local variables
array<int8> offsets(4);
MidiEvent tempEvent; ///< defining temp object in the script to avoid allocations in time-critical processBlock function

/* per-block processing function: called for every block with updated parameters values.
*
*/
DSP_EXPORT void processBlock(BlockData& data)
{
    // iterate on input MIDI events
    for(uint i=0;i<data.inputMidiEvents.length;i++)
    {
        // forward all events (unchanged)
        data.outputMidiEvents.push(data.inputMidiEvents[i]);

        // add transposed Note On and Off events
        MidiEventType type=MidiEventUtils::getType(data.inputMidiEvents[i]);
        if(type==kMidiNoteOn || type==kMidiNoteOff)
        {
            for(uint j=0;j<offsets.length;j++)
            {
                if(offsets[j]!=0)
                {
                    tempEvent=data.inputMidiEvents[i];
                    int8 note=MidiEventUtils::getNote(data.inputMidiEvents[i]);
                    note+=offsets[j];
                    if(note>=0)
                    {
                        MidiEventUtils::setNote(tempEvent,note);
                        data.outputMidiEvents.push(tempEvent);
                    }
                }
            }
        }
    }
}

/* Update internal variables based on input parameters.
*
*/
DSP_EXPORT void updateInputParametersForBlock(const TransportInfo* info)
{
    // store MIDI note offsets as integers
    for(uint i=0;i<offsets.length;i++)
    {
        if(inputParameters[i]<0)
            offsets[i]=int8(inputParameters[i]);
        else
            offsets[i]=int8(inputParameters[i]+.5);
    }
}
