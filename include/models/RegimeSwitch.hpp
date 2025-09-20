#pragma once

#include "models/BaseModel.hpp"
#include <memory>

class RegimeSwitch : public BaseModel {
    std::unique_ptr<BaseModel> bull_model;
    std::unique_ptr<BaseModel> bear_model;
    double threshold;

public:
    RegimeSwitch(std::unique_ptr<BaseModel> bull,
                 std::unique_ptr<BaseModel> bear,
                 double thresh = 50.0);

    void fit(const std::vector<FeatureRow>& X,
             const std::vector<double>& y) override;

    double predict(const FeatureRow& x) const override;
};
