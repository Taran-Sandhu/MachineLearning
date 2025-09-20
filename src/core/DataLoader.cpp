#include "core/DataLoader.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>
#include <vector>
#include <iomanip>
#include <ctime>

using namespace std;

chrono::system_clock::time_point CSVLoader::parse_time(const string& ts) const {
    if (all_of(ts.begin(), ts.end(), ::isdigit)) {
        long long secs = stoll(ts);
        return chrono::system_clock::time_point{chrono::seconds{secs}};
    }

    std::tm t = {};
    istringstream ss(ts);
    ss >> get_time(&t, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        throw runtime_error("Failed to parse timestamp: " + ts);
    }

#ifdef _WIN32

    return chrono::system_clock::from_time_t(_mkgmtime(&t));
#else

    return chrono::system_clock::from_time_t(timegm(&t));
#endif
}

vector<Bar> CSVLoader::load_file(const string& path, const string& symbol) {
    ifstream file(path);
    if (!file.is_open()) throw runtime_error("Could not open " + path);

    vector<Bar> bars;
    string line;
    bool header_skipped = false;

    while (getline(file, line)) {
        if (!header_skipped) {
            header_skipped = true;
            continue;
        }
        stringstream ss(line);
        string ts, o, h, l, c, v;
        getline(ss, ts, delimiter);
        getline(ss, o, delimiter);
        getline(ss, h, delimiter);
        getline(ss, l, delimiter);
        getline(ss, c, delimiter);
        getline(ss, v, delimiter);

        Bar b;
        b.timestamp = parse_time(ts);
        b.open   = stod(o);
        b.high   = stod(h);
        b.low    = stod(l);
        b.close  = stod(c);
        b.volume = stod(v);
        b.symbol = symbol.empty() ? path : symbol;

        bars.push_back(b);
    }

    return bars;
}

vector<Bar> CSVLoader::load_dir(const string& dirpath) {
    vector<Bar> all_bars;
    for (const auto& entry : filesystem::directory_iterator(dirpath)) {
        if (entry.path().extension() == ".csv") {
            string symbol = entry.path().stem().string();
            auto bars = load_file(entry.path().string(), symbol);
            all_bars.insert(all_bars.end(), bars.begin(), bars.end());
        }
    }
    return all_bars;
}
