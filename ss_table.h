#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>

class SSTable{
public:
    SSTable(const std::string& filename): in {filename, std::ios::in}{

    }

    std::pair<std::string, bool> search(const std::string& key){
        // linear scan for now.
        // reset file offset.
        size_t n_entries;
        in >> n_entries;

        for (size_t i = 0; i < n_entries; i++){
            std::string line;
            std:getline(in, line);

            std::stringstream ss(line);
            std::string record_type, k, val;
            std::getline(ss, record_type, ':');
            std::getline(ss, k, ':'); 

            if (k == key) {
                if (record_type == "insert") {
                    std::getline(ss, val, ':');
                    return {val, true};
                }
                else return {std::string{}, true};
            }
        }
        return {std::string{}, false};
    }

private:
    std::ifstream in;

    // define bloom filter later.

};


SSTable compact (const SSTable& table1, const SSTable& table2){
    // performs three way merge compaction and returns a new table.
}