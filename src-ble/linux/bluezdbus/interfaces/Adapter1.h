#pragma once

#include "simpledbus/SimpleDBus.h"

#include <string>

class Adapter1 : public SimpleDBus::Interfaces::PropertyHandler {
  private:
    SimpleDBus::Connection* _conn;
    std::string _path;

    bool _discovering;

    void add_option(std::string option_name, SimpleDBus::Holder value);
    void remove_option(std::string option_name);

  public:
    Adapter1(SimpleDBus::Connection* conn, std::string path);
    ~Adapter1();

    void StartDiscovery();
    void StopDiscovery();

    SimpleDBus::Holder GetDiscoveryFilters();
    void SetDiscoveryFilter(SimpleDBus::Holder properties);


    bool is_discovering();


};
