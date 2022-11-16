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
        else if (i % 10 == 0 || i % 10 == 9 || i / 10 == 0 || i / 10 >= 9) {
            _device->setLed(i, ColorSpec(ColorSpec::ColorType::Static, 1));
        }
    }

    switch(_project.selectedTrack().trackMode()) {
        case Track::TrackMode::Note: {
            const auto &trackEngine = _engine.selectedTrackEngine().as<NoteTrackEngine>();
            const auto &sequence = _project.selectedNoteSequence();
            auto layer = _project.selectedNoteSequenceLayer();
            int currentStep = trackEngine.isActiveSequence(sequence) ? trackEngine.currentStep() : -1;
            for (uint8_t stepIndex = sequence.firstStep(); stepIndex <= sequence.lastStep(); stepIndex++) {
                const auto &step = sequence.step(stepIndex);
                _device->setLed(getStepPadIndex(stepIndex), stepIndex == currentStep
                    ? ColorSpec(ColorSpec::ColorType::Pulse, 0x28)
                    : ColorSpec(ColorSpec::ColorType::Static, step.layerValue(layer) != 0 ? 0x29 : 0x2b));
            }
            break;
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