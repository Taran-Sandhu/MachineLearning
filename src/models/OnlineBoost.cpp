#include "models/OnlineBoost.hpp"

OnlineBoost::OnlineBoost(int n_learners, double lr, double shrink)
    : learners(), shrinkage(shrink) {
    learners.reserve(n_learners);
    for (int i = 0; i < n_learners; ++i) {
        learners.emplace_back(lr);
    }
}

void OnlineBoost::fit(const std::vector<FeatureRow>& X,
                      const std::vector<double>& y) {
    std::vector<double> residual = y;
    for (auto& lm : learners) {
        lm.fit(X, residual);
        auto preds = lm.predict(X);
        for (std::size_t i = 0; i < residual.size(); ++i) {
            residual[i] -= shrinkage * preds[i];
        }
    }
}

double OnlineBoost::predict(const FeatureRow& x) const {
    double out = 0.0;
    for (const auto& lm : learners) {
        out += shrinkage * lm.predict(x);
    }
    return out;
}
