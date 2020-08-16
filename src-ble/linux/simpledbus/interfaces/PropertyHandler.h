#pragma once

#include "../base/Holder.h"

namespace SimpleDBus {

namespace Interfaces {

class PropertyHandler {
  protected:
    virtual void add_option(std::string option_name, Holder value) = 0;
    virtual void remove_option(std::string option_name) = 0;

  public:
    void set_options(Holder changed_properties);
    void set_options(Holder changed_properties, Holder invalidated_properties);
};
}  // namespace Interfaces

}  // namespace SimpleDBus