#ifndef KV_STORE_HPP
#define KV_STORE_HPP

#include <iostream>

class KV_Store{
public:
    virtual void insert_record(std::string key, std::string value) = 0;

    virtual std::string get_value(std::string key) = 0;

    virtual void delete_key(std::string key) = 0;

};

#endif