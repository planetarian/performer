#pragma once

#include "core/midi/MidiMessage.h"

#include <array>
#include <bitset>
#include <functional>

class MidiDevice {
public:

    typedef std::function<bool(uint8_t cable, const MidiMessage &)> SendMidiHandler;

    MidiDevice() {

    }
    virtual ~MidiDevice() {

    }
    
    virtual const char * getName() {
        return "MIDI Device";
    }
    
    // initialization

    virtual void initialize() {

    }

    // midi handling

    void setSendMidiHandler(SendMidiHandler sendMidiHandler) {
        _sendMidiHandler = sendMidiHandler;
    }
    
    virtual void recvMidi(uint8_t cable, const MidiMessage &message) {
        
    }

protected:
    static constexpr uint8_t Cable = 0;

    bool sendMidi(uint8_t cable, const MidiMessage &message) {
        if (_sendMidiHandler) {
            return _sendMidiHandler(cable, message);
        }
        return false;
    }

    SendMidiHandler _sendMidiHandler;

};