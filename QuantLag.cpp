#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <tuple>
#include <string>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

float average(const vector<float>& lst) {
    if (lst.empty()) return 0.0;
    float sum = 0.0;
    for (float v : lst) sum += v;
    return sum / lst.size();
}

int main() {
    vector<int> timestamps;
    vector<float> prices;


    ifstream file("tick_data.csv");
    if (!file.is_open()) {
        cerr << "Failed to open tick_data.csv" << endl;
        return 1;
    }

    string line;
    getline(file, line);  // i am skiping header here
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        getline(ss, token, ',');
        int timestamp = stoi(token);
        timestamps.push_back(timestamp);

        getline(ss, token, ',');
        float price = stof(token);
        prices.push_back(price);

        // i am sking bid , ask price as they are not needed
    }
    file.close();

    int n_ticks = timestamps.size();

    
    vector<int> signals;
    int window = 5;
    float threshold = 0.2;
    for (int i = window; i < n_ticks; ++i) {
        float price_change = prices[i] - prices[i - window];
        if (price_change > threshold) {
            signals.push_back(timestamps[i]);
        }
    }

    
    vector<string> latency_labels = {"low_latency", "high_latency"};
    vector<int> latencies = {10, 100};
    vector<vector<tuple<int, int, float, float, float>>> results(2);

    auto sim_start = high_resolution_clock::now();

    for (int l = 0; l < 2; ++l) {
        int latency = latencies[l];
        for (int signal_time : signals) {
            int exec_time = signal_time + latency;
            float entry_price = -1, exit_price = -1;

            for (int i = 0; i < n_ticks; ++i) {
                if (timestamps[i] >= exec_time) {
                    entry_price = prices[i];
                    break;
                }
            }
            if (entry_price < 0) continue;

            int exit_time = exec_time + 5000;
            for (int i = 0; i < n_ticks; ++i) {
                if (timestamps[i] >= exit_time) {
                    exit_price = prices[i];
                    break;
                }
            }
            if (exit_price < 0) continue;

            float pnl = exit_price - entry_price;
            results[l].push_back(make_tuple(signal_time, exec_time, entry_price, exit_price, pnl));
        }
    }

    auto sim_end = high_resolution_clock::now();
    duration<double> sim_time = sim_end - sim_start;
    cout << "C++ simulation took " << sim_time.count() << " seconds\n";

    
    vector<float> low_pnls, high_pnls, slippages;
    for (const auto& r : results[0]) low_pnls.push_back(get<4>(r));
    for (const auto& r : results[1]) high_pnls.push_back(get<4>(r));

    size_t trade_count = min(results[0].size(), results[1].size());
    for (size_t i = 0; i < trade_count; ++i) {
        float low_price = get<2>(results[0][i]);
        float high_price = get<2>(results[1][i]);
        slippages.push_back(high_price - low_price);
    }

    cout << "\n--- Summary ---" << endl;
    cout << "Low Latency Avg PnL: " << average(low_pnls) << endl;
    cout << "High Latency Avg PnL: " << average(high_pnls) << endl;
    cout << "Avg Slippage: " << average(slippages) << endl;
    cout << "Trades (Low Latency): " << low_pnls.size() << endl;
    cout << "Trades (High Latency): " << high_pnls.size() << endl;

    return 0;
}