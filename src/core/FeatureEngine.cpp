#include "core/FeatureEngine.hpp"
#include <cmath>
#include <numeric>
#include <deque>
#include <stdexcept>

static double mean(const std::deque<double>& window) {
    double sum = std::accumulate(window.begin(), window.end(), 0.0);
    return sum / window.size();
}


std::vector<double> FeatureEngine::returns(const std::vector<double>& closes, int lag) {
    std::vector<double> out(closes.size(), NAN);
    for (size_t i = lag; i < closes.size(); i++) {
        out[i] = (closes[i] - closes[i - lag]) / closes[i - lag];
    }
    return out;
}

std::vector<double> FeatureEngine::rsi(const std::vector<double>& closes, int window) {
    std::vector<double> out(closes.size(), NAN);
    double avg_gain = 0, avg_loss = 0;

    for (size_t i = 1; i < closes.size(); i++) {
        double delta = closes[i] - closes[i - 1];
        double gain = std::max(delta, 0.0);
        double loss = std::max(-delta, 0.0);

        if (i < (size_t)window) {
            avg_gain += gain;
            avg_loss += loss;
            continue;
        }

        if (i == (size_t)window) {
            avg_gain /= window;
            avg_loss /= window;
        } else {
            avg_gain = (avg_gain * (window - 1) + gain) / window;
            avg_loss = (avg_loss * (window - 1) + loss) / window;
        }

        double rs = (avg_loss == 0) ? 0 : avg_gain / avg_loss;
        out[i] = 100.0 - (100.0 / (1.0 + rs));
    }
    return out;
}

void FeatureEngine::bollinger(const std::vector<double>& closes, int window, double k,
                              std::vector<double>& mid,
                              std::vector<double>& upper,
                              std::vector<double>& lower,
                              std::vector<double>& pctb,
                              std::vector<double>& bandwidth) {
    size_t n = closes.size();
    mid.assign(n, NAN);
    upper.assign(n, NAN);
    lower.assign(n, NAN);
    pctb.assign(n, NAN);
    bandwidth.assign(n, NAN);

    std::deque<double> win;
    for (size_t i = 0; i < n; i++) {
        win.push_back(closes[i]);
        if (win.size() > (size_t)window) win.pop_front();

        if (win.size() == (size_t)window) {
            double m = mean(win);
            double sq_sum = std::inner_product(win.begin(), win.end(), win.begin(), 0.0);
            double variance = (sq_sum / window) - (m * m);
            double stddev = std::sqrt(std::max(variance, 0.0));

            mid[i] = m;
            upper[i] = m + k * stddev;
            lower[i] = m - k * stddev;
            pctb[i] = (closes[i] - lower[i]) / (upper[i] - lower[i]);
            bandwidth[i] = (upper[i] - lower[i]) / (m != 0 ? m : 1.0);
        }
    }
}

std::vector<double> FeatureEngine::realized_vol(const std::vector<double>& closes, int window) {
    std::vector<double> out(closes.size(), NAN);
    std::deque<double> win;

    for (size_t i = 1; i < closes.size(); i++) {
        double ret = (closes[i] - closes[i - 1]) / closes[i - 1];
        win.push_back(ret);
        if (win.size() > (size_t)window) win.pop_front();

        if (win.size() == (size_t)window) {
            double m = mean(win);
            double sq_sum = std::inner_product(win.begin(), win.end(), win.begin(), 0.0);
            double variance = (sq_sum / window) - (m * m);
            out[i] = std::sqrt(std::max(variance, 0.0));
        }
    }
    return out;
}

std::vector<double> FeatureEngine::range_frac(const std::vector<Bar>& bars) {
    std::vector<double> out(bars.size(), NAN);
    for (size_t i = 0; i < bars.size(); i++) {
        double rng = bars[i].high - bars[i].low;
        if (rng > 0) {
            out[i] = (bars[i].close - bars[i].low) / rng;
        }
    }
    return out;
}






std::vector<double> FeatureEngine::ema(const std::vector<double>& closes, int window) {
    std::vector<double> out(closes.size(), NAN);
    if (closes.empty()) return out;
    double alpha = 2.0 / (window + 1);
    out[0] = closes[0];
    for (size_t i = 1; i < closes.size(); i++) {
        out[i] = alpha * closes[i] + (1 - alpha) * out[i - 1];
    }
    return out;
}


std::vector<double> FeatureEngine::sma(const std::vector<double>& closes, int window) {
    std::vector<double> out(closes.size(), NAN);
    std::deque<double> win;
    for (size_t i = 0; i < closes.size(); i++) {
        win.push_back(closes[i]);
        if (win.size() > (size_t)window) win.pop_front();
        if (win.size() == (size_t)window) {
            out[i] = mean(win);
        }
    }
    return out;
}


std::vector<double> FeatureEngine::atr(const std::vector<Bar>& bars, int window) {
    std::vector<double> out(bars.size(), NAN);
    std::deque<double> win;
    for (size_t i = 1; i < bars.size(); i++) {
        double tr = std::max({bars[i].high - bars[i].low,
                              std::fabs(bars[i].high - bars[i - 1].close),
                              std::fabs(bars[i].low - bars[i - 1].close)});
        win.push_back(tr);
        if (win.size() > (size_t)window) win.pop_front();
        if (win.size() == (size_t)window) {
            out[i] = mean(win);
        }
    }
    return out;
}





std::vector<FeatureRow> FeatureEngine::make_features(const std::vector<Bar>& bars) {
    std::vector<FeatureRow> features;
    size_t n = bars.size();
    std::vector<double> closes(n);
    for (size_t i = 0; i < n; i++) closes[i] = bars[i].close;


    auto ret1  = returns(closes, 1);
    auto ret5  = returns(closes, 5);
    auto ret10 = returns(closes, 10);
    auto ret20 = returns(closes, 20);


    auto rsi7  = rsi(closes, 7);
    auto rsi14 = rsi(closes, 14);
    auto rsi28 = rsi(closes, 28);


    auto rv24 = realized_vol(closes, 24);
    auto rv48 = realized_vol(closes, 48);
    auto rv72 = realized_vol(closes, 72);


    auto rangeF = range_frac(bars);


    std::vector<double> bb20_mid, bb20_upper, bb20_lower, bb20_pctb, bb20_bw;
    bollinger(closes, 20, 2.0, bb20_mid, bb20_upper, bb20_lower, bb20_pctb, bb20_bw);


    auto sma20 = sma(closes, 20);
    auto sma50 = sma(closes, 50);
    auto ema12 = ema(closes, 12);
    auto ema26 = ema(closes, 26);


    std::vector<double> macd(n, NAN), macd_signal(n, NAN), macd_hist(n, NAN);
    for (size_t i = 0; i < n; i++) {
        if (!std::isnan(ema12[i]) && !std::isnan(ema26[i])) {
            macd[i] = ema12[i] - ema26[i];
        }
    }

    macd_signal = ema(macd, 9);
    for (size_t i = 0; i < n; i++) {
        if (!std::isnan(macd[i]) && !std::isnan(macd_signal[i])) {
            macd_hist[i] = macd[i] - macd_signal[i];
        }
    }


    auto atr14 = atr(bars, 14);


    for (size_t i = 0; i < n; i++) {
        FeatureRow row;
        row.timestamp = bars[i].timestamp;
        row.symbol = bars[i].symbol;


        row.values["ret_1"]  = ret1[i];
        row.values["ret_5"]  = ret5[i];
        row.values["ret_10"] = ret10[i];
        row.values["ret_20"] = ret20[i];


        row.values["rsi7"]  = rsi7[i];
        row.values["rsi14"] = rsi14[i];
        row.values["rsi28"] = rsi28[i];


        row.values["rv_24"] = rv24[i];
        row.values["rv_48"] = rv48[i];
        row.values["rv_72"] = rv72[i];


        row.values["range_frac"] = rangeF[i];


        row.values["bb20_mid"]   = bb20_mid[i];
        row.values["bb20_upper"] = bb20_upper[i];
        row.values["bb20_lower"] = bb20_lower[i];
        row.values["bb20_pctb"]  = bb20_pctb[i];
        row.values["bb20_bw"]    = bb20_bw[i];

        row.values["sma20"] = sma20[i];
        row.values["sma50"] = sma50[i];
        row.values["ema12"] = ema12[i];
        row.values["ema26"] = ema26[i];

        row.values["macd"]       = macd[i];
        row.values["macd_signal"]= macd_signal[i];
        row.values["macd_hist"]  = macd_hist[i];

        row.values["atr14"] = atr14[i];

        features.push_back(row);
    }

    return features;
}
