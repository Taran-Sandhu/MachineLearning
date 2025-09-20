#include "models/RegimeSwitch.hpp"
#include <cmath>

RegimeSwitch::RegimeSwitch(std::unique_ptr<BaseModel> bull,
                           std::unique_ptr<BaseModel> bear,
                           double thresh)
    : bull_model(std::move(bull)),
      bear_model(std::move(bear)),
      threshold(thresh) {}

void RegimeSwitch::fit(const std::vector<FeatureRow>& X,
                       const std::vector<double>& y) {
    std::vector<FeatureRow> bull_X, bear_X;
    std::vector<double> bull_y, bear_y;

    for (std::size_t i = 0; i < X.size(); ++i) {
        auto it = X[i].values.find("rsi14");
        double val = (it != X[i].values.end() && !std::isnan(it->second))
                         ? it->second
                         : 50.0;
        if (val >= threshold) {
            bull_X.push_back(X[i]);
            bull_y.push_back(y[i]);
        } else {
            bear_X.push_back(X[i]);
            bear_y.push_back(y[i]);
        }
    }

    if (bull_model && !bull_X.empty()) {
        bull_model->fit(bull_X, bull_y);
    }
    if (bear_model && !bear_X.empty()) {
        bear_model->fit(bear_X, bear_y);
    }
}

double RegimeSwitch::predict(const FeatureRow& x) const {
    auto it = x.values.find("rsi14");
    double val = (it != x.values.end() && !std::isnan(it->second)) ? it->second : 50.0;
    if (val >= threshold) {
        return bull_model ? bull_model->predict(x) : 0.0;
    }
    return bear_model ? bear_model->predict(x) : 0.0;
}
