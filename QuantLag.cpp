#include <iostream>
#include <vector>
using namespace std;

int main() {
    cout << "QuantLag – Simple Latency Simulation\n";

    vector<float> prices = {100, 101, 102, 103, 104, 105, 106};
    int n = prices.size();

    int low_latency = 10;
    int high_latency = 100;

    vector<float> pnl_low;
    vector<float> pnl_high;

    for (int i = 0; i < n - 2; i++) {
        float entry_low = prices[i + 1];
        float entry_high = prices[i + 2];

        float exit_low = prices[i + 2];
        float exit_high = prices[i + 3];

        pnl_low.push_back(exit_low - entry_low);
        pnl_high.push_back(exit_high - entry_high);
    }

    float avg_low = 0, avg_high = 0, slip = 0;
    for (int i = 0; i < pnl_low.size(); i++) {
        avg_low += pnl_low[i];
        avg_high += pnl_high[i];
        slip += (pnl_low[i] - pnl_high[i]);
    }

    avg_low /= pnl_low.size();
    avg_high /= pnl_high.size();
    slip /= pnl_low.size();

    cout << "\n--- RESULT ---\n";
    cout << "Avg PnL Low Latency: " << avg_low << endl;
    cout << "Avg PnL High Latency: " << avg_high << endl;
    cout << "Average Slippage: " << slip << endl;

    return 0;
}
