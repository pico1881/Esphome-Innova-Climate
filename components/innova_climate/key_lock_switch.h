#pragma once

#include "esphome/components/switch/switch.h"
#include "innova.h"

namespace esphome {
namespace innova {

class LockSwitch : public switch_::Switch, public Parented<Innova> {

protected:
  void write_state(bool state) override
         {
            parent_->setLock(state);
         };
};

}  // namespace innova
}  // namespace esphome
