#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

// Define a structure for periodic tasks
struct PeriodicTask {
    int id;
    float period;
    float deadline;
    vector<float> processing_time; // Processing time on each processor
    float next_release; // Next release time
    float rank; // Rank of the task
    float eft; // Earliest Finish Time
};

// Define a structure for messages
struct Message {
    int id;
    int source; // Source stage of the message
    int destination; // Destination stage of the message
    float transmission_time; // Time taken to transmit the message
};

// Define a structure for stages in the pipeline
struct Stage {
    int id;
    vector<PeriodicTask> tasks; // Tasks assigned to this stage
};

// Define a structure for buses with heterogeneous communication times
struct Bus {
    int id;
    float communication_time; // Communication time of the bus
};

// Function to calculate the rank of a periodic task
float calculateRank(const PeriodicTask& task) {
    return task.period; // Example: Use period as rank
}

// Function to calculate the Earliest Finish Time (EFT) of a task on a processor
float calculateEFT(const PeriodicTask& task, int processor) {
    // Example: EFT is the sum of next release time and processing time on the processor
    return task.next_release + task.processing_time[processor];
}

// Function to schedule periodic tasks using Rate Monotonic scheduling
void schedulePeriodicTasks(vector<PeriodicTask>& tasks, int num_processors) {
    // Sort tasks by period (tasks with shorter periods have higher priority)
    sort(tasks.begin(), tasks.end(), [](const PeriodicTask& a, const PeriodicTask& b) {
        return a.period < b.period;
    });

    vector<float> processor_busy_until(num_processors, 0); // Track the time until each processor is busy

    cout << "Nodes: " << tasks.size() << "\tProcessor: " << num_processors << endl << endl;

    cout << "Processing Cost Matrix\n";
    for (const auto& task : tasks) {
        cout << "Task " << task.id << ": ";
        for (float time : task.processing_time) {
            cout << time << "\t";
        }
        cout << endl;
    }

    cout << endl << "Adj Matrix\n";
    for (const auto& task : tasks) {
        cout << "Task " << task.id << ": ";
        for (int i = 0; i < tasks.size(); ++i) {
            if (i == task.id - 1) {
                cout << "1\t";
            } else {
                cout << "-1\t";
            }
        }
        cout << endl;
    }

    cout << endl << "Processor Matrix\n";
    for (int i = 0; i < num_processors; ++i) {
        for (int j = 0; j < num_processors; ++j) {
            if (i == j) {
                cout << "1\t";
            } else {
                cout << "0\t";
            }
        }
        cout << endl;
    }

    cout << endl << "Ranks calculated\n";
    for (const auto& task : tasks) {
        cout << "Task " << task.id << ": " << task.rank << endl;
    }

    cout << endl << "EST and EFT for each task\n";
    for (const auto& task : tasks) {
        cout << "Task " << task.id << ":\n";
        for (int i = 0; i < num_processors; ++i) {
            float est = max(task.next_release, processor_busy_until[i]);
            float eft = calculateEFT(task, i);
            cout << "Processor " << i << " - EST: " << est << ", EFT: " << eft << endl;
        }
    }

    cout << endl << "Task scheduling order\n";

    // Schedule tasks
    for (auto& task : tasks) {
        // Calculate rank
        task.rank = calculateRank(task);

        // Calculate Earliest Finish Time (EFT) on each processor
        for (int i = 0; i < num_processors; ++i) {
            task.eft = calculateEFT(task, i);
        }

        float min_busy_until = numeric_limits<float>::max();
        int selected_processor = -1;

        // Find the earliest available processor
        for (int i = 0; i < num_processors; ++i) {
            if (processor_busy_until[i] < min_busy_until) {
                min_busy_until = processor_busy_until[i];
                selected_processor = i;
            }
        }

        // Schedule the task at or before its next release time
        float start_time = max(task.next_release, processor_busy_until[selected_processor]);
        float end_time = start_time + task.processing_time[selected_processor];

        // Update next release time
        task.next_release += task.period;

        // Update processor busy time
        processor_busy_until[selected_processor] = end_time;

        cout << "Task " << task.id << " scheduled on Processor " << selected_processor + 1 << " from time " << start_time << " to " << end_time << endl;
    }
}

// Function to schedule messages on heterogeneous buses
void scheduleMessages(vector<Message>& messages, const vector<Bus>& buses) {
    cout << endl << "Message scheduling order\n";

    // Display communication cost matrix
    cout << endl << "Communication Cost Matrix\n";
    for (const auto& bus : buses) {
        cout << "Bus " << bus.id << ": ";
        for (const auto& other_bus : buses) {
            cout << other_bus.communication_time << "\t";
        }
        cout << endl;
    }

    // Schedule messages
    for (const auto& message : messages) {
        // Find the bus with minimum communication time
        float min_communication_time = numeric_limits<float>::max();
        int selected_bus = -1;
        for (int i = 0; i < buses.size(); ++i) {
            if (buses[i].communication_time < min_communication_time) {
                min_communication_time = buses[i].communication_time;
                selected_bus = i;
            }
        }

        // Schedule the message on the selected bus
        float transmission_start_time = max(message.source, message.destination); // Adjusted to use source and destination directly
        float transmission_end_time = transmission_start_time + message.transmission_time;

        // No need to update next release time of source and destination tasks

        cout << "Message " << message.id << " scheduled from Stage " << message.source << " to Stage " << message.destination << " on Bus " << selected_bus + 1 << " from time " << transmission_start_time << " to " << transmission_end_time << endl;
    }
}

int main() {
    // Define input data for periodic tasks
    vector<PeriodicTask> periodic_tasks = {
        {1, 5, 5, {2, 3}, 0},  // Example periodic task with period 5, deadline 5, and processing times [2, 3]
        {2, 7, 7, {3, 4}, 0},  // Another example periodic task
        // Add more periodic tasks here
    };

    int num_processors = 2; // Number of processors

    // Schedule periodic tasks
    schedulePeriodicTasks(periodic_tasks, num_processors);

    // Define input data for messages and buses
    vector<Message> messages = {
        {1, 1, 2, 3}, // Example message from stage 1 to stage 2 with transmission time 3
        {2, 2, 3, 4}, // Another example message
        // Add more messages here
    };

    vector<Bus> buses = {
        {1, 3}, // Example bus with communication time 3
        {2, 5}, // Another example bus
        // Add more buses here
    };

    // Schedule messages on heterogeneous buses
    scheduleMessages(messages, buses);

    return 0;
}


