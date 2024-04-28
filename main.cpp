#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

const float CROSS_THRESHOLD = 0.3; // Cross-over threshold

int nodes, n_proc;
int p_matrix[1000][1000];
int adj[1000][1000];
int processor_assigned[1000];
float aft[1000], weight[1000][1000], rank_[1000], rank_proposed[1000], EFT[1000][1000] = {0.0}, EST[1000][1000] = {0.0}, tp[1000] = {0.0};
vector<int> ready_list;

float weight_ni(int ni);
float weight_abstract(int p);
void display();
float max_nj_succ(int ni);
void algo();
void est(int i);
int Pwik(int p);
bool sort_R(int i, int j) { return rank_proposed[i] > rank_proposed[j]; }

int main() {
    cout << "Task Scheduling For Heterogeneous Computing Systems\n";
    cout << "----------------------------------------------------------------\n\n\n";

    // Initialize input data manually
    nodes = 3;
    n_proc = 2;

    // Processing cost matrix
    weight[0][0] = 2; weight[0][1] = 4;
    weight[1][0] = 3; weight[1][1] = 5;
    weight[2][0] = 4; weight[2][1] = 6;

    // Adjacency matrix
    adj[0][0] = 1; adj[0][1] = -1; adj[0][2] = 1;
    adj[1][0] = -1; adj[1][1] = 1; adj[1][2] = -1;
    adj[2][0] = -1; adj[2][1] = -1; adj[2][2] = 1;

    // Processor matrix
    p_matrix[0][0] = 1; p_matrix[0][1] = 0;
    p_matrix[1][0] = 0; p_matrix[1][1] = 1;

    display();

    for (int i = 0; i < nodes; i++)
        rank_[i] = weight_ni(i);

    for (int i = nodes - 1; i >= 0; i--) {
        rank_proposed[i] = max_nj_succ(i) + rank_[i];
        cout << "Node[" << i + 1 << "]\t" << rank_proposed[i] << endl;
    }

    // Populate ready_list with task IDs and sort based on rank_proposed
    for (int i = 0; i < nodes; ++i) {
        ready_list.push_back(i);
    }
    sort(ready_list.begin(), ready_list.end(), sort_R);

    algo();

    // Display scheduling order
    cout << "\nTask scheduling order (based on Rank and EFT with heterogeneous processors):\n";
    for (int i = 0; i < nodes; ++i) {
        cout << "Task " << i + 1 << " with EFT " << aft[i] << " on Processor " << processor_assigned[i]+1 << endl;
    }

    return 0;
}

void algo() {
    while (!ready_list.empty()) {
        int task_id = ready_list.back();
        ready_list.pop_back();
        cout << "\tPROCESS " << task_id + 1 << endl;
        float min = numeric_limits<float>::max();
        est(task_id);
        cout << "\nEST\t";
        for (int z = 0; z < n_proc; z++)
            cout << EST[task_id][z] << "\t";
        cout << endl << "EFT\t";

        int pro = -1; // Processor to be assigned
        for (int i = 0; i < n_proc; i++) {
            EFT[task_id][i] = weight[task_id][i] + EST[task_id][i];
            cout << EFT[task_id][i] << "\t";
            if (EFT[task_id][i] <= min) {
                pro = i;
                min = EFT[task_id][i]; // Selection of min. EFT
            }
        }

        if (weight[task_id][pro] <= Pwik(task_id)) {
            processor_assigned[task_id] = pro;
            aft[task_id] = EFT[task_id][pro];
            tp[pro] = EFT[task_id][pro];
        } else {
            if ((weight_ni(task_id) / weight_abstract(task_id)) >= CROSS_THRESHOLD) {
                float max = EFT[task_id][0];
                for (int i = 0; i < n_proc; i++)
                    if (max <= EFT[task_id][i]) {
                        pro = i;
                        max = EFT[task_id][i];
                    }
                processor_assigned[task_id] = pro;
                tp[pro] = aft[task_id] = EFT[task_id][pro];
            } else {
                processor_assigned[task_id] = pro;
                aft[task_id] = EFT[task_id][pro];
                tp[pro] = EFT[task_id][pro];
            }
        }
        cout << "\nActual Finish Time:\t" << aft[task_id] << endl;
        cout << "Processor Selected:\t" << processor_assigned[task_id]+1 << endl;
        cout << "Processor State:\t" << tp[0] << "\t" << tp[1] << endl;
        cout << "\n_____________________________________________________________\n\n";
    }
}

void est(int ni) {
    float temp[1000] = {0};
    float mt;
    for (int i = 0; i < nodes; i++) {
        if (adj[i][ni] != -1) {
            for (int j = 0; j < n_proc; j++) {
                mt = aft[i] + p_matrix[processor_assigned[i]][j] * adj[i][ni];
                if (temp[j] <= mt)
                    temp[j] = mt;
            }
        }
    }
    for (int i = 0; i < n_proc; i++) {
        if (tp[i] > temp[i])
            EST[ni][i] = tp[i];
        else
            EST[ni][i] = temp[i];
    }
}

float weight_abstract(int p) {
    float min = numeric_limits<float>::max();
    float max = numeric_limits<float>::min();
    for (int i = 0; i < n_proc; i++) {
        if (min > EFT[p][i])
            min = EFT[p][i];
        if (max < EFT[p][i])
            max = EFT[p][i];
    }
    return (max - min) / (max / min);
}

float max_nj_succ(int ni) {
    float temp = 0.0;
    for (int i = 0; i < nodes; i++)
        if (adj[ni][i] != -1) {
            if ((rank_proposed[i] + adj[ni][i]) > temp)
                temp = rank_proposed[i] + adj[ni][i];
        }
    return temp;
}

float weight_ni(int ni) {
    float min = numeric_limits<float>::max();
    float max = numeric_limits<float>::min();
    for (int i = 0; i < n_proc; i++) {
        if (min > weight[ni][i])
            min = weight[ni][i];
        if (max < weight[ni][i])
            max = weight[ni][i];
    }
    return ((max - min) / (max / min));
}

void display() {
    cout << "\nNodes: " << nodes << "\tProcessor: " << n_proc << endl << endl;
    cout << "Processing Cost Matrix\n";
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < n_proc; j++)
            cout << weight[i][j] << "\t";
        cout << endl;
    }
    cout << endl << "Adj Matrix\n";
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++)
            cout << adj[i][j] << " ";
        cout << endl;
    }
    cout << endl << "Processor Matrix\n";
    for (int i = 0; i < n_proc; i++) {
        for (int j = 0; j < n_proc; j++)
            cout << p_matrix[i][j] << "\t";
        cout << endl << endl;
    }
}

int Pwik(int p) {
    int min = numeric_limits<int>::max();
    for (int i = 0; i < n_proc; i++)
        if (min > weight[p][i])
            min = weight[p][i];
    return min;
}

