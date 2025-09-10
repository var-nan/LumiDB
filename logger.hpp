#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <zlib.h>
#include <vector>
#include <stdint.h>
#include <cstring>


enum class LogEntryType {
    PUT,
    DELETE
};

typedef struct {
    LogEntryType entry_type;
    std::string key;
    std::string value;
} LogEntry;


class LogWriter {
private:
    std::ofstream out;

public:
    LogWriter(const std::string& filename): out{filename, std::ios::out | std::ios::app | std::ios::binary}{
        // out.open(filename, std::ios::app | std::ios::out);
        std::cout << "Log file: " << filename << " opened." << std::endl;
    }

    void append(const LogEntry& log_entry);

    void flush(); // flush write buffer to disk periodically (ideally after every append).

    ~LogWriter() {
        out.close();
    } // clear file contents ?
};

class LogReader {
private:
    std::ifstream in;
    char record_buffer[1024];

public:
    LogReader(const std::string& file) : in{file, std::ios::in | std::ios::binary} {
        std::cout << "log file opened for recovery" << std::endl;
    }

    bool hasNext();

    LogEntry next();

    ~LogReader() {
        in.close();
    }

}; // TODO: implement later.

#endif