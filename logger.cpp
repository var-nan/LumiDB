#include "logger.hpp"

void ByteLogWriter::append(const LogEntry& log_entry){

    /*
        record format: crc<8> key_size<4> val_size<4> type<1> key<x> val<y>
    */
    size_t crccode = crc32(0, Z_NULL, 0);
    uint32_t key_size = log_entry.key.size(), val_size = log_entry.value.size();
    size_t buff_size = sizeof(crccode) + sizeof(key_size) + sizeof(val_size) 
                        + sizeof(LogEntry::entry_type) + key_size + val_size;
    char type = (log_entry.entry_type == LogEntryType::PUT) ? 0 : 1;

    char *buffer = new char[buff_size];
    char* buff_ptr = buffer + sizeof(crccode);

    std::memcpy(buff_ptr, static_cast<const void *>(&key_size), sizeof(key_size));      buff_ptr += sizeof(key_size);
    std::memcpy(buff_ptr, static_cast<const void *>(&val_size), sizeof(val_size));      buff_ptr += sizeof(val_size);
    std::memcpy(buff_ptr, static_cast<const void *>(&type), sizeof(type));              buff_ptr += sizeof(type);
    std::memcpy(buff_ptr, static_cast<const void *>(log_entry.key.data()), key_size);   buff_ptr += key_size;
    if (type == 0) std::memcpy(buff_ptr, static_cast<const void *>(log_entry.value.data()), val_size);

    // compute cyclic redundancy check code for (record.type, key,value);
    buff_ptr = buffer + (sizeof(crccode) + sizeof(key_size) + sizeof(val_size));
    crccode = crc32(crccode, reinterpret_cast<const Bytef *>(buff_ptr), key_size + val_size + sizeof(type));
    std::memcpy(buffer, static_cast<const void *>(&crccode), sizeof(crccode));
    
    // can we print the buffer?
    // std::string result (buffer, buff_size);
    // std::cout << "Contents of the buffer: " << result << std::endl;

    out.write(buffer, buff_size);
    // flush here?
    delete[] buffer;
}

void ByteLogWriter::flush(){
    out.flush();
}

LogEntry ByteLogReader::next(){

    size_t crccode;
    uint32_t key_size, val_size;
    char type;
    std::string key, value;
    size_t buff_size = sizeof(crccode) + sizeof(key) + sizeof(value) + sizeof(type);

    char *record_buffer = static_cast<char *>(malloc(buff_size));

    in.read(record_buffer, buff_size);
    nread += in.gcount();

    const char *buff_ptr = record_buffer;
    std::memcpy(&crccode, buff_ptr, sizeof(crccode));       buff_ptr += sizeof(crccode);
    std::memcpy(&key_size, buff_ptr, sizeof(key_size));     buff_ptr += sizeof(key_size);
    std::memcpy(&val_size, buff_ptr, sizeof(val_size));     buff_ptr += sizeof(val_size);
    std::memcpy(&type, buff_ptr, sizeof(type));

    // read key and value
    if ((key_size + val_size) > buff_size)
        record_buffer = static_cast<char *>(realloc(record_buffer, key_size+val_size));
    
    in.read(record_buffer, key_size + val_size); 
    nread += in.gcount();
    
    buff_ptr = record_buffer;
    key = std::string(buff_ptr, key_size);
    if (type == 0)
        value = std::string(buff_ptr + key_size, val_size);

    // TODO: compute crccode and validate the record.
    // std::cout << "recovered: " << key << " " << value << std::endl;
    LogEntryType e_type = (type == 0)? LogEntryType::PUT : LogEntryType::DELETE;
    return {e_type, key,value};
}

bool ByteLogReader::hasNext() {
    return in.peek() != EOF;
}


StringLogWriter::StringLogWriter(const std::string& filename) : out{filename, std::ios::app | std::ios::out} {

}

void StringLogWriter::append(const LogEntry& log_entry){

    // <crccode> <type> <key> <value>
    const std::string key = log_entry.key, val = log_entry.value;
    char entry_type = (log_entry.entry_type == LogEntryType::PUT) ? 'p' : 'd';
    std::string result;
    size_t crccode = crc32(0, Z_NULL, 0);
    crccode = crc32(crccode, reinterpret_cast<const Bytef *>(&entry_type), sizeof(entry_type));
    crccode = crc32(crccode, reinterpret_cast<const Bytef *>(key.data()), key.size());
    crccode = crc32(crccode, reinterpret_cast<const Bytef *>(val.data()), val.size());

    out << crccode << ':' << entry_type << ':' << key << ':' << val << std::endl;
    out.flush();
}

void StringLogWriter::flush() {
    out.flush();
}


StringLogReader::StringLogReader(const std::string& filename): in{filename, std::ios::in}{

}

LogEntry StringLogReader::next() {
    // 
    std::string line, crccode_str, entry_type, key, value;
    std::getline(in, line);
    std::stringstream ss(line);

    std::getline(ss, crccode_str, ':');
    std::getline(ss, entry_type, ':');
    std::getline(ss, key, ':');
    char type = entry_type[0];
    if (type == 'p')
        std::getline(ss, value, ':');

    size_t crccode_expected = static_cast<size_t>(std::atoll(crccode_str.data()));
    size_t crccode = crc32(0,Z_NULL, 0);
    crccode = crc32(crccode, reinterpret_cast<const Bytef *>(&type), sizeof(type));
    crccode = crc32(crccode, reinterpret_cast<const Bytef *>(key.data()), key.size());
    crccode = crc32(crccode, reinterpret_cast<const Bytef *>(value.data()), value.size());

    if (crccode != crccode_expected) {
        // exit program: either corrupted block or disk crash.
        std::cout << "crccode mismatch. " << std::endl;
    }
    return {(type == 'p')? LogEntryType::PUT : LogEntryType::DELETE , key, value};
}

bool StringLogReader::hasNext() {
    return in.peek() != EOF;
}

