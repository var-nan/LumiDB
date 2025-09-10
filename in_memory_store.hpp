#include "kvstore.hpp"
#include <unordered_map>
#include <string>

using namespace std;

class InMemoryStore : public KV_Store{
private:
    std::unordered_map<std::string, std::string> db;

public:
    void insert_record(std::string key, std::string value) override {
        db.insert({key,value});
    }

    std::string get_value(std::string key) override {
        auto pos = db.find(key);
        if (pos != db.end()) return pos->second;
        else return "";
    }

    void delete_key(std::string key) override {
        auto pos = db.find(key);
        if (pos != db.end()) db.erase(pos);
    }
};

