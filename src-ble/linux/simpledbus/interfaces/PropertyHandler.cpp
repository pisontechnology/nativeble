#include "PropertyHandler.h"

using namespace SimpleDBus::Interfaces;

void PropertyHandler::set_options(SimpleDBus::Holder changed_properties) {
    this->set_options(changed_properties, SimpleDBus::Holder());
}

void PropertyHandler::set_options(SimpleDBus::Holder changed_properties, SimpleDBus::Holder invalidated_properties) {
    auto changed_options = changed_properties.get_dict();
    for (auto& [name, value] : changed_options) {
        this->add_option(name, value);
    }

    auto removed_options = invalidated_properties.get_array();
    for (auto& removed_option : removed_options) {
        this->remove_option(removed_option.get_string());
    }
}