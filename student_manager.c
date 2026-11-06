/*
 * Student Record Management System
 * A small menu-driven C project using structures and file handling.
 *
 * Compile: gcc student_manager.c -o student_manager
 * Run:     ./student_manager       (Linux/macOS)
 *          student_manager.exe     (Windows)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "students.txt"
#define TEMP_FILE "students.tmp"
#define NAME_LENGTH 100
#define LINE_LENGTH 256

typedef struct {
    int roll;
    char name[NAME_LENGTH];
    float marks;
} Student;

static void readLine(const char *prompt, char *buffer, size_t size) {
    printf("%s", prompt);

    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }

    buffer[strcspn(buffer, "\n")] = '\0';
}

static int readInt(const char *prompt) {
    char input[64];
    char *end;
    long value;

    while (1) {
        readLine(prompt, input, sizeof(input));
        value = strtol(input, &end, 10);

        if (input[0] != '\0' && *end == '\0') {
            return (int)value;
        }
        printf("Please enter a valid whole number.\n");
    }
}

static float readMarks(const char *prompt) {
    char input[64];
    char *end;
    float value;

    while (1) {
        readLine(prompt, input, sizeof(input));
        value = strtof(input, &end);

        if (input[0] != '\0' && *end == '\0' && value >= 0.0f && value <= 100.0f) {
            return value;
        }
        printf("Enter marks between 0 and 100.\n");
    }
}

static int parseStudent(const char *line, Student *student) {
    char name[NAME_LENGTH];
    int roll;
    float marks;

    if (sscanf(line, "%d|%99[^|]|%f", &roll, name, &marks) != 3) {
        return 0;
    }

    student->roll = roll;
    strcpy(student->name, name);
    student->marks = marks;
    return 1;
}

static int rollExists(int roll) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[LINE_LENGTH];
    Student student;

    if (file == NULL) {
        return 0;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (parseStudent(line, &student) && student.roll == roll) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

static void addStudent(void) {
    Student student;
    FILE *file;

    printf("\n--- Add Student ---\n");
    student.roll = readInt("Enter roll number: ");

    if (student.roll <= 0) {
        printf("Roll number must be positive.\n");
        return;
    }

    if (rollExists(student.roll)) {
        printf("A student with that roll number already exists.\n");
        return;
    }

    readLine("Enter student name: ", student.name, sizeof(student.name));

    if (student.name[0] == '\0' || strchr(student.name, '|') != NULL) {
        printf("Name cannot be empty or contain the | character.\n");
        return;
    }

    student.marks = readMarks("Enter marks (0-100): ");

    file = fopen(DATA_FILE, "a");
    if (file == NULL) {
        printf("Could not open the data file.\n");
        return;
    }

    fprintf(file, "%d|%s|%.2f\n", student.roll, student.name, student.marks);
    fclose(file);

    printf("Student record saved successfully.\n");
}

static void viewStudents(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[LINE_LENGTH];
    Student student;
    int count = 0;

    printf("\n--- All Student Records ---\n");

    if (file == NULL) {
        printf("No records found yet. Add a student first.\n");
        return;
    }

    printf("%-10s %-30s %s\n", "Roll No.", "Name", "Marks");
    printf("------------------------------------------------------\n");

    while (fgets(line, sizeof(line), file) != NULL) {
        if (parseStudent(line, &student)) {
            printf("%-10d %-30s %.2f\n",
                   student.roll, student.name, student.marks);
            count++;
        }
    }

    fclose(file);

    if (count == 0) {
        printf("No valid records found.\n");
    } else {
        printf("\nTotal students: %d\n", count);
    }
}

static void searchStudent(void) {
    FILE *file = fopen(DATA_FILE, "r");
    char line[LINE_LENGTH];
    Student student;
    int roll;
    int found = 0;

    printf("\n--- Search Student ---\n");
    roll = readInt("Enter roll number to search: ");

    if (file == NULL) {
        printf("No records found yet.\n");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL) {
        if (parseStudent(line, &student) && student.roll == roll) {
            printf("\nRecord found:\n");
            printf("Roll number : %d\n", student.roll);
            printf("Name        : %s\n", student.name);
            printf("Marks       : %.2f\n", student.marks);
            found = 1;
            break;
        }
    }

    fclose(file);

    if (!found) {
        printf("No student found with roll number %d.\n", roll);
    }
}

static void deleteStudent(void) {
    FILE *source = fopen(DATA_FILE, "r");
    FILE *temp;
    char line[LINE_LENGTH];
    Student student;
    int roll;
    int deleted = 0;

    printf("\n--- Delete Student ---\n");
    roll = readInt("Enter roll number to delete: ");

    if (source == NULL) {
        printf("No records found yet.\n");
        return;
    }

    temp = fopen(TEMP_FILE, "w");
    if (temp == NULL) {
        fclose(source);
        printf("Could not create a temporary file.\n");
        return;
    }

    while (fgets(line, sizeof(line), source) != NULL) {
        if (parseStudent(line, &student) && student.roll == roll) {
            deleted = 1;
        } else {
            fputs(line, temp);
        }
    }

    fclose(source);
    fclose(temp);

    if (remove(DATA_FILE) != 0 || rename(TEMP_FILE, DATA_FILE) != 0) {
        printf("Could not update the data file. Check file permissions.\n");
        return;
    }

    if (deleted) {
        printf("Student record deleted successfully.\n");
    } else {
        printf("No student found with roll number %d.\n", roll);
    }
}

int main(void) {
    int choice;

    printf("=====================================\n");
    printf("     STUDENT RECORD MANAGER (C)      \n");
    printf("=====================================\n");

    do {
        printf("\n1. Add student\n");
        printf("2. View all students\n");
        printf("3. Search student by roll number\n");
        printf("4. Delete student\n");
        printf("5. Exit\n");

        choice = readInt("\nEnter your choice: ");

        switch (choice) {
            case 1:
                addStudent();
                break;
            case 2:
                viewStudents();
                break;
            case 3:
                searchStudent();
                break;
            case 4:
                deleteStudent();
                break;
            case 5:
                printf("Thank you for using Student Record Manager!\n");
                break;
            default:
                printf("Invalid choice. Please select 1 to 5.\n");
        }
    } while (choice != 5);

    return 0;
}
