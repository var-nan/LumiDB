#include <iostream>
#include <map>
#include <fstream>
#include <utility>
#include <string>

#define BYTES_LIMIT 1024

class MemTable{
public:
    MemTable() = default;

    void insert(const std::string& key, const std::string& val) {
        table.insert({key, {val, false}});
        size += key.size() + val.size();
    }

    void remove(const std::string& key){
        table[key].second = true;
        size += key.size(); // if too many deletes are present?
    }

    void flush_to_disk(const std::string& filename) {
        std::ofstream out{filename, std::ios::out};

        out << "Size: " << table.size() << std::endl;
        for (const auto& [k,v] : table){
            if (v.second) out << "delete:" << k << std::endl;
            else out << "insert:" << k << ':' << v.first <<std::endl;
        }
        table.clear();
        out.flush();
        out.close();
    }

    /**
     * Scans the entire mem table and returns a pair (val, bool). If true, the key exist in the
     * table and if val is empty, then it is also marked val is marked deleted. If false, 
     * the key doesn't exist in the table and might present in the lower levels.
     */
    std::pair<std::string, bool> get(const std::string& key){
        auto pos = table.find(key);
        if (pos != table.end()) 
            return (pos->second.second) ? std::make_pair(std::string{}, true) 
                                        : std::make_pair(pos->second.first, true);
        return std::make_pair(std::string{}, false); 

    }

    // TODO:: move c-tor and d-tor. useful when using threads.

private:
    std::map<std::string, std::pair<std::string, bool>> table;
    size_t size;

    // TODO: declare mutex.

};