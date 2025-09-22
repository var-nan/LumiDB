#include "ss_table.h"

#include <fstream>
#include <vector>

class Levels{
public:
    Levels(const std::string& filename): in{filename, std::ios::in}{
        // recover
        recover();
    }

private:
    std::vector<std::vector<SSTable>> levels;
    std::ifstream in;

    void recover(){
        // open the file and recover filenames.
        size_t n_levels;
        in >> n_levels;

        for (size_t i = 0; i < n_levels; i++){
            // read files in this level
            size_t n_files;
            in >> n_files;

            for (size_t j = 0; j < n_files; j++){
                std::string filename;
                in >> filename;
                levels[i].emplace_back(filename);
            }
        }

        std::cout << "Files are recovered" << std::endl;
    }
};