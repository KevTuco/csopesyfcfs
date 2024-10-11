#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>   
#include <iomanip>   
#include <ctime>  
#include <thread>

using namespace std;

struct Process {
    int pid;
    int start_time;          
    int numprocess = -1; 
    int finish = 0;
    int processing = 0;
    string starttime;
    string finishtime;
    int core_id = -1;
};

string print_datetime() {
    auto now = chrono::system_clock::now();
    time_t current_time = chrono::system_clock::to_time_t(now);

    tm local_time;
    localtime_s(&local_time, &current_time);

    char date_buffer[100]; 
    strftime(date_buffer, sizeof(date_buffer), "%m/%d/%Y %I:%M:%S", &local_time);

    string am_pm = (local_time.tm_hour >= 12) ? "PM" : "AM";

    string formatted_time = string(date_buffer) + " " + am_pm;

    return formatted_time;
}

string int_to_string(int number) {
    char buffer[20];
    sprintf_s(buffer, "%d", number); 
    return string(buffer);        
}

vector<Process> processes;  
int core_progress[10] = { 0 }; 
int next_process_index = 0;   

void fcfs_core(int core_id){
    while (true) {
        if (next_process_index >= processes.size()) {
            return;
        }

        int process_index = next_process_index++;
        if (process_index >= processes.size()) {
            return; 
        }

        Process& process = processes[process_index];
        process.starttime = print_datetime();
        process.core_id = core_id;

        string filename = "process" + int_to_string(process.pid) + ".txt";
        
        while (core_progress[process.pid - 1] < 100) {
            process.processing = 1; 
            process.numprocess -= 1;
            core_progress[process.pid - 1]++; 

            ofstream log_file(filename, ios::app);
            if (log_file.is_open()) {
                log_file << print_datetime()
                    << ", Core: " << (core_id + 1)
                    << ", Hello from process " << process.pid << endl;
                log_file.close();
            }

            this_thread::sleep_for(chrono::milliseconds(100)); 
        }
        process.finish = 1; 
        process.processing = 0; 
        process.core_id = -1;
        process.finishtime = print_datetime();

        ofstream log_file(filename, ios::app);
        if (log_file.is_open()) {
            //log_file << "Process " << process.pid << " finished at " << process.finishtime << endl;
            log_file.close();
        }
    }
}

int main() {
    processes = {
        {1, 0, 100, 0},
        {2, 1, 100, 0},
        {3, 2, 100, 0},
        {4, 3, 100, 0},
        {5, 4, 100, 0},
        {6, 5, 100, 0},
        {7, 6, 100, 0},
        {8, 7, 100, 0},
        {9, 8, 100, 0},
        {10, 9, 100, 0}
    };

    cout << "FCFS SCHEDULER\n";
    thread core_threads[4];
    while (true) {
        cout << "\Command >> ";
        string cmd;
        getline(cin, cmd);

        if (cmd == "screen -ls") {
            if (next_process_index == 0) {
                for (int i = 0; i < 4; i++) {
                    core_threads[i] = thread(fcfs_core, i);
                    this_thread::sleep_for(chrono::milliseconds(100)); 
                }
            }

            cout << "\nRunning Processes:\n";
            for (size_t i = 0; i < processes.size(); i++) {
                if (processes[i].finish == 0 && processes[i].processing == 1) {
                    cout << "process" << processes[i].pid << "\t(" << processes[i].starttime << ") \tCore:" << processes[i].core_id << "\t\t" << 100 - processes[i].numprocess << " / 100\n";
                }
            }
            cout << "\nFinished Processes:\n";
            for (size_t i = 0; i < processes.size(); i++) {
                if (processes[i].finish == 1) {
                    cout << "process" << processes[i].pid << "\t(" << processes[i].finishtime << ") \tFINISHED\t" << 100 - processes[i].numprocess << " / 100\n" ;
                }
            }
        }
        else {
            cout << "\nUnknown command. Type 'screen -ls' to check progress.\n";
        }

        bool all_finished = true;
        for (const auto& process : processes) {
            if (process.finish == 0) {
                all_finished = false;
                break;
            }
        }

        if (all_finished) {
            break; 
        }

    }

    for (int i = 0; i < 4; i++) {
        if (core_threads[i].joinable()) {
            core_threads[i].join();
        }
    }
	return 0;
}