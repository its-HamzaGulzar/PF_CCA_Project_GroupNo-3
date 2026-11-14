# University Student Management System

A console-based University Student Management System written in **C**.
It supports three roles (Admin, Teacher and Student) and stores all records in CSV files so data is kept between runs.

**Course:** Programming Fundamentals (CT-175) – Complex Computing Activity (CCA)
**Instructor:** Ms. Maham Ashraf
**Semester:** Fall 2026

## Group Members (Group No. 3, Section E)

| S. No. | Name | Roll Number |
|--------|------|-------------|
| 1 | Syed Humayun Ali | CT-26242 |
| 2 | Muhammad Hamza Gulzar | CT-26238 |
| 3 | Muhammad Zain ul Abidin | CT-26227 |
| 4 | Uzair Asim Hassan | CT-26228 |

## Features

- **Login and roles:** Admin, Teacher and Student log in with email and password. The first run creates the admin account.
- **Student registration:** Unique roll number (CT-26xxx format) and unique Gmail address, with confirmation.
- **Account management:** Add, update and remove students and teachers. Change password with an old-password check.
- **Teacher access:** Teachers see and change only their own course and sections.
- **Attendance:** Mark each student P or A per session. The percentage is calculated automatically.
- **Marks entry:** Assignments (10), Quizzes (10), Midterm (20) and Final (60), for one student or a whole section. Marks above the maximum are rejected.
- **GPA and CGPA:** Grade points weighted by credit hours across six subjects.
- **Scholarship check:** Eligible if CGPA is above 3.70 and attendance is above 80%.
- **Timetable:** Weekly timetable for each section.
- **Sort and rank:** By CGPA, roll number, name or attendance. Equal CGPA shares a rank.
- **Search:** By roll number (recursive binary search) or by part of a name.
- **Reports:** Subject statistics (average, highest, lowest, standard deviation) and pass/fail with academic probation (CGPA below 2.00).
- **Input validation** on every menu and field.
- **File save and load:** All records are saved to CSV on every change and at exit.

## How to Compile and Run

Requires a C compiler such as GCC (the program uses a Windows-only password input, so build it on Windows).

```bash
gcc main.c -o project
./project
```

On Windows PowerShell, run `.\project.exe` instead.
Keep the three CSV files in the same folder as the program. If the admin file is missing, the program asks you to create the first admin account.

## Sample Input and Output

**1. Main menu and invalid input**

```
=== Student Management System ===
1. Admin
2. Teacher
3. Student
0. Exit
Choose login type: 9
Enter a whole number from 0 to 3.
Choose login type:
```

**2. Admin login**

```
Choose login type: 1

=== Login ===
Email: admin@example.com
Password: *********
Welcome, Admin One!

=== Admin: manage accounts ===
1. Add student
2. Remove student or teacher
3. Add teacher
4. Update account
5. Change password
6. Reports and search
0. Log out
Choose:
```

**3. Duplicate roll number rejected**

```
Choose: 1
Student roll number (for example CT-26001): CT-26001
That roll number already exists.
Student roll number (for example CT-26001):
```

**4. Student scholarship check**

```
=== Student Dashboard ===
Welcome SYEDA KASHAF FATIMA JAFFRI | Section A
1. View attendance
2. View marks
3. View timetable
4. Check scholarship eligibility
5. Change password
0. Log out
Choose: 4
CGPA: 3.52
Attendance: 81.2%
Not currently eligible. Requirements: CGPA above 3.70 and attendance above 80%.
```

## PF Concepts Used

| Concept | Where it is used |
|---------|------------------|
| Variables and data types | Marks (double), counts (int), IDs, CGPA |
| Conditional statements (if / switch) | Grade points, scholarship rule, validation, role menus |
| Loops | Menus (do-while), file reading, sorting, section traversal |
| Functions | About 85 functions, one task each (`save_students`, `cgpa`, `find_id`, ...) |
| Arrays and strings | `teachers[100]`, `marks[6]`, `timetable[6][5]`, `fgets`, `strcmp`, `strstr` |
| Structures | `Student` and `Account` structs |
| Pointers and dynamic memory | `Student *students` with `malloc`/`realloc`/`free`, pointer parameters, pointer arrays for sorting |
| Recursion | `find_id` (binary search), `sum_marks` (recursive sum) |
| Sorting and searching | Bubble sort by four keys, linear search by ID, recursive binary search |
| File handling | Read and write `file_student.csv`, `file_teacher.csv`, `file_admin.csv` |

## Folder Structure

```
PF_CCA_Project_GroupNo-3/
├── main.c               # Source code
├── file_admin.csv       # Admin accounts
├── file_teacher.csv     # Teacher accounts
├── file_student.csv     # Student records
├── Project_Report.pdf   # Full project report
├── Flowchart.pdf        # Main program flowchart
├── screenshots/         # Program screenshots
└── README.md
```

## Limitations and Future Work

- Passwords are stored as plain text. A future version should store hashes.
- Password input uses a Windows-only function. A portable version is planned.
