#include "in_memory_store_wal.h"

InMemoryStore_Wal::InMemoryStore_Wal(const std::string& filename): 
    log_filename{filename}, log_writer{log_filename}{

    recover(); // recover from crash.
}

void InMemoryStore_Wal::insert_record(std::string key, std::string value) {
    // write to log 
    log_writer.append({LogEntryType::PUT, key, value});
    log_writer.flush();

    im_store.insert_record(key,value);
};

std::string InMemoryStore_Wal::get_value(std::string key) {
    return im_store.get_value(key);
}

void InMemoryStore_Wal::delete_key(std::string key) {
    log_writer.append({LogEntryType::DELETE, key, ""});
    log_writer.flush();

    im_store.delete_key(key);
}

void InMemoryStore_Wal::recover() {
    LogReader log_reader{log_filename};
    std::cout << "Opening log file for recovery." << std::endl;
    while(log_reader.hasNext()){
        LogEntry entry = log_reader.next();
        if (entry.entry_type == LogEntryType::PUT) {
            im_store.insert_record(entry.key, entry.value);
        }
        else im_store.delete_key(entry.key);
    }

}