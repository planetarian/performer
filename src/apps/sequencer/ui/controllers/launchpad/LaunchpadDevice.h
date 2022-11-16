#pragma once

#include "apps/sequencer/ui/controllers/MidiDevice.h"

#include "core/midi/MidiMessage.h"

#include <array>
#include <bitset>
#include <functional>

// Compatible with: Launchpad S, Launchpad Mini Mk1 and Mk2
class LaunchpadDevice : public MidiDevice {
public:
    static constexpr int Rows = 8;
    static constexpr int Cols = 8;
    static constexpr int ExtraRows = 2;
    static constexpr int ButtonCount = (Rows + ExtraRows) * Cols;

    static constexpr int SceneRow = 8;
    static constexpr int FunctionRow = 9;

    virtual const char * getName() override {
        return "Launchpad";
    }

    typedef std::function<void(int, int, bool)> ButtonHandler;

    struct Color {
        union {
            struct {
                uint8_t red: 4;
                uint8_t green: 4;
            };
            uint8_t data;
        };

        Color(int red, int green) : red(red), green(green) {}
    };

    LaunchpadDevice();
    virtual ~LaunchpadDevice();

    // midi handling
    
    void recvMidi(uint8_t cable, const MidiMessage &message) override;

    // button handling

    void setButtonHandler(ButtonHandler buttonHandler) {
        _buttonHandler = buttonHandler;
    }

    bool buttonState(int row, int col) const {
        return _buttonState[row * Cols + col];
    }

    // led handling

    void clearLeds() {
        std::fill(_ledState.begin(), _ledState.end(), 0);
    }

    virtual void setLed(int row, int col, Color color) {
        _ledState[row * Cols + col] = color.data;
    }

    virtual void setLed(int row, int col, int red, int green) {
        uint8_t state = (red & 0x3) | ((green & 0x3) << 4);
        _ledState[row * Cols + col] = state;
    }

    virtual void syncLeds();

protected:

    void setButtonState(int row, int col, bool state) {
        _buttonState[row * Cols + col] = state;
        if (_buttonHandler) {
            _buttonHandler(row, col, state);
        }
    }

    ButtonHandler _buttonHandler;
    std::bitset<ButtonCount> _buttonState;
    std::array<uint8_t, ButtonCount> _ledState;
    std::array<uint8_t, ButtonCount> _deviceLedState;
};
