#pragma once

#include "core/FeatureEngine.hpp"
#include "models/BaseModel.hpp"
#include <unordered_map>
#include <vector>
#include <string>

class LinearModel : public BaseModel {
public:

    using BaseModel::predict;

    LinearModel(double lr = 0.01, int epochs = 100, double lambda = 0.0,
                double decay = 0.0, bool logistic = false)
        : learning_rate(lr),
          epochs(epochs),
          lambda(lambda),
          decay(decay),
          logistic(logistic) {}

    void fit(const std::vector<FeatureRow>& X,
             const std::vector<double>& y) override;

    double predict(const FeatureRow& x) const override;

private:
    double learning_rate;
    int epochs;
    double lambda;
    double decay;
    bool logistic;

    std::vector<std::string> feature_names;
    std::unordered_map<std::string, std::size_t> feature_index;
    std::vector<double> weights;
};
