import pandas as pd
import joblib
from collections import deque
import os

class Process:
    def __init__(self, pid, arrival_time, burst_time, priority, cpu_burst_est, 
                 io_burst_est, memory_req, total_cpu_used):
        self.pid = pid
        self.arrival_time = arrival_time
        self.burst_time = burst_time
        self.remaining_time = burst_time
        self.priority = priority
        self.cpu_burst_est = cpu_burst_est
        self.io_burst_est = io_burst_est
        self.memory_req = memory_req
        self.total_cpu_used = total_cpu_used
        
        # Metrics to be calculated
        self.completion_time = 0
        self.turnaround_time = 0
        self.waiting_time = 0
        self.response_time = 0
        self.start_time = -1
        self.isFirst = True

class MLRoundRobinScheduler:
    def __init__(self):
        self.processes = []
        self.model = None
        self.features = ['priority', 'cpu_burst_est', 'io_burst_est', 'arrival_time', 
                        'memory_req', 'total_cpu_used', 'waiting_time', 'turnaround_time']
        
        try:
            self.model = joblib.load("Random_Forest.pkl")
            print("ML model loaded successfully!")
        except FileNotFoundError:
            print("Error: Random_Forest.pkl not found!")
            print("Please make sure the model file is in the same directory.")
            exit()
        except Exception as e:
            print(f"Error loading model: {e}")
            exit()

    def predict_time_slice(self, process, waiting_time=0, turnaround_time=0):
        """Predict optimal time slice using ML model"""
        try:
            features_data = [
                process.priority,
                process.cpu_burst_est,
                process.io_burst_est,
                process.arrival_time,
                process.memory_req,
                process.total_cpu_used,
                waiting_time,
                turnaround_time
            ]
            
            X_new = pd.DataFrame([features_data], columns=self.features)
            predicted_time = self.model.predict(X_new)[0]
            return max(1, predicted_time)  # Ensure at least 1 unit
        except Exception as e:
            print(f"Prediction failed, using default time slice. Error: {e}")
            return 4

    def get_process_input(self, pid):
        print(f"\n--- Process {pid} Details ---")
        
        arrival_time = int(input(f"Enter arrival time for process {pid}: "))
        burst_time = int(input(f"Enter burst time for process {pid}: "))
        priority = int(input(f"Enter priority for process {pid}: "))
        cpu_burst_est = burst_time
        io_burst_est = float(input(f"Enter I/O burst estimate for process {pid}: "))
        memory_req = float(input(f"Enter memory requirement for process {pid}: "))
        total_cpu_used = burst_time
        
        return Process(pid, arrival_time, burst_time, priority, cpu_burst_est, 
                      io_burst_est, memory_req, total_cpu_used)

    def display_process_table(self, title):
        """Display processes in a proper tabular format"""
        print(f"\n{'='*120}")
        print(f"{title:^120}")
        print(f"{'='*120}")
        
        headers = [
            "PID", "Arrival", "Burst", "Priority", "I/O Est", "Memory", 
            "Start", "Completion", "Turnaround", "Waiting", "Response"
        ]
        
        # Print header
        header_line = " | ".join(f"{header:^10}" for header in headers)
        print(header_line)
        print("-" * 120)
        
        # Print each process
        for process in sorted(self.processes, key=lambda x: x.pid):
            row = [
                f"{process.pid:^10}",
                f"{process.arrival_time:^10}",
                f"{process.burst_time:^10}",
                f"{process.priority:^10}",
                f"{process.io_burst_est:^10.2f}",
                f"{process.memory_req:^10.2f}",
                f"{process.start_time:^10}",
                f"{process.completion_time:^10}",
                f"{process.turnaround_time:^10}",
                f"{process.waiting_time:^10}",
                f"{process.response_time:^10}",
            ]
            print(" | ".join(row))
        
        print(f"{'='*120}")

    def display_execution_log(self, execution_steps):
        print(f"\n{'-'*80}")
        print(f"{'EXECUTION LOG':^80}")
        print(f"{'='*80}")
        
        headers = ["Time", "Process", "Action", "Duration", "Remaining", "ML Prediction"]
        header_line = " | ".join(f"{header:^15}" for header in headers)
        print(header_line)
        print("-" * 80)
        
        for step in execution_steps:
            row = [
                f"{step['time']:^15}",
                f"P{step['process']:^14}",
                f"{step['action']:^15}",
                f"{step['duration']:^15}",
                f"{step['remaining']:^15}",
                f"{step['prediction']:^15.2f}"
            ]
            print(" | ".join(row))
        
        print(f"{'='*80}")

    def display_performance_metrics(self, wait_sum, response_sum, tat_sum, n):
        print(f"\n{'-'*60}")
        print(f"{'PERFORMANCE METRICS':^60}")
        print(f"{'-'*60}")
        
        metrics = [
            ("Total Processes", f"{n}"),
            ("Average Waiting Time", f"{wait_sum/n:.4f} units"),
            ("Average Turnaround Time", f"{tat_sum/n:.4f} units"),
            ("Average Response Time", f"{response_sum/n:.4f} units"),
            ("CPU Utilization", f"{(sum(p.burst_time for p in self.processes) / max(p.completion_time for p in self.processes)) * 100:.2f}%"),
            ("Throughput", f"{n / max(p.completion_time for p in self.processes):.4f} processes/unit")
        ]
        
        print(f"{'Metric':<25} {'Value':<35}")
        print("-" * 60)
        for metric, value in metrics:
            print(f"{metric:<25} {value:<35}")
        print(f"{'-'*60}")

    def display_ml_predictions(self):
        print(f"\n{'='*50}")
        print(f"{'ML PREDICTED TIME SLICES':^50}")
        print(f"{'='*50}")

    def schedule_processes(self):
        
        sorted_processes = sorted(self.processes, key=lambda x: x.arrival_time)
        queue = []
        execution_steps = []
        
        wait_sum = 0
        response_sum = 0
        tat_sum = 0
        
        if sorted_processes:
            queue.append(sorted_processes[0])
            point = 1
        else:
            print("No processes to schedule!")
            return
        
        current_time = sorted_processes[0].arrival_time
        front = 0
        
        print(f"\n{' INITIAL PROCESS TABLE ':-^120}")
        self.display_process_table("INITIAL PROCESS STATE")
        
        while front < len(queue):
            current_process = queue[front]
            
            if current_process.isFirst:
                current_process.start_time = current_time
                current_process.isFirst = False
                current_process.response_time = current_process.start_time - current_process.arrival_time

            predicted_time_slice = self.predict_time_slice(current_process)
            execution_time = min(predicted_time_slice, current_process.remaining_time)
            
            execution_steps.append({
                'time': current_time,
                'process': current_process.pid,
                'action': 'EXECUTING',
                'duration': execution_time,
                'remaining': current_process.remaining_time - execution_time,
                'prediction': predicted_time_slice
            })
            
            current_time += execution_time
            current_process.remaining_time -= execution_time
            
            while point < len(sorted_processes) and sorted_processes[point].arrival_time <= current_time:
                queue.append(sorted_processes[point])
                execution_steps.append({
                    'time': sorted_processes[point].arrival_time,
                    'process': sorted_processes[point].pid,
                    'action': 'ARRIVED',
                    'duration': 0,
                    'remaining': sorted_processes[point].remaining_time,
                    'prediction': 0
                })
                point += 1

            if current_process.remaining_time <= 0:
                current_process.completion_time = current_time
                current_process.turnaround_time = current_process.completion_time - current_process.arrival_time
                current_process.waiting_time = current_process.turnaround_time - current_process.burst_time
                
                wait_sum += current_process.waiting_time
                response_sum += current_process.response_time
                tat_sum += current_process.turnaround_time
                
                execution_steps.append({
                    'time': current_time,
                    'process': current_process.pid,
                    'action': 'COMPLETED',
                    'duration': 0,
                    'remaining': 0,
                    'prediction': 0
                })
            else:
                queue.append(current_process)
                execution_steps.append({
                    'time': current_time,
                    'process': current_process.pid,
                    'action': 'REQUEUED',
                    'duration': 0,
                    'remaining': current_process.remaining_time,
                    'prediction': 0
                })
            
            front += 1

        self.display_execution_log(execution_steps)
        self.display_process_table("FINAL PROCESS TABLE")
        self.display_performance_metrics(wait_sum, response_sum, tat_sum, len(self.processes))
        self.display_ml_predictions()

    def run(self):
        
        while True:
            try:
                n = int(input("\nEnter the number of processes: "))
                if n > 0:
                    break
                else:
                    print("Please enter a positive number.")
            except ValueError:
                print("Please enter a valid integer.")

        for i in range(n):
            process = self.get_process_input(i + 1)
            self.processes.append(process)

        self.schedule_processes()

# Run the scheduler
if __name__ == "__main__":
    scheduler = MLRoundRobinScheduler()
    scheduler.run()