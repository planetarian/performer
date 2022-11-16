#include "LaunchpadProMk3Device.h"

//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 90|  | 91| 92| 93| 94| 95| 96| 97| 98|  | 99| < CC
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//   v CC                v Notes v            CC v
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 80|  | 81|...|   |   |   |   |   | 88|  | 89| row 7
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 70|  | 71|...|   |   |   |   |   | 78|  | 79| row 6
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 60|  | 61|...|   |   |   |   |   | 68|  | 69| row 5
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 50|  | 51|...|   |   |   |   |   | 58|  | 59| row 4
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 40|  | 41|...|   |   |   |   |   | 48|  | 49| row 3
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 30|  | 31|...|   |   |   |   |   | 38|  | 39| row 2
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 20|  | 21|...|   |   |   |   |   | 28|  | 29| row 1
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//  | 10|  | 11|...|   |   |   |   |   | 18|  | 19| row 0
//  +---+  +---+---+---+---+---+---+---+---+  +---+
//         |101|102|103|104|105|106|107|108| < CC
//         +---+---+---+---+---+---+---+---+
//         |  1|  2|  3|  4|  5|  6|  7|  8| < CC
//         +---+---+---+---+---+---+---+---+
//          cl0 cl1 cl2 cl3 cl4 cl5 cl6 cl7


LaunchpadProMk3Device::LaunchpadProMk3Device() : MidiDevice()
{
    std::fill(_ledUpdateArr.begin(), _ledUpdateArr.end(), 0);
    uint8_t preamble[] = { 0x00, 0x20, 0x29, 0x02, 0x0e, 0x03 };
    for (uint8_t i = 0; i < sizeof(preamble); i++) {
        _ledUpdateArr[i] = preamble[i];
    }
}

void LaunchpadProMk3Device::initialize() {
    // send sysex message to enter programmer mode
    std::array<uint8_t, 7> payload { 0x00, 0x20, 0x29, 0x02, 0x0e, 0x0e, 0x01 };
    sendMidi(Cable, MidiMessage::makeSystemExclusive(payload.data(), payload.size()));
}

void LaunchpadProMk3Device::recvMidi(uint8_t cable, const MidiMessage &message) {
    if (message.isNoteOn() || message.isNoteOff()) {
        int index = message.note();
        //int row = 7 - (index - 11) / 10;
        //int col = (index - 11) % 10;
        setButtonState(index, message.velocity(), false);
    } else if (message.isControlChange()) {
        int index = message.controlNumber();
        setButtonState(index, message.controlValue(), false);
    } else if (message.isKeyPressure()) {
        int index = message.note();
        setButtonState(index, message.keyPressure(), true);
    }
}

void LaunchpadProMk3Device::syncLeds() {
    int payloadLength = LedUpdatePreambleLength;
    uint8_t updatedLeds[10];
    int numUpdated = 0;
    for (int b = 0; b < ButtonCount; b++) {
        if (_ledState[b] != _deviceLedState[b]) {

            // Static palette-based colors can use note messages
            if (_ledState[b].type == ColorSpec::ColorType::Static) {
                if (sendMidi(Cable, MidiMessage::makeNoteOn(0, b, _ledState[b].data[0]))) {
                    _deviceLedState[b] = _ledState[b];
                }
                continue;
            }

            // Otherwise, use led control sysex

            _ledUpdateArr[payloadLength++] = _ledState[b].type;
            _ledUpdateArr[payloadLength++] = b;

            
            int len = _ledState[b].dataLength();
            std::copy_n(_ledState[b].data.begin(), len, _ledUpdateArr.begin() + payloadLength);
            payloadLength += len;
            updatedLeds[numUpdated++] = b;

            if (numUpdated == 10 || payloadLength > 27) { // No more room left in the payload pool slot
                checkSendLedUpdate(updatedLeds, numUpdated, payloadLength);
                numUpdated = 0;
                payloadLength = LedUpdatePreambleLength;
            }

            //if (sendMidi(Cable, MidiMessage::makeSystemExclusive(_ledUpdateArr.data(), payloadLength)))
            //if (sendMidi(Cable, MidiMessage::makeNoteOn(0, b, _ledState[b].data[0])))
            //if (sendMidi(Cable, MidiMessage::makeSystemExclusive(payload.data(), 9)))
            //_deviceLedState[b] = _ledState[b];
        }
    }
    if (numUpdated > 0) {
        checkSendLedUpdate(updatedLeds, numUpdated, payloadLength);
    }
}

bool LaunchpadProMk3Device::checkSendLedUpdate(uint8_t *updatedLeds, int numUpdated, int payloadLength)
{
    if (sendMidi(Cable, MidiMessage::makeSystemExclusive(_ledUpdateArr.data(), payloadLength))) {
        for (int i = 0; i < numUpdated; i++) {
            _deviceLedState[updatedLeds[i]] = _ledState[updatedLeds[i]];
        }
        return true;
    }
    return false;
}
