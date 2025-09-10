#include "logger.hpp"

void LogWriter::append(const LogEntry& log_entry){
    /*
        std::string entry;
        if (log_entry.entry_type == LogEntryType::PUT) {
            entry  = "PUT:" + log_entry.key + ":" + log_entry.value;
        }
        else {
            entry = "DELETE:" + log_entry.key;
        }

        std::vector<char> buffer(entry.begin(), entry.end());
        uint64_t crccode = crc32(0, Z_NULL, 0);
        crccode = crc32(crccode, reinterpret_cast<const Bytef *>(buffer.data()), buffer.size());
        out << entry << ':' << crccode << std::endl;

    */
    /*
        record format: crc<8> key_size<4> val_size<4> type<1> key<x> val<y>
    */

    size_t crccode = crc32(0, Z_NULL, 0);
    uint32_t key_size = log_entry.key.size(), val_size = log_entry.value.size();
    size_t buff_size = sizeof(log_entry.entry_type) + sizeof(key_size) + sizeof (val_size) 
                        + sizeof(crccode) + key_size + val_size;

    char *buffer = new char[buff_size];

    size_t offset = sizeof(crccode); // crccode at first k bytes.
    std::memcpy(buffer + offset, reinterpret_cast<const char *>(&key_size), sizeof(key_size));
    offset += sizeof(key_size);
    std::memcpy(buffer + offset, reinterpret_cast<const char *>(&val_size), sizeof(val_size));
    offset += sizeof(val_size);
    std::memcpy(buffer + offset, reinterpret_cast<const char *>(&log_entry.entry_type), sizeof(LogEntryType));
    offset += sizeof(LogEntryType);
    std::memcpy(buffer + offset, reinterpret_cast<const char *>(log_entry.key.data()), key_size);
    offset += key_size;
    if (log_entry.entry_type == LogEntryType::PUT)
        std::memcpy(buffer + offset, reinterpret_cast<const char *>(log_entry.value.data()), val_size);

    // compute cyclic redundancy code for (record type, key, value);
    offset  = sizeof(crccode) + sizeof(key_size) + sizeof(val_size);
    crccode = crc32(crccode, reinterpret_cast<const Bytef *>(buffer + offset), sizeof(LogEntryType) + key_size + val_size); 
    std::memcpy(buffer, reinterpret_cast<const char *>(&crccode), sizeof(crccode));

    out.write(buffer, buff_size);
    // flush here?
    delete[] buffer;
    std::cout << "added record to log" << std::endl;
}

void LogWriter::flush(){
    out.flush();
}

LogEntry LogReader::next(){

    size_t crccode;
    uint32_t key_size, val_size;
    LogEntryType type;
    std::string key, value;
    size_t buff_size = sizeof(crccode) + sizeof(key) + sizeof(value) + sizeof(type);

    in.read(record_buffer, buff_size);

    const void *buff_ptr = record_buffer;
    std::memcpy(&crccode, buff_ptr, sizeof(crccode));       buff_ptr += sizeof(crccode);
    std::memcpy(&key_size, buff_ptr, sizeof(key_size));     buff_ptr += sizeof(key_size);
    std::memcpy(&val_size, buff_ptr, sizeof(val_size));     buff_ptr += sizeof(val_size);
    std::memcpy(&type, buff_ptr, sizeof(type));

    // read key and value
    in.read(record_buffer, key_size + val_size); 
    buff_ptr = record_buffer;

    key.resize(key_size);
    value.resize(val_size);

    std::memcpy(key.data(), buff_ptr, key_size); buff_ptr += key_size;

    if (type == LogEntryType::PUT){
        std::memcpy(value.data(), buff_ptr, val_size);
    }

    // TODO:: compute crc code and validate.

    return {type, key, value};
    // std::string line;
    // std::getline(in, line);
    // std::stringstream ss(line);
    
    // std::string type;
    // std::getline(ss,type,':');
    // // TODO:: validate the crc codes after reading.
    // if (type == "PUT"){
    //     std::string key, value, crccode;
    //     std::getline(ss, key,':');
    //     std::getline(ss,value,':');
    //     std::getline(ss, crccode, ':');
    //     std::cout << "Insert record: " << key << " : " << value << " : " << crccode<< std::endl;
    //     return {LogEntryType::PUT,key,value};
    // }
    // else {
    //     std::string key, crccode;
    //     std::getline(ss,key,':');
    //     std::getline(ss, crccode, ':');
    //     std::cout << "Delete record: " << key << " : " << crccode << std::endl;
    //     return {LogEntryType::DELETE, key, ""};
    // }
}

bool LogReader::hasNext() {
    return !in.eof();
}

