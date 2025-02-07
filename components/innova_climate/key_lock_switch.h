#pragma once

#include "esphome/components/switch/switch.h"
#include "innova.h"

namespace esphome {
namespace innova {

class KeyLockSwitch : public switch_::Switch, public Parented<Innova> {

protected:
  void write_state(bool state) override
         {
            //parent_->setIon(state);
         };
};

}  // namespace innova
}  // namespace esphome
