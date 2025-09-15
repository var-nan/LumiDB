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


enum class LogEntryType : uint8_t {
    PUT = 0,
    DELETE = 1
};

typedef struct {
    LogEntryType entry_type;
    std::string key;
    std::string value;
} LogEntry;

class LogWriterInterface{
public:
    virtual void append(const LogEntry& log_entry) = 0;
    virtual void flush() = 0;
};

class LogReaderInterface {
public:
    virtual bool hasNext() = 0;
    virtual LogEntry next() = 0;
};


class StringLogWriter : public LogWriterInterface {
private:
    std::ofstream out;

public:
    StringLogWriter(const std::string& filename);
    void append(const LogEntry& log_entry) override;
    void flush() override;
};


class StringLogReader : public LogReaderInterface {
private:
    std::ifstream in;

public:
    StringLogReader(const std::string& filename);
    bool hasNext() override;
    LogEntry next() override;
};



class ByteLogWriter : public LogWriterInterface{
private:
    std::ofstream out;

public:
    ByteLogWriter(const std::string& filename): out{filename, std::ios::out | std::ios::app | std::ios::binary}{
        // out.open(filename, std::ios::app | std::ios::out);
        // std::cout << "Log file: " << filename << " opened." << std::endl;
    }

    void append(const LogEntry& log_entry) override;

    void flush() override; // flush write buffer to disk periodically (ideally after every append).

    ~ByteLogWriter() {
        out.close();
    } // clear file contents ?
};

class ByteLogReader : public LogReaderInterface {
private:
    std::ifstream in;
    // char record_buffer[1024];
    size_t nread = 0;

public:
    ByteLogReader(const std::string& file) : in{file, std::ios::in | std::ios::binary} {
        // std::cout << "log file opened for recovery" << std::endl;
    }

    bool hasNext() override;

    LogEntry next() override;

    ~ByteLogReader() {
        std::cout << "Read " << nread << " bytes from log file" << std::endl;
        in.close();
    }

}; // TODO: implement later.

#endif