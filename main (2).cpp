#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

struct PeriodicTask {
    int id;
    float period;
    float deadline;
    vector<float> processing_time;
    float next_release;
    float rank;
    float est;
    float eft;
};

struct Message {
    int id;
    int source;
    int destination;
    float transmission_time;
    float est;
    float eft;
};

struct Bus {
    int id;
    vector<float> costs;
};

float calculateRank(const PeriodicTask& task) {
    return task.period;
}

float calculateEFT(const PeriodicTask& task, int processor) {
    return task.next_release + task.processing_time[processor];
}

void schedulePeriodicTasks(vector<PeriodicTask>& tasks, int num_processors) {
    sort(tasks.begin(), tasks.end(), [](const PeriodicTask& a, const PeriodicTask& b) {
        return a.period < b.period;
    });

    vector<float> processor_busy_until(num_processors, 0);

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
    for (auto& task : tasks) {
        task.rank = calculateRank(task);
        cout << "Task " << task.id << ": " << task.rank << endl;
    }

    cout << endl << "EST and EFT for each task\n";
    for (auto& task : tasks) {
        cout << "Task " << task.id << ":\n";
        for (int i = 0; i < num_processors; ++i) {
            task.est = max(task.next_release, processor_busy_until[i]);
            task.eft = calculateEFT(task, i);
            cout << "Processor " << i << " - EST: " << task.est << ", EFT: " << task.eft << endl;
        }
    }

    cout << endl << "Task scheduling order\n";
    
    for (auto& task : tasks) {
        task.rank = calculateRank(task);
        for (int i = 0; i < num_processors; ++i) {
            task.eft = calculateEFT(task, i);
        }

        float min_busy_until = numeric_limits<float>::max();
        int selected_processor = -1;

        for (int i = 0; i < num_processors; ++i) {
            if (processor_busy_until[i] < min_busy_until) {
                min_busy_until = processor_busy_until[i];
                selected_processor = i;
            }
        }

        float start_time = max(task.next_release, processor_busy_until[selected_processor]);
        float end_time = start_time + task.processing_time[selected_processor];
        
        task.next_release += task.period;
        processor_busy_until[selected_processor] = end_time;

        cout << "Task " << task.id << " scheduled on Processor " << selected_processor+1 << " from time " << start_time << " to " << end_time << endl;
    }
}

void scheduleMessages(vector<Message>& messages, const vector<Bus>& buses, vector<PeriodicTask>& tasks) {
    cout << endl << "Communication Cost Matrix\n";
    for (const auto& bus : buses) {
        cout << "Bus " << bus.id << ": ";
        for (float cost : bus.costs) {
            cout << cost << "\t";
        }
        cout << endl;
    }

    cout << endl << "EST and EFT for each message\n";
    for (auto& message : messages) {
        message.est = max(tasks[message.source - 1].eft, tasks[message.destination - 1].eft);
        message.eft = message.est + message.transmission_time;
        cout << "Message " << message.id << " - EST: " << message.est << ", EFT: " << message.eft << endl;
    }

    cout << endl << "Message scheduling order\n";
    for (auto& message : messages) {
        cout << "Message " << message.id << " scheduled from Stage " << message.source << " to Stage " << message.destination << " on Bus " << message.id << " from time " << message.est << " to " << message.eft << endl;
    }
}

int main() {
    vector<Message> messages = {
        {1, 1, 2, 1, 0, 0},
        {2, 2, 3, 2, 0, 0},
        {3, 1, 2, 3, 0, 0},
        {4, 2, 3, 4, 0, 0}
    };

    vector<PeriodicTask> tasks = {
        {1, 5, 5, {2, 3}, 0, 0, 0, 0},
        {2, 10, 10, {3, 4}, 0, 0, 0, 0}
    };

    vector<Bus> buses = {
        {1, {1, 2}},
        {2, {2, 3}}
    };

    scheduleMessages(messages, buses, tasks);
    schedulePeriodicTasks(tasks, 2);

    return 0;
}

