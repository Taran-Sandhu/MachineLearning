#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <optional>

struct Bar {
    std::chrono::system_clock::time_point timestamp;
    double open;
    double high;
    double low;
    double close;
    double volume;
    std::string symbol;
};

class DataLoader {
public:
    virtual ~DataLoader() = default;
    virtual std::vector<Bar> load_file(const std::string& path,
                                       const std::string& symbol = "") = 0;
    virtual std::vector<Bar> load_dir(const std::string& dirpath) = 0;
};

class CSVLoader : public DataLoader {
    char delimiter;

    std::chrono::system_clock::time_point parse_time(const std::string& ts) const;

public:
    explicit CSVLoader(char delim = ',') : delimiter(delim) {}

    std::vector<Bar> load_file(const std::string& path,
                               const std::string& symbol = "") override;

    std::vector<Bar> load_dir(const std::string& dirpath) override;
};
