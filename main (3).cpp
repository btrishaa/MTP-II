#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

using namespace std;

struct PeriodicTask {
    int id;
    float period;
    float deadline;
    vector<float> processing_time; // Processing time on each processor
    float next_release; // Next release time
    float rank; // Rank of the task
    float eft; // Earliest Finish Time
};

struct Message {
    int id;
    int source;
    int destination;
    float transmission_time;
    float est; // Earliest Start Time
    float eft; // Earliest Finish Time
};

struct Bus {
    int id;
    vector<float> costs; // Communication costs to each destination stage
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

// Function to calculate the Earliest Start Time (EST) and Earliest Finish Time (EFT) for each message
void calculateMessageSchedule(vector<Message>& messages, const vector<Bus>& buses, const vector<PeriodicTask>& tasks) {
    for (auto& message : messages) {
        // Calculate transmission time based on the selected bus
        float transmission_time = buses[0].costs[message.source - 1] + buses[0].costs[message.destination - 1];
        
        // Update EST and EFT for the message
        message.est = tasks[message.source - 1].eft; // EST is after the source task finishes
        message.eft = message.est + transmission_time;
    }
}

// Function to schedule messages
void scheduleMessages(vector<Message>& messages, const vector<Bus>& buses, const vector<PeriodicTask>& tasks) {
    // Calculate EST and EFT for each message
    calculateMessageSchedule(messages, buses, tasks);

    // Display EST and EFT for each message
    cout << endl << "EST and EFT for each message\n";
    for (const auto& message : messages) {
        cout << "Message " << message.id << " - EST: " << message.est << ", EFT: " << message.eft << endl;
    }

    // Schedule messages
    cout << endl << "Message scheduling order\n";
    for (const auto& message : messages) {
        cout << "Message " << message.id << " scheduled from Stage " << message.source << " to Stage " << message.destination << " on Bus " << buses[0].id << " from time " << message.est << " to " << message.eft << endl;
    }
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

    // Display ranks calculated
    cout << endl << "Ranks calculated\n";
    for (auto& task : tasks) {
        // Calculate rank
        task.rank = calculateRank(task);
        cout << "Task " << task.id << ": " << task.rank << endl;
    }

    // Display EST and EFT for each task
    cout << endl << "EST and EFT for each task\n";
    for (auto& task : tasks) {
        cout << "Task " << task.id << ":\n";
        for (int i = 0; i < num_processors; ++i) {
            float est = max(task.next_release, processor_busy_until[i]);
            float eft = calculateEFT(task, i);
            task.eft = eft;
            cout << "Processor " << i << " - EST: " << est << ", EFT: " << eft << endl;
        }
    }

    // Schedule tasks
    cout << endl << "Task scheduling order\n";
    for (auto& task : tasks) {
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

        cout << "Task " << task.id << " scheduled on Processor " << selected_processor+1 << " from time " << start_time << " to " << end_time << endl;
    }
}

int main() {
    // Define input data for periodic tasks
    vector<PeriodicTask> periodic_tasks = {
        {1, 5, 5, {2, 3}, 0},  // Example periodic task with period 5, deadline 5, and processing times [2, 3]
        {2, 7, 7, {3, 4}, 0},  // Another example periodic task
        // Add more periodic tasks here
    };

    // Define input data for buses
    vector<Bus> buses = {
        {1, {1, 2}},  // Example bus with communication costs to each destination stage
        // Add more buses here
    };

    // Define input data for messages
    vector<Message> messages = {
        {1, 1, 2, 0}, // Message from stage 1 to stage 2 with transmission time 0
        {2, 2, 3, 0}, // Message from stage 2 to stage 3 with transmission time 0
        {3, 1, 3, 0}, // Additional message for testing
        {4, 2, 1, 0}, // Additional message for testing
        // Add more messages here
    };

    int num_processors = 2; // Number of processors

    // Schedule periodic tasks
    schedulePeriodicTasks(periodic_tasks, num_processors);

    // Schedule messages
    scheduleMessages(messages, buses, periodic_tasks);

    return 0;
}

