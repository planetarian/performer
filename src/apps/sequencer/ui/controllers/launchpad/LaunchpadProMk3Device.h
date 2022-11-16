#pragma once

#include "apps/sequencer/ui/controllers/MidiDevice.h"
#include "LaunchpadDevice.h"

#include "core/midi/MidiMessage.h"

// Compatible with Launchpad Pro MK3
class LaunchpadProMk3Device : public MidiDevice {
public:
    static constexpr int ButtonCount = 110;

    struct ColorSpec {
        enum ColorType : uint8_t {
            Static,
            Flash,
            Pulse,
            Rgb
        };
        ColorType type = Static;
        std::array<uint8_t, 3> data = { 0x00, 0x00, 0x00 };

        ColorSpec() : data({ 0, 0, 0 }) {}

        ColorSpec(const ColorSpec& other) : type(other.type), data({ other.data[0], other.data[1], other.data[2] }) {}
        
        ColorSpec(ColorType type, uint8_t byte1, uint8_t byte2 = 0, uint8_t byte3 = 0)
            : type(type), data({ byte1, byte2, byte3}) {}

        uint8_t dataLength() const {
            static const uint8_t lengths[] = { 1, 2, 1, 3 };
            return lengths[type];
        }

        bool operator==(const ColorSpec& b) const {
            return data[0] == b.data[0]
                && data[1] == b.data[1]
                && data[2] == b.data[2]
                && type == b.type;
        }

        bool operator!=(const ColorSpec& b) const {
            for (int i = 0; i < dataLength(); i++) {
                if (data[i] != b.data[i]) {
                    return true;
                }
            }
            return type != b.type;
        }
    };

    LaunchpadProMk3Device();

    const char * getName() override {
        return "Launchpad Pro Mk3";
    }

    typedef std::function<void(uint8_t, uint8_t, bool)> ButtonHandler;

    void initialize();

    void recvMidi(uint8_t cable, const MidiMessage &message);
    
    void setButtonHandler(ButtonHandler buttonHandler) {
        _buttonHandler = buttonHandler;
    }

    inline int getIndex(int row, int col) const {
        if (col == 0) // col 0 will be the right-hand selectors
            col = 9;
        if (row == 0) // row 0 will be the bottom selectors
            row = 10;
        return (row*10) + col;
    }
    
    uint8_t getButtonState(int row, int col) const {
        int index = getIndex(row, col);
        return _buttonState[index];
    }
    uint8_t getButtonState(int index) const {
        return _buttonState[index];
    }

    void setLed(int row, int col, ColorSpec colorSpec) {
        int index = getIndex(row, col);
        _ledState[index] = colorSpec;
    }
    void setLed(int index, ColorSpec colorSpec) {
        _ledState[index] = colorSpec;
    }

    void clearLeds() {
        for (int i = 0; i < ButtonCount; i++) {
            _ledState[i] = ColorSpec();
        }
    }

    void syncLeds();

private:
    static constexpr uint8_t Cable = 0;
    ButtonHandler _buttonHandler;
    std::array<uint8_t, ButtonCount> _buttonState;
    std::array<ColorSpec, ButtonCount> _ledState;
    std::array<ColorSpec, ButtonCount> _deviceLedState;

    static constexpr int LedUpdateArrLength = 64;
    static constexpr int LedUpdatePreambleLength = 6;
    std::array<uint8_t, LedUpdateArrLength> _ledUpdateArr;

    void setButtonState(uint8_t index, uint8_t state, bool isAftertouch) {
        //if (!isAftertouch)
            _buttonState[index] = state;
        if (_buttonHandler) {
            _buttonHandler(index, state, isAftertouch);
        }
    }

    bool checkSendLedUpdate(uint8_t *updatedLeds, int numUpdated, int payloadLength);
    
};
