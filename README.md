# University Student Management System

A console-based Student Management System written in **C**.
Programming Fundamentals (CT-175) – Complex Computing Activity (CCA), Fall 2026.

**Instructor:** Ms. Maham Ashraf
**Group No. 3 – Section E**

| Name | Roll Number |
|------|-------------|
| Syed Humayun Ali | CT-26242 |
| Muhammad Hamza Gulzar | CT-26238 |
| Muhammad Zain ul Abidin | CT-26227 |
| Uzair Asim Hassan | CT-26228 |

## About

The program supports three roles: **Admin**, **Teacher** and **Student**.
Records are saved in CSV files, so data is kept between runs.

## Features

- Login by email and password for Admin, Teacher and Student
- Add, update and remove student and teacher accounts (unique roll number and Gmail checks)
- Change password with an old-password check
- Teachers record attendance (P/A) and marks only for their own course and sections
- Weighted GPA and CGPA calculation based on credit hours
- Scholarship check (CGPA above 3.70 and attendance above 80%)
- Weekly timetable for each section
- Sort and rank students by CGPA, roll number, name or attendance (bubble sort)
- Search by roll number (recursive binary search) or by part of a name
- Reports: subject statistics, pass/fail list and academic probation (CGPA below 2.00)
- Input validation on every menu and field
- Automatic save and load from CSV files

## Programming Concepts Used

Variables and data types, if/switch, loops, functions (about 85), arrays and strings,
structures, pointers and dynamic memory (malloc/realloc/free), recursion,
sorting and searching, and file handling.

## Files

| File | Purpose |
|------|---------|
| `project.c` | Source code |
| `file_admin.csv` | Admin accounts |
| `file_teacher.csv` | Teacher accounts |
| `file_student.csv` | Student records |
| `Project_Report.pdf` | Full project report |
| `Flowchart.pdf` | Main program flowchart |

## How to Run

Requires a C compiler such as GCC (Windows).

```bash
gcc project.c -o project
./project
```

On the first run, if no admin account exists, the program asks you to create one.

## Screenshots

Add the images to a `screenshots` folder and link them here:

```markdown
![Main menu](screenshots/main_menu.png)
![Student dashboard](screenshots/student_dashboard.png)
![Admin menu](screenshots/admin_menu.png)
```

## Limitations and Future Work

- Passwords are stored as plain text. A future version should store hashes.
- Password input uses a Windows-only function. A portable version is planned.
