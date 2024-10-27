#ifndef EMPLOYEE_H
#define EMPLOYEE_H

struct Employee {
    int id; 
    char name[25];
    char gender; 
    int age; 
    char login[30]; 
    char password[30]; 
    int employeeType; 
    // bool isLoggedIn;
};

#endif 