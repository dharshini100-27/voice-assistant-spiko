#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct Task {
    int id;
    char name[100];
    char date[20];
    char time[10];
    char day[10];
    char status[5];
    char target_time[30];
    char completed_time[20];
    char completed_day[10];
};

int get_next_id() {
    FILE *file = fopen("tasks.txt", "r");
    if (!file) return 1; // First ID if file doesn't exist

    int id = 0, max_id = 0;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "Id : %d", &id) == 1) {
            if (id > max_id) {
                max_id = id;
            }
        }
    }

    fclose(file);
    return max_id + 1;
}
void to_do_list_mode() {
    char buffer[1024];
    struct Task t;
    int update_id;

    printf("\n--- TO-DO LIST ACTIVATED ---\n");
    printf("Say: 'add', 'mark', or 'exit'\n");

    FILE *fp = popen("python v.py", "r");
    if (!fp) {
        perror("popen failed");
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        printf("To-Do Heard: %s\n", buffer);

        if (strncmp(buffer,"add",3)==0) {
            FILE *file = fopen("tasks.txt", "a");
            if (!file) {
                printf("Error opening file.\n");
                continue;
            }

            printf("Say the task description:\n");
            if (fgets(buffer, sizeof(buffer), fp) == NULL) continue;
            buffer[strcspn(buffer, "\n")] = 0;
            strcpy(t.name, buffer);

            time_t now = time(NULL);
            struct tm *lt = localtime(&now);
            strftime(t.date, sizeof(t.date), "%d-%m-%Y", lt);
            strftime(t.time, sizeof(t.time), "%H:%M:%S", lt);
            strftime(t.day, sizeof(t.day), "%A", lt);

            t.id = get_next_id();
            strcpy(t.status, "❌");
            strcpy(t.completed_time, "-");
            strcpy(t.completed_day, "-");

            printf("Enter the target time or 'no':\n");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = 0;

            if (strcmp(buffer, "no") == 0) {
                strcpy(t.target_time, "N/A");
            } else {
                snprintf(t.target_time,sizeof(t.target_time),"%s.hrs",buffer);
            }

            fprintf(file,
                "Id : %d\nTask: %s\nDate: %s\nTime: %s\nDay: %s\nStatus: %s\nTarget: %s\nCompleted  time: %s\nCompleted  day: %s\n\n",
                t.id, t.name, t.date, t.time, t.day, t.status,
                t.target_time, t.completed_time, t.completed_day);

            fclose(file);
            printf("Task added.\n");

        } else if (strncmp(buffer, "mark", 4) == 0) {
            FILE *file = fopen("tasks.txt", "r");
            FILE *tempFile = fopen("temp.txt", "w");
            if (!file || !tempFile) {
                printf("Error opening files.\n");
                continue;
            }

            printf("Enter the task ID to mark complete:\n");
            scanf("%d", &update_id);
            getchar();  // consume newline

            int found = 0;
            while (fscanf(file, "Id : %d\n", &t.id) == 1) {
                fgets(buffer, sizeof(buffer), file); sscanf(buffer, "Task: %[^\n]", t.name);
                fgets(buffer, sizeof(buffer), file); sscanf(buffer, "Date: %[^\n]", t.date);
                fgets(buffer, sizeof(buffer), file); sscanf(buffer, "Time: %[^\n]", t.time);
                fgets(buffer, sizeof(buffer), file); sscanf(buffer, "Day: %[^\n]", t.day);
                fgets(buffer, sizeof(buffer), file); sscanf(buffer, "Status: %[^\n]", t.status);
                fgets(buffer, sizeof(buffer), file); sscanf(buffer, "Target: %[^\n]", t.target_time);
                fgets(buffer, sizeof(buffer), file); sscanf(buffer, "Completed on time: %[^\n]", t.completed_time);
                fgets(buffer, sizeof(buffer), file); sscanf(buffer, "Completed on day: %[^\n]", t.completed_day);
                fgets(buffer, sizeof(buffer), file); // empty line

                if (t.id == update_id && strcmp(t.status, "✅") != 0) {
                    strcpy(t.status, "✅");
                    time_t now = time(NULL);
                    struct tm *lt = localtime(&now);
                    strftime(t.completed_time, sizeof(t.completed_time), "%H:%M:%S", lt);
                    strftime(t.completed_day, sizeof(t.completed_day), "%A", lt);
                    found = 1;
                }

                fprintf(tempFile,
                    "Id : %d\nTask: %s\nDate: %s\nTime: %s\nDay: %s\nStatus: %s\nTarget: %s\nCompleted time: %s\nCompleted  day: %s\n\n",
                    t.id, t.name, t.date, t.time, t.day, t.status,
                    t.target_time, t.completed_time, t.completed_day);
            }

            fclose(file);
            fclose(tempFile);
            remove("tasks.txt");
            rename("temp.txt", "tasks.txt");

            if (found)
                printf("Task marked as complete.\n");
            else
                printf("Task not found.\n");

        } else if (strcmp(buffer, "exit") == 0) {
            printf("Exiting To-Do List...\n");
            break;
        } else {
            printf("Unknown to-do command: %s\n", buffer);
        }

        printf("\nSay: 'add task', 'mark', or 'exit'\n");
    }

    pclose(fp);
}

int main() {
    FILE *fp;
    char buffer[1024];
    char command[1100];

    printf("");
    fp = popen("python v.py", "r");
    if (!fp) {
        perror("popen failed");
        return 1;
    }
    system("espeak \"This is your voice assistant\"");
    printf("--Voice Assistant Running-\n");
    printf("-- Options:\n-- open <platform>\n-- search <query>\n-- open to do list\n-- bye to exit\n");

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
        printf("Recognized: %s\n", buffer);

        if (strcmp(buffer, "Listening... (Say 'bye' to stop)") == 0)
            continue;

        if (strncmp(buffer, "open ", 5) == 0) {
            char *site = buffer + 5;
            snprintf(command, sizeof(command), "start https://www.%s.com &", site);
            printf("Opening: %s\n", site);
            system(command);

        } else if (strncmp(buffer, "search ", 7) == 0) {
            char *query;
            query = buffer + 7;
            printf("Searching: %s\n", query);
            for(int i = 0;i<strlen(query);i++){
                if(query[i]==' '){
                    query[i]='+';
                }
            }
            snprintf(command, sizeof(command), "start https://www.google.com/search?q=%s ", query);
           // printf("Searching: %s\n", query);
            system(command);

        } else if (strcmp(buffer, "to do list") == 0) {
            to_do_list_mode();
            printf("Returned to main assistant.\n");

        } else if (strcmp(buffer, "bye") == 0 || strcmp(buffer, "shutdown") == 0) {
            printf("Shutting down assistant...\n");
            break;

        } else {
            printf("Command not recognized: %s\n", buffer);
        }
    }

    pclose(fp);
    return 0;
}