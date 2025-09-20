#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "core/DataLoader.hpp"
#include "core/FeatureEngine.hpp"
#include "models/LinearModel.hpp"

namespace py = pybind11;

double predict_from_candles(const std::vector<std::vector<double>>& candles) {
    std::size_t n = candles.size();
    if (n < 2) {
        throw std::runtime_error("Need at least 2 candles to train/predict");
    }


    std::vector<Bar> bars(n);
    for (std::size_t i = 0; i < n; ++i) {
        Bar b{};
        b.open   = candles[i][1];
        b.high   = candles[i][2];
        b.low    = candles[i][3];
        b.close  = candles[i][4];
        b.volume = candles[i][5];
        b.symbol = "";
        b.timestamp = std::chrono::system_clock::time_point{};
        bars[i] = b;
    }


    auto feats = FeatureEngine::make_features(bars);


    for (auto &row : feats) {
        for (auto &kv : row.values) {
            if (std::isnan(kv.second) || std::isinf(kv.second)) {
                kv.second = 0.0;
            }
        }
    }


    std::vector<FeatureRow> X;
    std::vector<double> y;
    for (std::size_t i = 1; i < n; ++i) {
        X.push_back(feats[i - 1]);
        y.push_back(bars[i].close);
    }

    if (X.size() < 2) {
        return bars.back().close;
    }

    LinearModel model;
    model.fit(X, y);

    double pred = model.predict(feats.back());
    if (std::isnan(pred) || std::isinf(pred)) {
        throw std::runtime_error("Model produced invalid prediction");
    }
    return pred;
}

PYBIND11_MODULE(cppmodel, m) {
    m.doc() = "Bindings for the C++ LinearModel";
    m.def("predict", &predict_from_candles, "Train on past candles and predict next close");
}
