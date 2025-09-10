#ifndef IN_MEMORY_STORE_WAL_H
#define IN_MEMORY_STORE_WAL_H

#include "kvstore.hpp"
#include "in_memory_store.hpp"
#include "logger.hpp"
#include <string>

#include <iostream>

class InMemoryStore_Wal : public KV_Store {
private:
    // NOTE:: In constructor, the variables are initialized as per order of definition in the class variable.
    InMemoryStore im_store;
    std::string log_filename;
    LogWriter log_writer;

    void recover();

public:
    InMemoryStore_Wal(const std::string &filename);

    void insert_record(std::string key, std::string value) override;

    std::string get_value(std::string key) override;

    void delete_key(std::string key) override;
};

#endif