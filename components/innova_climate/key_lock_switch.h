#pragma once

#include "esphome/components/switch/switch.h"
#include "innova_climate.h"

namespace esphome {
namespace innova {

class InnovaSwitch : public switch_::Switch, public Parented<Innova> {
public:
void dump_config(){
    LOG_SWITCH("", "Innova Switch", this);
}

protected:
  void write_state(bool state) override {
      parent_->set_key_lock(state);
  };

};

}  // namespace innova
}  // namespace esphome
