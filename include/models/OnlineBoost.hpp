#pragma once

#include "models/BaseModel.hpp"
#include "models/LinearModel.hpp"
#include <vector>

class OnlineBoost : public BaseModel {
    std::vector<LinearModel> learners;
    double shrinkage;

public:
    OnlineBoost(int n_learners = 3, double lr = 0.01, double shrink = 0.1);


    using BaseModel::predict;

    void fit(const std::vector<FeatureRow>& X,
             const std::vector<double>& y) override;

    double predict(const FeatureRow& x) const override;
};
