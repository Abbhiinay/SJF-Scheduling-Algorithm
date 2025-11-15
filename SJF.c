#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// Structure to represent a process
typedef struct
{
    int id;
    int arrival;
    int burst;
    int completion;
    int tat;         // Turnaround Time
    int wt;          // Waiting Time
    int isCompleted; // (1 if complete 0 if not)
} Process;

// Structure to store Gantt chart entries
typedef struct
{
    int processId;
    int endTime;
} GanttEntry;

// comparision funtion to sort process on AT using qsort()
int compareArrival(const void *a, const void *b)
{
    Process *p1 = (Process *)a;
    Process *p2 = (Process *)b;
    return p1->arrival - p2->arrival;
}

int main()
{
    int n;
    int currentTime = 0;
    int completedProcesses = 0;
    float totalTAT = 0.0;
    float totalWT = 0.0;
    int totalBurstTime = 0; // Total of all the processes BT
    int totalIdleTime = 0;

    printf("Enter the number of processes: ");
    scanf("%d", &n);
    if (n <= 0)
    {
        printf("No processes to schedule.\n");
        return 0;
    }

    Process processes[n];
    GanttEntry gantt[n * 2];
    int ganttCount = 0;

    for (int i = 0; i < n; i++)
    {
        processes[i].id = i + 1;
        printf("--- Process %d ---\n", processes[i].id);
        printf("Enter Arrival Time: ");
        scanf("%d", &processes[i].arrival);
        printf("Enter Burst Time: ");
        scanf("%d", &processes[i].burst);

        processes[i].isCompleted = 0;
        totalBurstTime += processes[i].burst;
    }

    // Sort processes by arrival time initially
    qsort(processes, n, sizeof(Process), compareArrival);

    printf("\n## In-Process Log ##\n");
    printf("Simulation starts at time 0.\n\n");

    // Scheduling starts here....
    while (completedProcesses < n)
    {
        int selectedIndex = -1;
        int minBurst = INT_MAX;

        // Find the available process with the shortest burst time
        for (int i = 0; i < n; i++)
        {
            if (processes[i].arrival <= currentTime && processes[i].isCompleted == 0)
            {
                if (processes[i].burst < minBurst)
                {
                    minBurst = processes[i].burst;
                    selectedIndex = i; // store the index of the process
                }
            }
        }
        // optimum process found
        if (selectedIndex != -1)
        {
            Process *p = &processes[selectedIndex];

            int startTime = currentTime; // Store start time
            currentTime += p->burst;
            p->completion = currentTime;

            printf("-> Process P%d ran from time %d to %d (Completed).\n", p->id, startTime, p->completion);

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

            // Logging of all the ready queue processes
            if (completedProcesses < n)
            {
                printf(" At time %d, checking ready queue...\n", currentTime);
                int queueCount = 0;
                int nextProcessId = -1;
                int nextMinBurst = INT_MAX;

                printf(" Ready Queue: [");
                for (int j = 0; j < n; j++)
                {
                    // Finding the processes in ready queue
                    if (processes[j].arrival <= currentTime && processes[j].isCompleted == 0)
                    {
                        printf(" P%d(Burst:%d) ", processes[j].id, processes[j].burst);
                        queueCount++;
                        // Find the shortest job among them
                        if (processes[j].burst < nextMinBurst)
                        {
                            nextMinBurst = processes[j].burst;
                            nextProcessId = processes[j].id;
                        }
                    }
                }

                if (queueCount == 0)
                {
                    printf(" Empty ");
                }
                printf("]\n");

                if (nextProcessId != -1)
                {
                    printf(" -> Next to run: P%d (shortest job).\n\n", nextProcessId);
                }
                else
                {
                    printf(" -> Ready Queue is empty. CPU will be idle.\n\n");
                }
            }
        }
        // No optimal process found i.e, IDLE
        else
        {

            // Find the time of the next arriving process
            int nextArrival = INT_MAX;
            for (int i = 0; i < n; i++)
            {
                if (processes[i].isCompleted == 0 && processes[i].arrival < nextArrival)
                {
                    nextArrival = processes[i].arrival;
                }
            }

            if (nextArrival != INT_MAX)
            {
                int idleStartTime = currentTime;
                totalIdleTime += (nextArrival - currentTime);
                currentTime = nextArrival;

                printf("-> CPU idle from time %d to %d.\n", idleStartTime, currentTime);

                int nextProcessId = -1;
                int nextMinBurst = INT_MAX;
                printf("   At time %d, new arrivals: [", currentTime);
                for (int j = 0; j < n; j++)
                {
                    if (processes[j].arrival == currentTime && processes[j].isCompleted == 0)
                    {
                        printf(" P%d(Burst:%d) ", processes[j].id, processes[j].burst);
                        if (processes[j].burst < nextMinBurst)
                        {
                            nextMinBurst = processes[j].burst;
                            nextProcessId = processes[j].id;
                        }
                    }
                }
                printf("]\n");
                printf("    -> Next to run: P%d (shortest new arrival).\n\n", nextProcessId);

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

    for (int i = 0; i < n; i++)
    {
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
    for (int i = 0; i < ganttCount; i++)
    {
        printf("| %d ", startTime);
        if (gantt[i].processId == -1)
        {
            printf(" IDLE ");
        }
        else
        {
            printf(" P%d ", gantt[i].processId);
        }
        startTime = gantt[i].endTime;
    }
    printf("| %d |\n", currentTime);

    return 0;
}
