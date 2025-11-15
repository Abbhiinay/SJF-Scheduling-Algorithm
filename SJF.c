#include <stdio.h>
#include <stdlib.h> 
#include <limits.h>

typedef struct {
    int id;
    int arrival;
    int burst;
    int completion;
    int tat; // Turnaround Time
    int wt;  // Waiting Time
    int isCompleted; // 0 or 1
} Process;

// Structure to store Gantt chart entries
typedef struct {
    int processId; // -1 for IDLE
    int endTime;
} GanttEntry;

/**
 * Comparison function for qsort.
 * Sorts processes based on their arrival time.
 */
int compareArrival(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    return p1->arrival - p2->arrival;
}

int main() {
    int n, i;
    int currentTime = 0;
    int completedProcesses = 0;
    float totalTAT = 0.0;
    float totalWT = 0.0;
    int totalBurstTime = 0;
    int totalIdleTime = 0;

    printf("Enter the number of processes: ");
    scanf("%d", &n);
    if (n <= 0) {
        printf("No processes to schedule.\n");
        return 0;
    }

    Process processes[n];
    GanttEntry gantt[n * 2]; 
    int ganttCount = 0;

    // 1. Get process details
    for (i = 0; i < n; i++) {
        processes[i].id = i + 1; // Use 1-based IDs
        printf("--- Process %d ---\n", processes[i].id);
        printf("Enter Arrival Time: ");
        scanf("%d", &processes[i].arrival);
        printf("Enter Burst Time: ");
        scanf("%d", &processes[i].burst);

        processes[i].isCompleted = 0; // Mark as not completed
        totalBurstTime += processes[i].burst;
    }

    // Sort processes by arrival time initially
    qsort(processes, n, sizeof(Process), compareArrival);

    // 2. Main Scheduling Loop
    while (completedProcesses < n) {
        int selectedIndex = -1;
        int minBurst = INT_MAX;

        // Find the available process with the shortest burst time
        for (i = 0; i < n; i++) {
            if (processes[i].arrival <= currentTime && processes[i].isCompleted == 0) {
                if (processes[i].burst < minBurst) {
                    minBurst = processes[i].burst;
                    selectedIndex = i;
                }
            }
        }

        if (selectedIndex != -1) {
            // --- A process is found and will be executed ---
            Process* p = &processes[selectedIndex];

            currentTime += p->burst;
            p->completion = currentTime;
            p->tat = p->completion - p->arrival;
            p->wt = p->tat - p->burst;

            totalTAT += p->tat;
            totalWT += p->wt;

            p->isCompleted = 1;
            completedProcesses++;

            // Add to Gantt chart
            gantt[ganttCount].processId = p->id;
            gantt[ganttCount].endTime = currentTime;
            ganttCount++;

        } else {
            // --- No process is available (CPU is idle) ---
            
            // Find the time of the *next* arriving process
            int nextArrival = INT_MAX;
            for (i = 0; i < n; i++) {
                if (processes[i].isCompleted == 0 && processes[i].arrival < nextArrival) {
                    nextArrival = processes[i].arrival;
                }
            }

            // If we are here, it means currentTime < nextArrival
            if (nextArrival != INT_MAX) {
                int idleStartTime = currentTime;
                totalIdleTime += (nextArrival - currentTime); // Add to idle time
                currentTime = nextArrival; // Move time forward

                // Add IDLE entry to Gantt chart
                gantt[ganttCount].processId = -1; // -1 represents IDLE
                gantt[ganttCount].endTime = currentTime;
                ganttCount++;
            }
        }
    }

    // 3. Display Results
    printf("\n## CPU Scheduling Results (SJF Non-Preemptive) ##\n");
    printf("-----------------------------------------------------------------------------------------\n");
    printf("%-10s %-15s %-15s %-20s %-20s %-15s\n", 
           "Process", "Arrival Time", "Burst Time", "Completion Time", "Turnaround Time", "Waiting Time");
    printf("-----------------------------------------------------------------------------------------\n");

    for (i = 0; i < n; i++) {
        printf("P%-9d %-15d %-15d %-20d %-20d %-15d\n", 
               processes[i].id, processes[i].arrival, processes[i].burst, 
               processes[i].completion, processes[i].tat, processes[i].wt);
    }
    printf("-----------------------------------------------------------------------------------------\n");

    // 4. Display Averages and CPU Utilization
    float avgTAT = totalTAT / n;
    float avgWT = totalWT / n;
    
    // 'currentTime' at the end of the simulation is the total time (Makespan)
    float cpuUtilization = ((float)totalBurstTime / currentTime) * 100.0;

    printf("\n**Average Waiting Time:** %.2f\n", avgWT);
    printf("**Average Turnaround Time:** %.2f\n", avgTAT);
    printf("**CPU Utilization:** %.2f%%\n", cpuUtilization);

    // 5. Display Gantt Chart
    printf("\n## Gantt Chart (Timeline) ##\n");
    int startTime = 0;
    for (i = 0; i < ganttCount; i++) {
        printf("| %d ", startTime);
        if (gantt[i].processId == -1) {
            printf(" IDLE ");
        } else {
            printf(" P%d ", gantt[i].processId);
        }
        startTime = gantt[i].endTime;
    }
    printf("| %d |\n", currentTime);

    return 0;
}
