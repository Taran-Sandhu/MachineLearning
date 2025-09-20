#include "models/LinearModel.hpp"
#include <cmath>
#include <stdexcept>

void LinearModel::fit(const std::vector<FeatureRow>& X,
                      const std::vector<double>& y) {
    if (X.size() != y.size()) {
        throw std::invalid_argument("X and y must have same size");
    }


    feature_names.clear();
    feature_index.clear();
    for (const auto& row : X) {
        for (const auto& kv : row.values) {
            if (!feature_index.count(kv.first)) {
                feature_index[kv.first] = feature_names.size();
                feature_names.push_back(kv.first);
            }
        }
    }

    std::size_t p = feature_names.size();
    weights.assign(p + 1, 0.0);


    for (int epoch = 0; epoch < epochs; ++epoch) {
        double lr = learning_rate / (1.0 + decay * epoch);

        for (std::size_t i = 0; i < X.size(); ++i) {

            double linear = weights[0];
            for (const auto& kv : X[i].values) {
                auto it = feature_index.find(kv.first);
                if (it != feature_index.end() && !std::isnan(kv.second)) {
                    linear += weights[it->second + 1] * kv.second;
                }
            }


            double pred = logistic ? 1.0 / (1.0 + std::exp(-linear)) : linear;


            double error = pred - y[i];


            weights[0] -= lr * error;


            for (const auto& kv : X[i].values) {
                auto it = feature_index.find(kv.first);
                if (it != feature_index.end() && !std::isnan(kv.second)) {
                    std::size_t idx = it->second + 1;
                    weights[idx] -= lr * (error * kv.second + lambda * weights[idx]);
                }
            }
        }
    }
}

double LinearModel::predict(const FeatureRow& x) const {
    double linear = weights.empty() ? 0.0 : weights[0];
    for (const auto& kv : x.values) {
        auto it = feature_index.find(kv.first);
        if (it != feature_index.end() && !std::isnan(kv.second)) {
            linear += weights[it->second + 1] * kv.second;
        }
    }

    return logistic ? 1.0 / (1.0 + std::exp(-linear)) : linear;
}
