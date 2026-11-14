#include <ctype.h>
#include <conio.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESETCOLOR "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"

typedef struct {
    int id;
    char name[64];
    char email[64];
    char password[64];
    char section;
    int present;
    int sessions;
    int subject_present[6];
    int subject_sessions[6];
    double marks[6];
    double assessments[6][4];
} Student;

typedef struct {
    int id;
    char name[64];
    char email[64];
    char password[64];
    char sections[64];
    char subjects[64];
    char course_codes[64];
} Account;

Student *students = NULL;
int student_capacity;
Account teachers[100];
Account admin;
int student_count;
int teacher_count;

/* Grow student list with realloc (no fixed limit) */
/* -1 = mark not entered yet */
double shown(double v)
{
    return v < 0.0 ? 0.0 : v;
}

void clear_marks(Student *s)
{
    int i, k;
    for (i = 0; i < 6; i++) {
        (*s).marks[i] = -1.0;
        for (k = 0; k < 4; k++) (*s).assessments[i][k] = -1.0;
    }
}

int has_marks(const Student *s)
{
    int i;
    for (i = 0; i < 6; i++) if ((*s).marks[i] >= 0.0) return 1;
    return 0;
}

int ensure_student_space(void)
{
    int n;
    Student *p;
    if (student_count < student_capacity) return 1;
    n = student_capacity ? student_capacity * 2 : 50;
    p = realloc(students, n * sizeof(Student));
    if (p == NULL) return 0;
    students = p;
    student_capacity = n;
    return 1;
}
int admin_exists;
const char *subject_names[6] = {
    "Programming Fundamentals", "Fundamentals English", "Applied Physics",
    "ICP", "ITFA", "IS"
};
static const int subject_credit_hours[6] = {4, 3, 3, 3, 4, 3};
static const char *assessment_names[4] = {"Assignments", "Quizzes", "Midterms", "Finals"};
static const double assessment_max_marks[4] = {10.0, 10.0, 20.0, 60.0};
const char *course_codes[6] = {"CT-175", "EA-128", "PH-129", "ES-108", "EF-101", "ES-206"};

static const char *days_of_week[5] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

static const char *timetable[6][5] = {
    {
        "11:30 - 13:10: ITFA (Room #06); 08:30 - 10:10: IS (Room #06); 10:10 - 11:00: AP (Room #12); 11:30 - 13:10: PF (Room #06); 14:00 - 15:40: Practical @ Lab 05",
        "08:30 - 10:10: PF (Room #06); 10:10 - 11:00: FE (Room #07); 11:30 - 13:10: ITFA (Room #06); 14:00 - 15:40: PF",
        "08:30 - 10:10: PF (Room #06); 10:10 - 11:00: FE (Room #07); 11:30 - 13:10: ITFA (Room #06); 14:00 - 15:40: PF",
        "14:00 - 15:40: FE & AP (Room #06)"
    },
    {
        "08:30 - 10:10: PF (Room #05); 10:10 - 11:00: ICP (Room #05); 11:30 - 13:10: IS (Room #05); 14:00 - 15:40: FE (Room #05)",
        "08:30 - 11:00: ITFA Practical @ Lab 02; 11:30 - 13:10: Practical @ Lab 02; 14:00 - 15:40: AP (Room #05)",
        "14:00 - 15:40: ITFA (Room #05) & FE (Room #05)",
        "08:30 - 10:10: AP (Room #05); 11:30 - 13:10: PF (Room #05)"
    },
    {
        "08:30 - 10:10: PF (Room #03); 10:10 - 11:00: ITFA (Room #03); 11:30 - 13:10: AP (Room #03)",
        "08:30 - 10:10: IS (Room #03); 10:10 - 11:00: FE (Room #03); 11:30 - 13:10: ICP (Room #03)",
        "08:30 - 11:00: ITFA Practical @ Lab 02; 11:30 - 13:10: Practical @ Lab 02; 14:00 - 15:40: AP (Room #03)",
        "11:30 - 13:10: PF (Room #01); 14:00 - 15:40: FE (Room #08)",
        "11:30 - 13:10: PF"
    },
    {
        "08:30 - 10:10: AP (Room #04); 10:10 - 11:00: ITFA (Room #04)",
        "08:30 - 10:10: ICP (Room #04); 10:10 - 11:00: FE (Room #04); 11:30 - 13:10: IS (Room #04); 14:00 - 15:40: FE (Room #03)",
        "08:30 - 10:10: PF (Room #04); 11:30 - 13:10: ITFA Practical @ Lab 03; 14:00 - 15:40: AP (Room #04)",
        "11:30 - 13:10: PF Practical @ Lab 01; 14:00 - 15:40: PF Practical @ Lab 02",
        "08:30 - 10:10: PF"
    },
    {
        "08:30 - 10:10: PF (Room #07); 10:10 - 11:00: AP (Room #07); 11:30 - 13:10: FE (Room #07)",
        "08:30 - 11:00: PF Practical @ Lab 04; 11:30 - 13:10: ICP (Room #08); 14:00 - 15:40: IS (Room #04)",
        "No standard theory listed, spare day/consultation",
        "14:00 - 15:40: ITFA & Practical @ Lab 03",
        "08:30 - 10:10: PF (Room #07); 10:10 - 11:00: ITFA (Room #07); 11:30 - 13:10: AP (Room #07)"
    },
    {
        "08:30 - 10:10: FE (Room #08); 11:30 - 13:10: PF (Room #08)",
        "08:30 - 10:10: ICP (Room #08); 11:30 - 13:10: FE (Room #05); 14:00 - 15:40: ITFA Practical @ Lab 05",
        "08:30 - 10:10: PF (Room #05); 11:30 - 13:10: AP (Room #05); 14:00 - 15:40: PF Practical @ Lab 05",
        "08:30 - 10:10: AP (Room #08); 11:30 - 13:10: ITFA (Room #08)",
        "08:30 - 10:10: IS (Room #08)"
    }
};

void heading(const char *text)
{
    printf("\n" BLUE "=== %s ===" RESETCOLOR "\n", text);
}

void error_message(const char *text)
{
    printf(RED "%s\n" RESETCOLOR, text);
}

int input(const char *prompt, char *buffer, size_t size)
{
    size_t length;
    printf(BLUE "%s" RESETCOLOR, prompt);
    fflush(stdout);
    if (fgets(buffer, (int)size, stdin) == NULL) return 0;
    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') buffer[--length] = '\0';
    else if (!feof(stdin)) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) {}
        error_message("Input is too long.");
        return -1;
    }
    if (length > 0 && buffer[length - 1] == '\r') buffer[length - 1] = '\0';
    if (buffer[0] == '\0' || strchr(buffer, ',') != NULL) {
        error_message("Input cannot be empty or contain commas.");
        return -1;
    }
    return 1;
}

int input_password(const char *prompt, char *buffer, size_t size)
{
    size_t length = 0;
    int character;
    printf(BLUE "%s" RESETCOLOR, prompt);
    fflush(stdout);
    if (size == 0) return 0;
    for (;;) {
        character = _getch();
        if (character == '\r' || character == '\n') {
            putchar('\n');
            if (length == 0) {
                error_message("Password cannot be empty.");
                printf(BLUE "%s" RESETCOLOR, prompt);
                fflush(stdout);
                continue;
            }
            buffer[length] = '\0';
            return 1;
        }
        if (character == 3) return 0;
        if (character == 0 || character == 224) {
            (void)_getch();
            continue;
        }
        if (character == '\b' || character == 127) {
            if (length > 0) {
                length--;
                printf("\b \b");
                fflush(stdout);
            }
            continue;
        }
        if (character == ',' || character < 32) continue;
        if (length + 1 < size) {
            buffer[length++] = (char)character;
            putchar('*');
            fflush(stdout);
        }
    }
}

int confirm_action(const char *prompt)
{
    char response[16];
    for (;;) {
        if (input(prompt, response, sizeof(response)) <= 0) return 0;
        if (response[1] == '\0' && (toupper((unsigned char)response[0]) == 'Y' ||
            toupper((unsigned char)response[0]) == 'N'))
            return toupper((unsigned char)response[0]) == 'Y';
        error_message("Enter Y to confirm or N to cancel.");
    }
}

int number(const char *prompt, int low, int high)
{
    char text[100];
    char *end;
    long value;
    for (;;) {
        int input_result = input(prompt, text, sizeof(text));
        if (input_result == 0) return -1;
        if (input_result < 0) continue;
        errno = 0;
        value = strtol(text, &end, 10);
        while (isspace((unsigned char)*end)) end++;
        if (text[0] && *end == '\0' && errno != ERANGE && value >= low && value <= high)
            return (int)value;
        printf(RED "Enter a whole number from %d to %d.\n" RESETCOLOR, low, high);
    }
}

int valid_email(const char *email)
{
    size_t length = strlen(email);
    const char suffix[] = "@gmail.com";
    return length > sizeof(suffix) - 1 &&
        strcmp(email + length - (sizeof(suffix) - 1), suffix) == 0;
}

int email_exists(const char *email)
{
    int i;
    if (admin_exists && strcmp(admin.email, email) == 0) return 1;
    for (i = 0; i < teacher_count; i++)
        if (strcmp(teachers[i].email, email) == 0) return 1;
    for (i = 0; i < student_count; i++)
        if (strcmp(students[i].email, email) == 0) return 1;
    return 0;
}

/* largest roll id in the list (loops walk ids 1..max in order) */
int max_student_id(void)
{
    int i, max = 0;
    for (i = 0; i < student_count; i++) if (students[i].id > max) max = students[i].id;
    return max;
}

Student *student_by_id(int id)
{
    int i;
    for (i = 0; i < student_count; i++)
        if (students[i].id == id) return &students[i];
    return NULL;
}

int parse_student_id(const char *roll_number, int *id)
{
    const char *digits = strncmp(roll_number, "CT-", 3) == 0 ? roll_number + 3 : roll_number;
    char *end;
    long parsed_id;
    size_t digits_length = strlen(digits);
    errno = 0;
    parsed_id = strtol(digits, &end, 10);
    if (digits[0] == '\0' || *end != '\0' || errno == ERANGE ||
        parsed_id < 1 || parsed_id > 2147483647L) return 0;
    if (digits_length == 5 && digits[0] == '2' && digits[1] == '6')
        parsed_id = strtol(digits + 2, NULL, 10);
    if (parsed_id < 1 || parsed_id > 2147483647L) return 0;
    *id = (int)parsed_id;
    return 1;
}

double attendance(const Student *student)
{
    if ((*student).sessions == 0) return 0.0;
    return 100.0 * (*student).present / (*student).sessions;
}

double grade_points(double mark)
{
    if (mark >= 85) return 4.0;
    if (mark >= 80) return 3.7;
    if (mark >= 75) return 3.4;
    if (mark >= 70) return 3.0;
    if (mark >= 67) return 2.7;
    if (mark >= 64) return 2.4;
    if (mark >= 60) return 2.0;
    if (mark >= 57) return 1.7;
    if (mark >= 54) return 1.4;
    if (mark >= 50) return 1.0;
    return 0.0;
}

double cgpa(const Student *student)
{
    int i;
    int total_credit_hours = 0;
    double weighted_grade_points = 0.0;
    for (i = 0; i < 6; i++) {
        if ((*student).marks[i] < 0.0) continue;
        weighted_grade_points += grade_points((*student).marks[i]) * subject_credit_hours[i];
        total_credit_hours += subject_credit_hours[i];
    }
    if (total_credit_hours == 0) return 0.0;
    return weighted_grade_points / total_credit_hours;
}

int save_accounts(const char *filename, const Account *accounts, int count)
{
    FILE *file = fopen(filename, "w");
    int i;
    if (file == NULL) return 0;
    fputs("ID,Name,Email,Password,Sections,Subjects,CourseCodes\n", file);
    for (i = 0; i < count; i++)
        fprintf(file, "%d,%s,%s,%s,%s,%s,%s\n", accounts[i].id, accounts[i].name,
            accounts[i].email, accounts[i].password, accounts[i].sections, accounts[i].subjects,
            accounts[i].course_codes);
    return fclose(file) == 0;
}

int save_students(void)
{
    FILE *file = fopen("file_student.csv", "w");
    int i, j;
    if (file == NULL) return 0;
    fputs("Roll Number,Name,Email,Password,Section,Present,Sessions,PF Attendance,FE Attendance,AP Attendance,ICP Attendance,ITFA Attendance,IS Attendance,"
        "PF Assignments,PF Quizzes,PF Midterms,PF Finals,FE Assignments,FE Quizzes,FE Midterms,FE Finals,"
        "AP Assignments,AP Quizzes,AP Midterms,AP Finals,ICP Assignments,ICP Quizzes,ICP Midterms,ICP Finals,"
        "ITFA Assignments,ITFA Quizzes,ITFA Midterms,ITFA Finals,IS Assignments,IS Quizzes,IS Midterms,IS Finals,CGPA\n", file);
    for (i = 0; i < student_count; i++) {
        Student *s = &students[i];
        fprintf(file, "CT-%03d,%s,%s,%s,%c,%d,%d", (*s).id, (*s).name, (*s).email,
            (*s).password, (*s).section, (*s).present, (*s).sessions);
        for (j = 0; j < 6; j++) fprintf(file, ",%d/%d", (*s).subject_present[j], (*s).subject_sessions[j]);
        for (j = 0; j < 6; j++) {
            int k;
            for (k = 0; k < 4; k++) {
                if ((*s).assessments[j][k] < 0.0) fputc(',', file);
                else fprintf(file, ",%.6f", (*s).assessments[j][k]);
            }
        }
        fprintf(file, ",%.2f\n", cgpa(s));
    }
    return fclose(file) == 0;
}

int save_admin(void)
{
    Account one[1];
    if (!admin_exists) return 1;
    one[0] = admin;
    return save_accounts("file_admin.csv", one, 1);
}

int change_account_password(Account *account, int is_admin)
{
    char email[64];
    char old_password[64];
    char new_password[64];
    char confirmed_password[64];
    char previous_password[64];
    if (input("Email: ", email, sizeof(email)) <= 0) return 0;
    if (strcmp(email, (*account).email) != 0) {
        error_message("Email does not match this account.");
        return 0;
    }
    if (input_password("Old password: ", old_password, sizeof(old_password)) <= 0) return 0;
    if (strcmp(old_password, (*account).password) != 0) {
        error_message("Old password is incorrect.");
        return 0;
    }
    if (input_password("New password: ", new_password, sizeof(new_password)) <= 0) return 0;
    if (input_password("Confirm new password: ", confirmed_password, sizeof(confirmed_password)) <= 0) return 0;
    if (strcmp(new_password, confirmed_password) != 0) {
        error_message("New password and confirmation do not match.");
        return 0;
    }
    strcpy(previous_password, (*account).password);
    strcpy((*account).password, new_password);
    if (!(is_admin ? save_admin() : save_accounts("file_teacher.csv", teachers, teacher_count))) {
        strcpy((*account).password, previous_password);
        error_message("Could not save the new password.");
        return 0;
    }
    printf(GREEN "Password changed successfully.\n" RESETCOLOR);
    return 1;
}

void change_admin_password(void)
{
    change_account_password(&admin, 1);
}

void change_teacher_password(int teacher_index)
{
    if (teacher_index < 0 || teacher_index >= teacher_count) return;
    change_account_password(&teachers[teacher_index], 0);
}

int save_all(void)
{
    return save_students() && save_accounts("file_teacher.csv", teachers, teacher_count) && save_admin();
}

int read_account_row(char *line, Account *account)
{
    char *fields[7];
    char *cursor = line;
    char *end;
    long id;
    size_t length = strlen(line);
    int commas = 0;
    int i;
    memset(account, 0, sizeof(*account));
    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r'))
        line[--length] = '\0';
    for (i = 0; line[i] != '\0'; i++) if (line[i] == ',') commas++;
    if (commas == 3) {
        if (sscanf(line, "%d,%63[^,],%63[^,],%63[^,]", &(*account).id,
            (*account).name, (*account).email, (*account).password) != 4 || (*account).id <= 0) return 0;
        return (*account).name[0] != '\0' && (*account).email[0] != '\0';
    }
    if (commas != 5 && commas != 6) return 0;
    fields[0] = cursor;
    for (i = 0; i < commas; i++) {
        cursor = strchr(cursor, ',');
        if (cursor == NULL) return 0;
        *cursor++ = '\0';
        fields[i + 1] = cursor;
    }
    if (strlen(fields[1]) >= sizeof((*account).name) ||
        strlen(fields[2]) >= sizeof((*account).email) || strlen(fields[3]) >= sizeof((*account).password) ||
        strlen(fields[4]) >= sizeof((*account).sections) || strlen(fields[5]) >= sizeof((*account).subjects)) return 0;
    if (commas == 6 && strlen(fields[6]) >= sizeof((*account).course_codes)) return 0;
    errno = 0;
    id = strtol(fields[0], &end, 10);
    if (errno == ERANGE || fields[0][0] == '\0' || *end != '\0' || id < 1 || id > 2147483647L ||
        fields[1][0] == '\0' || fields[2][0] == '\0') return 0;
    (*account).id = (int)id;
    strcpy((*account).name, fields[1]);
    strcpy((*account).email, fields[2]);
    strcpy((*account).password, fields[3]);
    strcpy((*account).sections, fields[4]);
    strcpy((*account).subjects, fields[5]);
    if (commas == 6) strcpy((*account).course_codes, fields[6]);
    return 1;
}

void load_accounts(const char *filename, Account *accounts, int *count, int maximum)
{
    FILE *file = fopen(filename, "r");
    char line[300];
    if (file == NULL) return;
    fgets(line, sizeof(line), file);
    while (*count < maximum && fgets(line, sizeof(line), file) != NULL) {
        Account account;
        if (read_account_row(line, &account)) accounts[(*count)++] = account;
    }
    fclose(file);
}

int parse_student_row(char *line, Student *student)
{
    char *fields[38];
    char *cursor = line;
    char *end;
    long integer;
    size_t length = strlen(line);
    int field_count = 1;
    int i;
    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r'))
        line[--length] = '\0';
    fields[0] = cursor;
    while (field_count < 38 && (cursor = strchr(cursor, ',')) != NULL) {
        *cursor++ = '\0';
        fields[field_count++] = cursor;
    }
    if (field_count != 14 && field_count != 20 && field_count != 24 && field_count != 38) return 0;
    memset(student, 0, sizeof(*student));
    if (strlen(fields[1]) >= sizeof((*student).name) ||
        strlen(fields[2]) >= sizeof((*student).email) || strlen(fields[3]) >= sizeof((*student).password) ||
        strlen(fields[4]) != 1) return 0;
    errno = 0;
    cursor = strncmp(fields[0], "CT-", 3) == 0 ? fields[0] + 3 : fields[0];
    integer = strtol(cursor, &end, 10);
    if (errno == ERANGE || cursor[0] == '\0' || *end != '\0' || integer < 1 || integer > 2147483647L) return 0;
    (*student).id = (int)integer;
    strcpy((*student).name, fields[1]);
    strcpy((*student).email, fields[2]);
    strcpy((*student).password, fields[3]);
    (*student).section = fields[4][0];
    if ((*student).section < 'A' || (*student).section > 'F') return 0;
    errno = 0;
    integer = strtol(fields[5], &end, 10);
    if (errno == ERANGE || fields[5][0] == '\0' || *end != '\0' || integer < 0 || integer > 2147483647L) return 0;
    (*student).present = (int)integer;
    errno = 0;
    integer = strtol(fields[6], &end, 10);
    if (errno == ERANGE || fields[6][0] == '\0' || *end != '\0' || integer < (*student).present || integer > 2147483647L) return 0;
    (*student).sessions = (int)integer;
    memset((*student).subject_present, 0, sizeof((*student).subject_present));
    memset((*student).subject_sessions, 0, sizeof((*student).subject_sessions));
    if (field_count >= 20) {
        int attendance_count = 6;
        for (i = 0; i < attendance_count; i++) {
            char *slash;
            slash = strchr(fields[7 + i], '/');
            if (slash == NULL) return 0;
            *slash = '\0';
            errno = 0;
            integer = strtol(fields[7 + i], &end, 10);
            if (errno == ERANGE || fields[7 + i][0] == '\0' || *end != '\0' || integer < 0 || integer > 2147483647L) return 0;
            (*student).subject_present[i] = (int)integer;
            errno = 0;
            integer = strtol(slash + 1, &end, 10);
            if (errno == ERANGE || slash[1] == '\0' || *end != '\0' || integer < (*student).subject_present[i] || integer > 2147483647L) return 0;
            (*student).subject_sessions[i] = (int)integer;
        }
    }
    for (i = 0; i < 6; i++) {
        int mark_index;
        double mark;
        int k;
        if (field_count == 38) {
            int entered = 0;
            (*student).marks[i] = 0.0;
            for (k = 0; k < 4; k++) {
                char *f = fields[13 + i * 4 + k];
                if (f[0] == '\0') {
                    (*student).assessments[i][k] = -1.0;
                    continue;
                }
                errno = 0;
                mark = strtod(f, &end);
                if (errno == ERANGE || *end != '\0' || !isfinite(mark) ||
                    mark < 0.0 || mark > assessment_max_marks[k]) return 0;
                (*student).assessments[i][k] = mark;
                (*student).marks[i] += mark;
                entered = 1;
            }
            if (!entered) (*student).marks[i] = -1.0;
            continue;
        } else if (field_count == 24) mark_index = 15 + i;
        else if (field_count == 20) mark_index = 13 + i;
        else if (field_count == 14) mark_index = 7 + i;
        else {
            (*student).marks[i] = 0.0;
            continue;
        }
        errno = 0;
        mark = strtod(fields[mark_index], &end);
        if (errno == ERANGE || fields[mark_index][0] == '\0' || *end != '\0' ||
            !isfinite(mark) || mark < 0.0 || mark > 100.0) return 0;
        (*student).marks[i] = mark;
        for (k = 0; k < 4; k++)
            (*student).assessments[i][k] = mark * assessment_max_marks[k] / 100.0;
    }
    return 1;
}

void load_data(void)
{
    FILE *file;
    char line[2048];
    load_accounts("file_teacher.csv", teachers, &teacher_count, 100);
    {
        Account loaded_admin[1];
        int count = 0;
        load_accounts("file_admin.csv", loaded_admin, &count, 1);
        if (count == 1) {
            admin = loaded_admin[0];
            admin_exists = 1;
        }
    }
    file = fopen("file_student.csv", "r");
    if (file == NULL) return;
    fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file) != NULL) {
        Student s;
        if (parse_student_row(line, &s) && ensure_student_space()) students[student_count++] = s;
    }
    fclose(file);
}

int setup_admin(void)
{
    char email[64];
    char password[64];
    heading("Create first admin account");
    admin.id = 1;
    if (input("Admin name: ", admin.name, sizeof(admin.name)) <= 0) return 0;
    do {
        if (input("Admin Gmail: ", email, sizeof(email)) <= 0) return 0;
        if (!valid_email(email)) error_message("Enter a valid email address.");
    } while (!valid_email(email));
    if (input_password("Admin password: ", password, sizeof(password)) <= 0) return 0;
    strcpy(admin.email, email);
    strcpy(admin.password, password);
    admin_exists = 1;
    if (!save_admin()) {
        error_message("Could not create file_admin.csv.");
        admin_exists = 0;
        return 0;
    }
    printf(GREEN "Admin account created.\n" RESETCOLOR);
    return 1;
}

int authenticate(int role, int *index)
{
    char email[64];
    char password[64];
    int i;
    const char *name = NULL;
    heading("Login");
    if (input("Email: ", email, sizeof(email)) <= 0) return 0;
    if (input_password("Password: ", password, sizeof(password)) <= 0) return 0;
    if (role == 1) {
        if (admin_exists && strcmp(email, admin.email) == 0 && strcmp(password, admin.password) == 0)
            name = admin.name;
    }
    else if (role == 2) {
        for (i = 0; i < teacher_count; i++) {
            if (strcmp(email, teachers[i].email) == 0 && strcmp(password, teachers[i].password) == 0) {
                *index = i;
                name = teachers[i].name;
                break;
            }
        }
    }
    else if (role == 3) {
        for (i = 0; i < student_count; i++) {
            if (strcmp(email, students[i].email) == 0 && strcmp(password, students[i].password) == 0) {
                *index = i;
                name = students[i].name;
                break;
            }
        }
    }
    if (name == NULL) {
        error_message("Wrong Gmail or password for the selected role.");
        return 0;
    }
    printf(GREEN "Welcome, %s!\n" RESETCOLOR, name);
    return 1;
}

int create_account(Account *account, int id, const char *role)
{
    char email[64];
    char password[64];
    char name_prompt[100];
    memset(account, 0, sizeof(*account));
    (*account).id = id;
    snprintf(name_prompt, sizeof(name_prompt), "%s name: ", role);
    if (input(name_prompt, (*account).name, sizeof((*account).name)) <= 0) return 0;
    do {
        if (input("Gmail: ", email, sizeof(email)) <= 0) return 0;
        if (!valid_email(email)) error_message("Enter a valid email address.");
        else if (email_exists(email)) error_message("That email is already in use.");
        else break;
    } while (1);
    if (input_password("Password: ", password, sizeof(password)) <= 0) return 0;
    strcpy((*account).email, email);
    strcpy((*account).password, password);
    return 1;
}

void add_student(void)
{
    char roll_number[32];
    char email[64];
    char password[64];
    char section_text[16];
    int id;
    Student s;
    if (!ensure_student_space()) {
        error_message("Out of memory.");
        return;
    }
    memset(&s, 0, sizeof(s));
    clear_marks(&s);
    for (;;) {
        if (input("Student roll number (for example CT-26001): ", roll_number, sizeof(roll_number)) <= 0) return;
        if (!parse_student_id(roll_number, &id)) {
            error_message("Enter a valid roll number, for example CT-26001.");
            continue;
        }
        if (student_by_id(id) != NULL) {
            error_message("That roll number already exists.");
            continue;
        }
        s.id = id;
        break;
    }
    if (input("Student name: ", s.name, sizeof(s.name)) <= 0) return;
    for (;;) {
        if (input("Section (A-F): ", section_text, sizeof(section_text)) <= 0) return;
        if (section_text[1] == '\0' &&
            toupper((unsigned char)section_text[0]) >= 'A' &&
            toupper((unsigned char)section_text[0]) <= 'F') {
            s.section = (char)toupper((unsigned char)section_text[0]);
            break;
        }
        error_message("Enter a section from A to F.");
    }
    for (;;) {
        if (input("Student email: ", email, sizeof(email)) <= 0) return;
        if (!valid_email(email)) error_message("Enter a valid Gmail address.");
        else if (email_exists(email)) error_message("That email already exists.");
        else break;
    }
    if (input_password("Student password: ", password, sizeof(password)) <= 0) return;
    strcpy(s.email, email);
    strcpy(s.password, password);
    if (!confirm_action("Confirm add student? (Y/N): ")) {
        printf("Student addition cancelled.\n");
        return;
    }
    students[student_count++] = s;
    if (!save_students()) {
        student_count--;
        error_message("Could not save file_student.csv.");
    } else {
        printf(GREEN "Student added with roll number CT-26%03d.\n" RESETCOLOR, s.id);
    }
}

void add_teacher(void)
{
    Account teacher;
    int i;
    int next_id = 1;
    if (teacher_count >= 100) {
        error_message("Teacher limit reached (100).");
        return;
    }
    for (i = 0; i < teacher_count; i++)
        if (teachers[i].id >= next_id) next_id = teachers[i].id + 1;
    if (!create_account(&teacher, next_id, "Teacher")) return;
    if (!confirm_action("Confirm add teacher? (Y/N): ")) {
        printf("Teacher addition cancelled.\n");
        return;
    }
    teachers[teacher_count++] = teacher;
    if (!save_accounts("file_teacher.csv", teachers, teacher_count)) error_message("Could not save file_teacher.csv.");
    else printf(GREEN "Teacher account added.\n" RESETCOLOR);
}

int teacher_index_by_id(int id)
{
    int i;
    for (i = 0; i < teacher_count; i++)
        if (teachers[i].id == id) return i;
    return -1;
}

int valid_teacher_sections(const char *sections)
{
    const char *cursor = sections;
    while (*cursor != '\0') {
        char section = (char)toupper((unsigned char)*cursor++);
        if (section < 'A' || section > 'F') return 0;
        if (*cursor == ';') {
            cursor++;
            if (*cursor == '\0') return 0;
        } else if (*cursor != '\0') {
            return 0;
        }
    }
    return sections[0] != '\0';
}

int email_exists_for_student(const char *email, const Student *current_student)
{
    int i;
    if (admin_exists && strcmp(admin.email, email) == 0) return 1;
    for (i = 0; i < teacher_count; i++)
        if (strcmp(teachers[i].email, email) == 0) return 1;
    for (i = 0; i < student_count; i++)
        if (&students[i] != current_student && strcmp(students[i].email, email) == 0) return 1;
    return 0;
}

int email_exists_for_teacher(const char *email, const Account *current_teacher)
{
    int i;
    if (admin_exists && strcmp(admin.email, email) == 0) return 1;
    for (i = 0; i < teacher_count; i++)
        if (&teachers[i] != current_teacher && strcmp(teachers[i].email, email) == 0) return 1;
    for (i = 0; i < student_count; i++)
        if (strcmp(students[i].email, email) == 0) return 1;
    return 0;
}

int update_student_field(Student *student, int field)
{
    Student previous = *student;
    char text[64];
    int input_result;
    if (field == 1) {
        int id;
        for (;;) {
            if (input("New roll number (for example CT-26001): ", text, sizeof(text)) <= 0) return 0;
            if (!parse_student_id(text, &id)) {
                error_message("Enter a valid roll number, for example CT-26001.");
                continue;
            }
            if (student_by_id(id) != NULL && student_by_id(id) != student) {
                error_message("That roll number already exists.");
                continue;
            }
            (*student).id = id;
            break;
        }
    } else if (field == 2) {
        if (input("New student name: ", text, sizeof(text)) <= 0) return 0;
        strcpy((*student).name, text);
    } else if (field == 3) {
        for (;;) {
            input_result = input("New student Gmail: ", text, sizeof(text));
            if (input_result <= 0) return 0;
            if (!valid_email(text)) error_message("Enter a valid Gmail address.");
            else if (email_exists_for_student(text, student))
                error_message("That email already exists.");
            else break;
        }
        strcpy((*student).email, text);
    } else if (field == 4) {
        if (input_password("New student password: ", text, sizeof(text)) <= 0) return 0;
        strcpy((*student).password, text);
    } else if (field == 5) {
        for (;;) {
            if (input("New section (A-F): ", text, sizeof(text)) <= 0) return 0;
            if (text[1] == '\0' && toupper((unsigned char)text[0]) >= 'A' &&
                toupper((unsigned char)text[0]) <= 'F') {
                (*student).section = (char)toupper((unsigned char)text[0]);
                break;
            }
            error_message("Enter a section from A to F.");
        }
    }
    if (!save_students()) {
        *student = previous;
        error_message("Could not save file_student.csv; student changes were not applied.");
        return 0;
    }
    printf(GREEN "Student account updated.\n" RESETCOLOR);
    return 1;
}

int update_teacher_field(Account *teacher, int field)
{
    Account previous = *teacher;
    char text[64];
    int input_result;
    if (field == 1) {
        int id;
        for (;;) {
            id = number("New teacher account ID: ", 1, 2147483647);
            if (id < 0) return 0;
            if (teacher_index_by_id(id) >= 0 && teacher_index_by_id(id) != (int)(teacher - teachers)) {
                error_message("That teacher account ID already exists.");
                continue;
            }
            (*teacher).id = id;
            break;
        }
    } else if (field == 2) {
        if (input("New teacher name: ", text, sizeof(text)) <= 0) return 0;
        strcpy((*teacher).name, text);
    } else if (field == 3) {
        for (;;) {
            input_result = input("New teacher Gmail: ", text, sizeof(text));
            if (input_result <= 0) return 0;
            if (!valid_email(text)) error_message("Enter a valid Gmail address.");
            else if (email_exists_for_teacher(text, teacher))
                error_message("That email already exists.");
            else break;
        }
        strcpy((*teacher).email, text);
    } else if (field == 4) {
        if (input_password("New teacher password: ", text, sizeof(text)) <= 0) return 0;
        strcpy((*teacher).password, text);
    } else if (field == 5) {
        for (;;) {
            if (input("Assigned sections (for example A;B;C): ", text, sizeof(text)) <= 0) return 0;
            if (valid_teacher_sections(text)) break;
            error_message("Enter one or more sections from A to F separated by semicolons.");
        }
        {
            size_t i;
            for (i = 0; text[i] != '\0'; i++)
                text[i] = (char)toupper((unsigned char)text[i]);
        }
        strcpy((*teacher).sections, text);
    } else if (field == 6) {
        int subject = number("Course: 1) PF  2) FE  3) AP  4) ICP  5) ITFA  6) IS: ", 1, 6);
        const char *subject_labels[6] = {
            "Programming Fundamentals (PF)", "Fundamentals English (FE)", "Applied Physics (AP)",
            "ICP", "ITFA", "IS"
        };
        if (subject < 0) return 0;
        strcpy((*teacher).subjects, subject_labels[subject - 1]);
        strcpy((*teacher).course_codes, course_codes[subject - 1]);
    }
    if (!save_accounts("file_teacher.csv", teachers, teacher_count)) {
        *teacher = previous;
        error_message("Could not save file_teacher.csv; teacher changes were not applied.");
        return 0;
    }
    printf(GREEN "Teacher account updated.\n" RESETCOLOR);
    return 1;
}

Student *student_from_roll_number(const char *roll_number);

void update_account(void)
{
    int kind = number("Update 1) teacher  2) student: ", 1, 2);
    if (kind < 0) return;
    if (kind == 1) {
        int id = number("Teacher account ID: ", 1, 2147483647);
        int teacher_index;
        int field;
        if (id < 0) return;
        teacher_index = teacher_index_by_id(id);
        if (teacher_index < 0) {
            error_message("Teacher not found.");
            return;
        }
        do {
            heading("Update teacher account");
            printf("ID: %d | Name: %s | Email: %s\nSections: %s | Subject: %s | Course code: %s\n",
                teachers[teacher_index].id, teachers[teacher_index].name, teachers[teacher_index].email,
                teachers[teacher_index].sections, teachers[teacher_index].subjects,
                teachers[teacher_index].course_codes);
            printf("1. Account ID\n2. Name\n3. Email\n4. Password\n5. Assigned sections\n6. Subject/course\n0. Done\n");
            field = number("Choose field to update: ", 0, 6);
            if (field > 0) update_teacher_field(&teachers[teacher_index], field);
        } while (field > 0);
    } else {
        char roll_number[32];
        Student *student;
        int field;
        if (input("Student roll number: ", roll_number, sizeof(roll_number)) <= 0) return;
        student = student_from_roll_number(roll_number);
        if (student == NULL) return;
        do {
            heading("Update student account");
            printf("Roll number: CT-26%03d | Name: %s | Email: %s | Section: %c\n",
                (*student).id, (*student).name, (*student).email, (*student).section);
            printf("1. Roll number\n2. Name\n3. Email\n4. Password\n5. Section\n0. Done\n");
            field = number("Choose field to update: ", 0, 5);
            if (field > 0) update_student_field(student, field);
        } while (field > 0);
    }
}

void remove_account(void)
{
    int kind = number("Remove 1) student  2) teacher: ", 1, 2);
    int id;
    int i;
    if (kind < 0) return;
    if (kind == 1) {
        char roll_number[32];
        Student *student;
        if (input("Student Roll Number: ", roll_number, sizeof(roll_number)) <= 0) return;
        student = student_from_roll_number(roll_number);
        if (student == NULL) return;
        if (!confirm_action("Confirm remove this student? (Y/N): ")) {
            printf("Student removal cancelled.\n");
            return;
        }
        i = (int)(student - students);
        for (; i < student_count - 1; i++) students[i] = students[i + 1];
        student_count--;
        if (!save_students()) error_message("Could not save file_student.csv.");
        else printf(GREEN "Student removed.\n" RESETCOLOR);
    } else {
        id = number("Teacher account ID: ", 1, 2147483647);
        if (id < 0) return;
        for (i = 0; i < teacher_count && teachers[i].id != id; i++) {}
        if (i == teacher_count) {
            error_message("Teacher not found.");
            return;
        }
        if (!confirm_action("Confirm remove this teacher? (Y/N): ")) {
            printf("Teacher removal cancelled.\n");
            return;
        }
        for (; i < teacher_count - 1; i++) teachers[i] = teachers[i + 1];
        teacher_count--;
        if (!save_accounts("file_teacher.csv", teachers, teacher_count)) error_message("Could not save file_teacher.csv.");
        else printf(GREEN "Teacher removed.\n" RESETCOLOR);
    }
}

/* ---------- Reports: sort, search, statistics ---------- */
static int out_of_order(const Student *a, const Student *b, int key)
{
    if (key == 1) return cgpa(a) < cgpa(b);
    if (key == 2) return (*a).id > (*b).id;
    if (key == 3) return strcmp((*a).name, (*b).name) > 0;
    return attendance(a) < attendance(b);
}

/* Bubble sort on pointers: students[] order stays same */
void sort_students(Student **a, int n, int key)
{
    int i, j;
    for (i = 0; i < n - 1; i++)
        for (j = 0; j < n - 1 - i; j++)
            if (out_of_order(a[j], a[j + 1], key)) {
                Student *t = a[j];
                a[j] = a[j + 1];
                a[j + 1] = t;
            }
}

Student **make_list(void)
{
    int i;
    Student **a = malloc((student_count ? student_count : 1) * sizeof(Student *));
    if (a == NULL) return NULL;
    for (i = 0; i < student_count; i++) a[i] = &students[i];
    return a;
}

/* Ties: same CGPA (2 decimals) share one rank */
void show_list(Student **a, int n, int ranked)
{
    int i, rank = 0;
    printf("%-5s %-11s %-30s %-4s %-6s %s\n", ranked ? "Rank" : "No.", "Roll", "Name", "Sec", "CGPA", "Att%");
    for (i = 0; i < n; i++) {
        if (i == 0 || (int)(cgpa(a[i]) * 100 + 0.5) != (int)(cgpa(a[i - 1]) * 100 + 0.5)) rank = i + 1;
        printf("%-5d CT-26%03d  %-30s %-4c %-6.2f %.1f\n", ranked ? rank : i + 1,
            (*a[i]).id, (*a[i]).name, (*a[i]).section, cgpa(a[i]), attendance(a[i]));
    }
}

/* Recursive binary search on list sorted by id */
int find_id(Student **a, int low, int high, int id)
{
    int mid;
    if (low > high) return -1;
    mid = (low + high) / 2;
    if ((*a[mid]).id == id) return mid;
    if ((*a[mid]).id < id) return find_id(a, mid + 1, high, id);
    return find_id(a, low, mid - 1, id);
}

/* Recursive sum of one subject's marks over first n students */
double sum_marks(int n, int subject)
{
    if (n == 0) return 0.0;
    return shown(students[n - 1].marks[subject]) + sum_marks(n - 1, subject);
}

static int has_text(const char *text, const char *part)
{
    size_t i, j, n = strlen(part), len = strlen(text);
    for (i = 0; i + n <= len; i++) {
        for (j = 0; j < n && tolower((unsigned char)text[i + j]) == tolower((unsigned char)part[j]); j++) {}
        if (j == n) return 1;
    }
    return 0;
}

void sort_report(void)
{
    Student **a;
    int key;
    if (student_count == 0) { error_message("No students yet."); return; }
    printf("1. CGPA (class ranking)\n2. Roll number\n3. Name\n4. Attendance\n");
    key = number("Sort by: ", 1, 4);
    if (key < 0) return;
    a = make_list();
    if (a == NULL) { error_message("Out of memory."); return; }
    sort_students(a, student_count, key);
    show_list(a, student_count, key == 1);
    free(a);
}

void find_roll(void)
{
    char text[32];
    int id, pos;
    Student **a;
    if (input("Roll number: ", text, sizeof(text)) <= 0) return;
    if (!parse_student_id(text, &id)) { error_message("Bad roll number."); return; }
    a = make_list();
    if (a == NULL) { error_message("Out of memory."); return; }
    sort_students(a, student_count, 2);
    pos = find_id(a, 0, student_count - 1, id);
    if (pos < 0) error_message("Student not found.");
    else show_list(a + pos, 1, 0);
    free(a);
}

void search_name(void)
{
    char part[64];
    int i, found = 0;
    if (input("Name (part is ok): ", part, sizeof(part)) <= 0) return;
    for (i = 0; i < student_count; i++) {
        if (has_text(students[i].name, part)) {
            printf("CT-26%03d  %-30s Sec %c  CGPA %.2f  Att %.1f%%\n", students[i].id,
                students[i].name, students[i].section, cgpa(&students[i]), attendance(&students[i]));
            found++;
        }
    }
    if (!found) error_message("No match.");
}

void subject_stats(void)
{
    int s, i, n;
    if (student_count == 0) { error_message("No students yet."); return; }
    printf("%-26s %8s %8s %8s %8s\n", "Subject", "Average", "Highest", "Lowest", "StdDev");
    for (s = 0; s < 6; s++) {
        double avg, high = 0.0, low = 100.0, var = 0.0;
        n = 0;
        for (i = 0; i < student_count; i++) {
            double m = students[i].marks[s];
            if (m < 0.0) continue;
            n++;
            if (m > high) high = m;
            if (m < low) low = m;
        }
        if (n == 0) { printf("%-26s no marks yet\n", subject_names[s]); continue; }
        avg = sum_marks(student_count, s) / n;
        for (i = 0; i < student_count; i++)
            if (students[i].marks[s] >= 0.0) var += (students[i].marks[s] - avg) * (students[i].marks[s] - avg);
        printf("%-26s %8.2f %8.2f %8.2f %8.2f\n", subject_names[s], avg, high, low, sqrt(var / n));
    }
}

/* Fail = any subject below 50. Probation = CGPA below 2.00 */
void pass_fail(void)
{
    int i, s, pass = 0, fail = 0, probation = 0, none = 0;
    for (i = 0; i < student_count; i++) {
        int failed = 0;
        if (!has_marks(&students[i])) { none++; continue; }
        for (s = 0; s < 6; s++) if (students[i].marks[s] >= 0.0 && grade_points(students[i].marks[s]) == 0.0) failed = 1;
        if (failed) fail++; else pass++;
    }
    printf("Passed all subjects: %d | Failed 1+ subject: %d | No marks yet: %d\n", pass, fail, none);
    printf("Academic probation (CGPA below 2.00):\n");
    for (i = 0; i < student_count; i++) {
        if (has_marks(&students[i]) && cgpa(&students[i]) < 2.0) {
            printf("  CT-26%03d  %-30s CGPA %.2f\n", students[i].id, students[i].name, cgpa(&students[i]));
            probation++;
        }
    }
    if (!probation) printf("  None.\n");
}

void reports_menu(void)
{
    int choice;
    do {
        heading("Reports and Search");
        printf("1. Sort / rank students\n2. Find by roll number\n3. Search by name\n"
            "4. Subject statistics\n5. Pass/fail and probation\n0. Back\n");
        choice = number("Choose: ", 0, 5);
        switch (choice) {
        case 1: sort_report(); break;
        case 2: find_roll(); break;
        case 3: search_name(); break;
        case 4: subject_stats(); break;
        case 5: pass_fail(); break;
        case 0: break;
        }
    } while (choice > 0);
}

void admin_menu(void)
{
    int choice;
    do {
        heading("Admin: manage accounts");
        printf("1. Add student\n2. Remove student or teacher\n3. Add teacher\n4. Update account\n5. Change password\n6. Reports and search\n0. Log out\n");
        choice = number("Choose: ", 0, 6);
        switch (choice) {
        case 1: add_student(); break;
        case 2: remove_account(); break;
        case 3: add_teacher(); break;
        case 4: update_account(); break;
        case 5: change_admin_password(); break;
        case 6: reports_menu(); break;
        case 0: break;
        }
    } while (choice > 0);
}

Student *student_from_roll_number(const char *roll_number)
{
    int id;
    if (!parse_student_id(roll_number, &id)) {
        error_message("Enter a valid roll number, for example CT-26001 or CT-001.");
        return NULL;
    }
    {
        Student *student = student_by_id(id);
        if (student == NULL) error_message("Student not found.");
        return student;
    }
}

Student *select_teacher_student(void)
{
    char roll_number[32];
    if (input("Roll Number: ", roll_number, sizeof(roll_number)) <= 0) return NULL;
    return student_from_roll_number(roll_number);
}

int subject_index_from_course_code(const char *course_code);

int teacher_subject_index(const Account *teacher)
{
    int i;
    if ((*teacher).course_codes[0] != '\0') return subject_index_from_course_code((*teacher).course_codes);
    for (i = 0; i < 6; i++) {
        if (i == 0 && strstr((*teacher).subjects, "PF") != NULL) return i;
        if (i == 1 && strstr((*teacher).subjects, "FE") != NULL) return i;
        if (i == 2 && strstr((*teacher).subjects, "AP") != NULL) return i;
        if (i == 3 && strstr((*teacher).subjects, "ICP") != NULL) return i;
        if (i == 4 && strstr((*teacher).subjects, "ITFA") != NULL) return i;
        if (i == 5 && strstr((*teacher).subjects, "IS") != NULL) return i;
    }
    return -1;
}

int subject_index_from_course_code(const char *course_code)
{
    int i;
    for (i = 0; i < 6; i++)
        if (strcmp(course_code, course_codes[i]) == 0) return i;
    return -1;
}

int teacher_has_course_code(const Account *teacher, const char *course_code)
{
    const char *cursor = (*teacher).course_codes;
    size_t code_length = strlen(course_code);
    while (*cursor != '\0') {
        const char *separator = strchr(cursor, ';');
        size_t token_length = separator == NULL ? strlen(cursor) : (size_t)(separator - cursor);
        if (token_length == code_length && strncmp(cursor, course_code, code_length) == 0) return 1;
        if (separator == NULL) break;
        cursor = separator + 1;
    }
    if ((*teacher).course_codes[0] == '\0') {
        int subject = teacher_subject_index(teacher);
        return subject >= 0 && subject_index_from_course_code(course_code) == subject;
    }
    return 0;
}

int choose_teacher_course(const Account *teacher)
{
    int subject;
    subject = teacher_subject_index(teacher);
    if (subject < 0 || !teacher_has_course_code(teacher, course_codes[subject])) {
        error_message("No valid course is assigned to this teacher.");
        return -1;
    }
    printf("Course: %s | Course code: %s\n", subject_names[subject], course_codes[subject]);
    return subject;
}

int teacher_has_section(const Account *teacher, char section)
{
    char section_text[2] = {section, '\0'};
    const char *match = strstr((*teacher).sections, section_text);
    while (match != NULL) {
        int left_ok = match == (*teacher).sections || match[-1] == ';';
        int right_ok = match[1] == '\0' || match[1] == ';';
        if (left_ok && right_ok) return 1;
        match = strstr(match + 1, section_text);
    }
    return 0;
}

void update_attendance(int teacher_index)
{
    int subject_index;
    int section_number;
    int expected = 0;
    int marked = 0;
    int roll_id;
    Account *teacher;
    if (teacher_index < 0 || teacher_index >= teacher_count) return;
    teacher = &teachers[teacher_index];
    subject_index = choose_teacher_course(teacher);
    if (subject_index < 0) return;
    printf("Assigned sections: %s\n", (*teacher).sections);
    section_number = number("Section (A=1, B=2, C=3, D=4, E=5, F=6): ", 1, 6);
    if (section_number < 0) return;
    {
        char section = (char)('A' + section_number - 1);
        if (!teacher_has_section(teacher, section)) {
            error_message("This teacher is not assigned to that section.");
            return;
        }
        for (roll_id = 1; roll_id <= max_student_id(); roll_id++) {
            Student *student = student_by_id(roll_id);
            if (student != NULL && (*student).section == section) expected++;
        }
        if (expected == 0) {
            error_message("No students are registered in this section.");
            return;
        }
        printf("Roll call for Section %c, %s (%d students). Enter P or A.\n", section,
            subject_names[subject_index], expected);
        for (roll_id = 1; roll_id <= max_student_id(); roll_id++) {
            Student *student = student_by_id(roll_id);
            char prompt[128];
            char response[16];
            char attendance_code;
            if (student == NULL || (*student).section != section) continue;
            snprintf(prompt, sizeof(prompt), "CT-26%03d %-30s [P/A]: ", (*student).id, (*student).name);
            for (;;) {
                int input_result = input(prompt, response, sizeof(response));
                if (input_result == 0) return;
                if (input_result < 0) continue;
                attendance_code = (char)toupper((unsigned char)response[0]);
                if (response[1] == '\0' && (attendance_code == 'P' || attendance_code == 'A')) break;
                error_message("Enter P for present or A for absent.");
            }
            (*student).subject_sessions[subject_index]++;
            (*student).sessions++;
            if (attendance_code == 'P') {
                (*student).subject_present[subject_index]++;
                (*student).present++;
            }
            if (!save_students()) {
                error_message("Could not save attendance to file_student.csv.");
                return;
            }
            marked++;
        }
    }
    printf(GREEN "Attendance saved for %d students.\n" RESETCOLOR, marked);
}

int enter_student_mark(Student *student, int subject)
{
    char text[100];
    char *end;
    static const int assessment_order[4] = {3, 0, 1, 2};
    int assessment_index;
    int choice;
    double total_mark = 0.0;
    int i;
    double current_attendance = (*student).subject_sessions[subject] == 0 ? 0.0 :
        100.0 * (*student).subject_present[subject] / (*student).subject_sessions[subject];
    printf("Current %s mark: %.1f/100 | Attendance: %.1f%% (%d/%d)\n",
        subject_names[subject], shown((*student).marks[subject]), current_attendance,
        (*student).subject_present[subject], (*student).subject_sessions[subject]);
    for (i = 0; i < 4; i++) {
        int component = assessment_order[i];
        printf("%s (%.1f/%.0f)%s", assessment_names[component],
            shown((*student).assessments[subject][component]), assessment_max_marks[component],
            i == 3 ? "\n" : " | ");
    }
    choice = number("Update: 1) Final  2) Assignments  3) Quizzes  4) Midterm: ", 1, 4);
    if (choice < 0) return 0;
    assessment_index = assessment_order[choice - 1];
    for (;;) {
        int input_result;
        char prompt[100];
        snprintf(prompt, sizeof(prompt), "%s mark (0-%.0f): ",
            assessment_names[assessment_index], assessment_max_marks[assessment_index]);
        input_result = input(prompt, text, sizeof(text));
        if (input_result == 0) return 0;
        if (input_result < 0) continue;
        errno = 0;
        total_mark = strtod(text, &end);
        while (isspace((unsigned char)*end)) end++;
        if (text[0] && *end == '\0' && errno != ERANGE && isfinite(total_mark) &&
            total_mark >= 0 && total_mark <= assessment_max_marks[assessment_index]) break;
        printf(RED "Enter a mark from 0 to %.0f.\n" RESETCOLOR,
            assessment_max_marks[assessment_index]);
    }
    {
        double previous_assessment = (*student).assessments[subject][assessment_index];
        double previous_total = (*student).marks[subject];
        (*student).assessments[subject][assessment_index] = total_mark;
        (*student).marks[subject] = 0.0;
        for (i = 0; i < 4; i++) (*student).marks[subject] += shown((*student).assessments[subject][i]);
        total_mark = (*student).marks[subject];
        if (!save_students()) {
            (*student).assessments[subject][assessment_index] = previous_assessment;
            (*student).marks[subject] = previous_total;
            error_message("Could not save marks to file_student.csv.");
            return 0;
        }
    }
    printf("Saved mark for CT-26%03d (%s): %.1f/100. Current CGPA: %.2f\n",
        (*student).id, (*student).name, total_mark, cgpa(student));
    return 1;
}

void view_teacher_section_records(int teacher_index, int show_attendance)
{
    int subject;
    int section_number;
    int roll_id;
    int found = 0;
    char section;
    Account *teacher;
    if (teacher_index < 0 || teacher_index >= teacher_count) return;
    teacher = &teachers[teacher_index];
    subject = choose_teacher_course(teacher);
    if (subject < 0) return;
    section_number = number("Section (A=1, B=2, C=3, D=4, E=5, F=6): ", 1, 6);
    if (section_number < 0) return;
    section = (char)('A' + section_number - 1);
    if (!teacher_has_section(teacher, section)) {
        error_message("This teacher is not assigned to that section.");
        return;
    }
    printf("Course: %s | Course code: %s\nSection %c current %s:\n",
        subject_names[subject], course_codes[subject], section,
        show_attendance ? "attendance" : "marks");
    for (roll_id = 1; roll_id <= max_student_id(); roll_id++) {
        Student *student = student_by_id(roll_id);
        if (student == NULL || (*student).section != section) continue;
        if (show_attendance) {
            double percent = (*student).subject_sessions[subject] == 0 ? 0.0 :
                100.0 * (*student).subject_present[subject] / (*student).subject_sessions[subject];
            printf("CT-26%03d %-30s Attendance: %5.1f%% (%d/%d)\n",
                (*student).id, (*student).name, percent,
                (*student).subject_present[subject], (*student).subject_sessions[subject]);
        } else {
            printf("CT-26%03d %-30s Mark: %5.1f/100 (Assignments %.1f/10, Quizzes %.1f/10, "
                "Midterms %.1f/20, Finals %.1f/60)\n",
                (*student).id, (*student).name, shown((*student).marks[subject]),
                shown((*student).assessments[subject][0]), shown((*student).assessments[subject][1]),
                shown((*student).assessments[subject][2]), shown((*student).assessments[subject][3]));
        }
        found++;
    }
    if (found == 0) error_message("No students are registered in this section.");
}

void update_marks(int teacher_index)
{
    int subject;
    int mode;
    Account *teacher;
    if (teacher_index < 0 || teacher_index >= teacher_count) return;
    teacher = &teachers[teacher_index];
    mode = number("Update marks: 1) one student  2) whole section: ", 1, 2);
    if (mode < 0) return;
    subject = choose_teacher_course(teacher);
    if (subject < 0) return;
    if (mode == 1) {
        Student *student = select_teacher_student();
        if (student == NULL) return;
        if (!teacher_has_section(teacher, (*student).section)) {
            error_message("This teacher is not assigned to the student's section.");
            return;
        }
        enter_student_mark(student, subject);
    } else {
        int section_number = number("Section (A=1, B=2, C=3, D=4, E=5, F=6): ", 1, 6);
        int roll_id;
        int updated = 0;
        char section;
        if (section_number < 0) return;
        section = (char)('A' + section_number - 1);
        if (!teacher_has_section(teacher, section)) {
            error_message("This teacher is not assigned to that section.");
            return;
        }
        for (roll_id = 1; roll_id <= max_student_id(); roll_id++) {
            Student *student = student_by_id(roll_id);
            if (student == NULL || (*student).section != section) continue;
            printf("CT-26%03d %s\n", (*student).id, (*student).name);
            if (!enter_student_mark(student, subject)) return;
            updated++;
        }
        if (updated == 0) error_message("No students are registered in this section.");
        else printf(GREEN "Marks updated for %d students in Section %c.\n" RESETCOLOR, updated, section);
    }
}

void teacher_menu(int teacher_index)
{
    int choice;
    do {
        heading("Teacher Menu");
        printf("1. Update attendance\n2. Update marks\n3. View current attendance\n"
            "4. View current mark of student\n5. Change password\n6. Reports and search\n0. Log out\n");
        choice = number("Choose: ", 0, 6);
        switch (choice) {
        case 1: update_attendance(teacher_index); break;
        case 2: update_marks(teacher_index); break;
        case 3: view_teacher_section_records(teacher_index, 1); break;
        case 4: view_teacher_section_records(teacher_index, 0); break;
        case 5: change_teacher_password(teacher_index); break;
        case 6: reports_menu(); break;
        case 0: break;
        }
    } while (choice > 0);
}

void show_timetable(const Student *s)
{
    int section_index = (*s).section >= 'A' && (*s).section <= 'F' ? (*s).section - 'A' : 0;
    int day;
    printf("Section %c timetable:\n", (*s).section);
    for (day = 0; day < 5; day++) {
        printf("%-10s %s\n", days_of_week[day], timetable[section_index][day]);
    }
}

void check_scholarship(const Student *s)
{
    double current_cgpa = cgpa(s);
    double current_attendance = attendance(s);
    printf("CGPA: %.2f\nAttendance: %.1f%%\n", current_cgpa, current_attendance);
    if (current_cgpa > 3.7 && current_attendance > 80.0)
        printf(GREEN "Scholarship eligible.\n" RESETCOLOR);
    else
        printf("Not currently eligible. Requirements: CGPA above 3.70 and attendance above 80%%.\n");
}

void change_student_password(Student *student)
{
    char email[64];
    char old_password[64];
    char new_password[64];
    char confirmed_password[64];
    char previous_password[64];
    if (input("Email: ", email, sizeof(email)) <= 0) return;
    if (strcmp(email, (*student).email) != 0) {
        error_message("Email does not match this student account.");
        return;
    }
    if (input_password("Old password: ", old_password, sizeof(old_password)) <= 0) return;
    if (strcmp(old_password, (*student).password) != 0) {
        error_message("Old password is incorrect.");
        return;
    }
    if (input_password("New password: ", new_password, sizeof(new_password)) <= 0) return;
    if (input_password("Confirm new password: ", confirmed_password, sizeof(confirmed_password)) <= 0) return;
    if (strcmp(new_password, confirmed_password) != 0) {
        error_message("New password and confirmation do not match.");
        return;
    }
    strcpy(previous_password, (*student).password);
    strcpy((*student).password, new_password);
    if (!save_students()) {
        strcpy((*student).password, previous_password);
        error_message("Could not save the new password.");
        return;
    }
    printf(GREEN "Password changed successfully.\n" RESETCOLOR);
}

void student_menu(Student *s)
{
    int choice;
    do {
        heading("Student Dashboard");
        printf("Welcome %s | Section %c\n", (*s).name, (*s).section);
        printf("1. View attendance\n2. View marks\n3. View timetable\n4. Check scholarship eligibility\n5. Change password\n0. Log out\n");
        choice = number("Choose: ", 0, 5);
        switch (choice) {
        case 1: {
            int i;
            printf("Attendance: %.1f%% (%d/%d)\n", attendance(s), (*s).present, (*s).sessions);
            for (i = 0; i < 6; i++) {
                double subject_attendance = (*s).subject_sessions[i] == 0 ? 0.0 :
                    100.0 * (*s).subject_present[i] / (*s).subject_sessions[i];
                printf("%-28s Attendance: %5.1f%% (%d/%d)\n",
                    subject_names[i], subject_attendance,
                    (*s).subject_present[i], (*s).subject_sessions[i]);
            }
            break;
        }
        case 2: {
            int i;
            for (i = 0; i < 6; i++) {
                if ((*s).marks[i] < 0.0) {
                    printf("%-28s Not entered yet\n", subject_names[i]);
                    continue;
                }
                printf("%-28s Total: %5.1f/100 | Assignments: %4.1f/10 | Quizzes: %4.1f/10 | "
                    "Midterms: %4.1f/20 | Finals: %4.1f/60 | GPA: %.2f\n",
                    subject_names[i], (*s).marks[i], (*s).assessments[i][0],
                    (*s).assessments[i][1], (*s).assessments[i][2], (*s).assessments[i][3],
                    grade_points((*s).marks[i]));
            }
            printf("CGPA: %.2f\n" RESETCOLOR, cgpa(s));
            break;
        }
        case 3: show_timetable(s); break;
        case 4: check_scholarship(s); break;
        case 5: change_student_password(s); break;
        case 0: break;
        }
    } while (choice > 0);
}

int main(void)
{
    int role;
    load_data();
    do {
        int index = 0;
        int authenticated;
        heading("Student Management System");
        printf("1. Admin\n2. Teacher\n3. Student\n0. Exit\n");
        role = number("Choose login type: ", 0, 3);
        if (role <= 0) break;
        if (role == 1 && !admin_exists) {
            if (!setup_admin()) continue;
            printf(GREEN "Welcome, %s!\n" RESETCOLOR, admin.name);
            authenticated = 1;
        } else {
            authenticated = authenticate(role, &index);
        }
        if (!authenticated) continue;
        if (role == 1) admin_menu();
        else if (role == 2) teacher_menu(index);
        else student_menu(&students[index]);
    } while (role != 0);
    if (!save_all()) error_message("One or more CSV files could not be saved.");
    else printf(GREEN "Saved Excel-readable CSV files. Goodbye.\n" RESETCOLOR);
    free(students);
    return 0;
}
