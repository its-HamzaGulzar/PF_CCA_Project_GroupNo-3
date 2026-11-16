# University Student Management System (CT-175 CCA Project)

A console-based student management system written in C. It has three roles: **Admin**, **Teacher** and **Student**. It manages student and teacher accounts, attendance, marks, weighted CGPA, scholarship eligibility, timetables, sorting, searching and class reports. All data is stored in CSV files.

## Group Members

| S. No. | Name | Roll Number | Section |
|---|---|---|---|
| 1 | Syed Humayun Ali | CT-26242 | E |
| 2 | Muhammad Hamza Gulzar | CT-26238 | E |
| 3 | Muhammad Zain ul Abidin | CT-26227 | E |
| 4 | Uzair Asim Hassan | CT-26228 | E |

## Features

- **Admin:** add, remove and update students and teachers; sort students (CGPA, roll number, name, attendance); search by roll number or name; subject statistics; pass/fail and academic probation report; change password.
- **Teacher:** record attendance and marks for their own course and sections; view section records; change password.
- **Student:** view attendance, marks, CGPA, timetable and scholarship eligibility; change password.

## Folder Structure

```
Project_PF/
├── Source_Code/
│   ├── main.c              program source
│   ├── file_admin.csv      admin account (data)
│   ├── file_teacher.csv    teacher accounts (data)
│   ├── file_student.csv    student records, attendance, marks (data)
│   └── .vscode/tasks.json  VS Code build settings
├── Screenshots/            screenshots of the working program
├── Flowchart.pdf
├── Project_Report.pdf
└── README.md
```

## How to Compile and Run (Windows, GCC / MSYS2 UCRT64)

From the `Project_PF` folder:

```bash
gcc Source_Code/main.c -o Source_Code/project.exe -lm
cd Source_Code
project.exe
```

The program must be run from the `Source_Code` folder, because it reads and writes the CSV files in the current folder. Use a Windows machine: the program uses `<conio.h>` for hidden password input.

## Sample Logins

| Role | Email | Password |
|---|---|---|
| Admin | admin@example.com | adminpass |
| Teacher | abdulkarimkazi@gmail.com | AbdulCSIT |
| Student | nehalashfaque@gmail.com | CT-002CSIT |

## PF Concepts Used

- Variables, data types and operators (marks, CGPA weighting, attendance percentage)
- Input and output with formatted tables
- `if / else` and nested conditions (grade points, validation, scholarship rule)
- `switch` (menus, role selection, subject selection)
- Loops (menus with `do-while`, file reading, sorting, section traversal)
- 1D arrays (marks, credit hours, teachers, students)
- 2D arrays (timetable, assessments per subject)
- Strings (`fgets`, `strcmp`, `strstr`, case-insensitive name search)
- Functions (about 85, each with one task, with parameters and return values)
- Pointers (update functions, sorting arrays of pointers, dynamic memory)
- Recursion (binary search by roll number, recursive sum of marks)
- Structures (`Student`, `Account`)
- Sorting (bubble sort) and searching (linear and recursive binary search)
- File handling (save and load CSV)
- Dynamic memory (`malloc`, `realloc`, `free`)
- Error handling (invalid input, duplicate IDs and emails, failed saves)

## Notes

- Passwords are stored as plain text in the CSV files. This is acceptable for a course project but is not secure.
- Marks not yet entered are stored as blank and shown as "Not entered yet". They are not counted as zero.
