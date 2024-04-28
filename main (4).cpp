#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <set>

using namespace std;

// Signal structure
struct Signal {
    int id;                 // Signal ID
    int size;               // Signal size in bytes
    int period;             // Periodicity of the signal in milliseconds

    Signal(int i, int s, int p) : id(i), size(s), period(p) {}
};

// Function to calculate the greatest common divisor (GCD)
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Function to check if two numbers are coprime
bool isCoprime(int a, int b) {
    return gcd(a, b) == 1;
}

// Function to calculate the number of coprime periodicities
int countCoprimePeriodicities(const vector<int> &periods) {
    int count = 0;
    for (size_t i = 0; i < periods.size(); ++i) {
        for (size_t j = i + 1; j < periods.size(); ++j) {
            if (isCoprime(periods[i], periods[j])) {
                count++;
            }
        }
    }
    return count;
}

// Function to find the final minimum number of slots
int findMinSlots(int Tc, int max_sl_s, const vector<int> &periods) {
    // Step 3: Determine the maximum slot size (max aggregate message size)
    int min_sl_s2 = Tc / max_sl_s;

    // Step 4: Minimum number of slots = sum of co-prime periodicities
    int min_slots = countCoprimePeriodicities(periods);

    // Step 5: Final minimum number of slots
    return max(min_sl_s2, min_slots);
}

// Function to find the final optimal slot size
int findOptimalSlotSize(const vector<Signal> &signals, int Tc, int max_sl_s) {
    // Step 2: Determine the minimum slot size (max signal size)
    int min_sl_s1 = 0;
    for (const Signal &signal : signals) {
        min_sl_s1 = max(min_sl_s1, signal.size);
    }

    // Step 6: Find optimal slot size
    int min_slots = findMinSlots(Tc, max_sl_s, {signals[0].period});
    int max_slots = Tc / min_sl_s1;
    int optimal_slot_size = 0;
    int max_utilization = 0;

    for (int k = min_slots; k <= max_slots; ++k) {
        // Combine signals into message bins for each periodicity
        unordered_map<int, vector<Signal>> message_bins; // Message bins keyed by periodicity
        for (const Signal &signal : signals) {
            if (k * signal.size <= Tc && k >= signal.size) {
                message_bins[signal.period].push_back(signal);
            }
        }

        // Combine signals within each message bin to minimize empty space
        // This is a simplified implementation, more complex algorithms can be used for better optimization
        int total_utilization = 0;
        for (const auto &bin : message_bins) {
            int total_size = accumulate(bin.second.begin(), bin.second.end(), 0, [](int sum, const Signal &signal) { return sum + signal.size; });
            int utilization = ceil((double)total_size / k);
            total_utilization += utilization;
        }

        // Update optimal slot size based on maximum utilization
        if (total_utilization > max_utilization) {
            optimal_slot_size = Tc / k;
            max_utilization = total_utilization;
        }
    }

    return optimal_slot_size;
}

// Function to display the final message sets
void displayMessageSets(const vector<Signal> &signals, int optimal_slot_size) {
    // Combine signals into message bins for each periodicity
    unordered_map<int, vector<Signal>> message_bins; // Message bins keyed by periodicity
    for (const Signal &signal : signals) {
        if (optimal_slot_size >= signal.size) {
            message_bins[signal.period].push_back(signal);
        }
    }

    // Display the final message sets
    cout << "Final message sets framed by combining signals:" << endl;
    int num_messages = 0;
    for (const auto &bin : message_bins) {
        set<int> used_signals; // Keep track of used signals to avoid duplicates
        cout << "Periodicity: " << bin.first << "ms, Messages: " << endl;
        for (const Signal &signal : bin.second) {
            if (used_signals.find(signal.id) == used_signals.end()) {
                cout << "Message " << ++num_messages << ": Signal " << signal.id << " ";
                used_signals.insert(signal.id);
            }
        }
        cout << endl;
    }
    cout << "Number of messages created: " << num_messages << endl;
}

int main() {
    // Example signals
    vector<Signal> signals = {
        Signal(1, 2, 10),  // Signal ID 1, Size 2 bytes, Periodicity 10ms
        Signal(2, 3, 5),   // Signal ID 2, Size 3 bytes, Periodicity 5ms
        Signal(3, 1, 20),  // Signal ID 3, Size 1 byte, Periodicity 20ms
        Signal(4, 2, 10),  // Signal ID 4, Size 2 bytes, Periodicity 10ms
        Signal(5, 3, 5),   // Signal ID 5, Size 3 bytes, Periodicity 5ms
        Signal(6, 1, 20)   // Signal ID 6, Size 1 byte, Periodicity 20ms
    };

    // Step 1: Calculate the Total cycle length (Tc)
    vector<int> periods;
    for (const Signal &signal : signals) {
        periods.push_back(signal.period);
    }
    int Tc = accumulate(periods.begin(), periods.end(), periods[0], gcd);
    cout << "Total cycle length (Tc): " << Tc << " milliseconds" << endl;

    // Step 3: Determine the maximum slot size (max aggregate message size)
    int max_sl_s = 0;
    for (int i = 1; i <= Tc; ++i) {
        int aggregate_size = 0;
        for (const Signal &signal : signals) {
            if (i % signal.period == 0) {
                aggregate_size += signal.size;
            }
        }
        max_sl_s = max(max_sl_s, aggregate_size);
    }

    // Step 6: Find final optimal slot size
    int optimal_slot_size = findOptimalSlotSize(signals, Tc, max_sl_s);
    cout << "Final optimal slot size: " << optimal_slot_size << " bytes" << endl;

    // Step 7: Display final message sets framed by combining signals
    displayMessageSets(signals, optimal_slot_size);

    return 0;
}

