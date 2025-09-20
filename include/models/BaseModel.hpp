#pragma once

#include <vector>
#include "core/FeatureEngine.hpp"

class BaseModel {
public:
    virtual ~BaseModel() = default;
    virtual void fit(const std::vector<FeatureRow>& X,
                     const std::vector<double>& y) = 0;
    virtual double predict(const FeatureRow& x) const = 0;

    std::vector<double> predict(const std::vector<FeatureRow>& X) const {
        std::vector<double> out;
        out.reserve(X.size());
        for (const auto& row : X) {
            out.push_back(predict(row));
        }
        return out;
    }
};
