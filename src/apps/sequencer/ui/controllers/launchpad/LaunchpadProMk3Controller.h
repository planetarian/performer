#pragma once

#include "LaunchpadProMk3Device.h"
#include "LaunchpadController.h" // import some helper objects


#include "ui/Controller.h"

#include "core/utils/Container.h"


class LaunchpadProMk3Controller : public Controller {
public:
    LaunchpadProMk3Controller(ControllerManager &manager, Model &model, Engine &engine, const ControllerInfo &info);
    virtual ~LaunchpadProMk3Controller();

    const char * getName() override {
        if (_device == nullptr) {
            return "Unprepared Launchpad Pro Mk3";
        }
        return _device->getName();
    }

    virtual void update() override;

    virtual void recvMidi(uint8_t cable, const MidiMessage &message) override;
private:
    using ColorSpec = LaunchpadProMk3Device::ColorSpec;

    Project &_project;
    Container<LaunchpadProMk3Device> _deviceContainer;
    LaunchpadProMk3Device *_device;
    static constexpr uint8_t maxLogLen = 128;
    uint8_t logData[maxLogLen] = {0};
    uint8_t logLength = 0;

    void buttonUp(uint8_t index);
    void buttonDown(uint8_t index, uint8_t velocity);
    void buttonPressure(uint8_t index, uint8_t velocity);
};
