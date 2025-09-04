
import time
import random
import csv


try:
    volatility = float(input("Enter market volatility (e.g., 0.05 for low, 0.2 for high): "))
except:
    print("Invalid input. Using default volatility = 0.1")
    volatility = 0.1

n_ticks = 5000
initial_price = 100.0
base_time = 0  

random.seed(42)


timestamps = []
prices = []
bids = []
asks = []
volumes = []

current_time = base_time
current_price = initial_price

with open("tick_data.csv", "w", newline='') as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "price", "bid", "ask", "volume"])

    for i in range(n_ticks):
        delta_time = random.randint(50, 200)
        current_time += delta_time
        timestamps.append(current_time)

        price_change = random.gauss(0, volatility)
        current_price += price_change
        prices.append(current_price)

        spread = random.uniform(0.01, 0.05)
        bid = current_price - spread / 2
        ask = current_price + spread / 2
        bids.append(bid)
        asks.append(ask)

        volume = random.randint(1, 20)
        volumes.append(volume)

        writer.writerow([current_time, current_price, bid, ask, volume])


signals = []
window = 5
threshold = 0.2

for i in range(window, n_ticks):
    price_change = prices[i] - prices[i - window]
    if price_change > threshold:
        signals.append(timestamps[i])


latency_values = {"low_latency": 10, "high_latency": 100}
results = {"low_latency": [], "high_latency": []}

start_time = time.time()

for label in latency_values:
    latency = latency_values[label]
    for signal_time in signals:
        exec_time = signal_time + latency

        entry_price = None
        for j in range(n_ticks):
            if timestamps[j] >= exec_time:
                entry_price = prices[j]
                break

        if entry_price is None:
            continue

        exit_time = exec_time + 5000
        exit_price = None
        for j in range(n_ticks):
            if timestamps[j] >= exit_time:
                exit_price = prices[j]
                break

        if exit_price is None:
            continue

        pnl = exit_price - entry_price
        results[label].append((signal_time, exec_time, entry_price, exit_price, pnl))

end_time = time.time()
print("Python simulation took", round(end_time - start_time, 4), "seconds")


low_pnls = [x[4] for x in results['low_latency']]
high_pnls = [x[4] for x in results['high_latency']]

slippages = []
for i in range(min(len(results['low_latency']), len(results['high_latency']))):
    low_price = results['low_latency'][i][2]
    high_price = results['high_latency'][i][2]
    slippages.append(high_price - low_price)


def average(lst):
    if len(lst) == 0:
        return 0
    return sum(lst) / len(lst)

print("\n--- Over All Results ---")
print("Low Latency Avg PnL:", round(average(low_pnls), 4))
print("High Latency Avg PnL:", round(average(high_pnls), 4))
print("Avg Slippage:", round(average(slippages), 4))
print("Trades (Low Latency):", len(low_pnls))
print("Trades (High Latency):", len(high_pnls))
