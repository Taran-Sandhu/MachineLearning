#pragma once

#include "core/DataLoader.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>

struct FeatureRow {
    std::chrono::system_clock::time_point timestamp;
    std::string symbol;
    std::unordered_map<std::string, double> values;
};

class FeatureEngine {
public:

    static std::vector<double> returns(const std::vector<double>& closes, int lag = 1);
    static std::vector<double> rsi(const std::vector<double>& closes, int window = 14);
    static void bollinger(const std::vector<double>& closes, int window, double k,
                          std::vector<double>& mid,
                          std::vector<double>& upper,
                          std::vector<double>& lower,
                          std::vector<double>& pctb,
                          std::vector<double>& bandwidth);
    static std::vector<double> realized_vol(const std::vector<double>& closes, int window);
    static std::vector<double> range_frac(const std::vector<Bar>& bars);


    static std::vector<double> ema(const std::vector<double>& closes, int window);
    static std::vector<double> sma(const std::vector<double>& closes, int window);
    static std::vector<double> atr(const std::vector<Bar>& bars, int window);


    static std::vector<FeatureRow> make_features(const std::vector<Bar>& bars);
};
