#include "LaunchpadProMk3Controller.h"

#include "core/Debug.h"
#include "model/FileManager.h"
#include "core/utils/StringBuilder.h"
#include "core/fs/FileSystem.h"
#include "core/fs/FileWriter.h"
#include "core/fs/FileReader.h"

#include "os/os.h"

LaunchpadProMk3Controller::LaunchpadProMk3Controller(ControllerManager &manager, Model &model, Engine &engine, const ControllerInfo &info) :
    Controller(manager, model, engine),
    _project(model.project())
{
    _device = _deviceContainer.create<LaunchpadProMk3Device>();

    _device->setSendMidiHandler([this] (uint8_t cable, const MidiMessage &message) {
        return sendMidi(cable, message);
    });
    
    _device->setButtonHandler([this] (uint8_t index, uint8_t velocity, bool isAftertouch) {
        /*
        logData[logLength++] = index;
        logData[logLength++] = state;
        if (logLength >= maxLogLen) {
            logLength = 0;
            fs::FileWriter fileWriter("btnlog.bin");
            if (fileWriter.error() != fs::OK) {
                return;
            }
            fileWriter.write(&logData, sizeof(logData));
            fileWriter.finish();
        }//*/

        // DBG("button %d/%d - %d", row, col, state);
        if (isAftertouch) {
            buttonPressure(index, velocity);
        } else if (velocity != 0) {
            buttonDown(index, velocity);
        } else {
            buttonUp(index);
        }
    });

    _device->initialize();
}


LaunchpadProMk3Controller::~LaunchpadProMk3Controller() {
}

void LaunchpadProMk3Controller::update() {
    _device->clearLeds();

    for (uint8_t i = 0; i < _device->ButtonCount; i++) {
        uint8_t bs = _device->getButtonState(i);
        if (bs != 0) {
            _device->setLed(i, ColorSpec(ColorSpec::ColorType::Static, bs));
        }
        else {
            _device->setLed(i, ColorSpec(ColorSpec::ColorType::Static, 1));
        }
    }

    _device->syncLeds();
}

void LaunchpadProMk3Controller::recvMidi(uint8_t cable, const MidiMessage &message) {
    _device->recvMidi(cable, message);
}

void LaunchpadProMk3Controller::buttonDown(uint8_t index, uint8_t velocity) {
    
}

void LaunchpadProMk3Controller::buttonUp(uint8_t index) {
    
}

void LaunchpadProMk3Controller::buttonPressure(uint8_t index, uint8_t pressure) {
    
}