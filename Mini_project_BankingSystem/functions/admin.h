#ifndef ADMIN_FUNCTIONS
#define ADMIN_FUNCTIONS

#include "./common.h"
#include "./server-constants.h"
#include "../recordtypes/employee.h"
// Function Prototypes =================================

bool admin_operation_handler(int connFD);
bool modify_customer_info(int connFD);
int add_employee(int connFD);
bool modify_employee_role(int connFD);
bool find_employee_by_id(int employeeId, struct Employee *employee);
bool update_employee_role(int employeeId, int newRole);
void change_admin_password(int connFD);

bool modify_employee_info(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    struct Employee employee;
    int employeeID;
    off_t offset;

    // Ask for Employee ID
    writeBytes = write(connFD, "Enter the Employee ID to modify: ", 34);    
    if (writeBytes == -1) {
        perror("Error while writing message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error while reading employee ID from client!");
        return false;
    }

    employeeID = atoi(readBuffer);

    int employeeFileDescriptor = open(EMPLOYEE_FILE, O_RDONLY);
    if (employeeFileDescriptor == -1) {
        perror("Employee File doesn't exist");
        return false;
    }

    offset = lseek(employeeFileDescriptor, employeeID * sizeof(struct Employee), SEEK_SET);
    if (offset == -1) {
        perror("Error while seeking to required employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Employee), getpid()};

    // Lock the record to be read
    int lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1) {
        perror("Couldn't obtain lock on employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    readBytes = read(employeeFileDescriptor, &employee, sizeof(struct Employee));
    if (readBytes == -1) {
        perror("Error while reading employee record from the file!");
        close(employeeFileDescriptor);
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);

    close(employeeFileDescriptor);

    // Send modification options
    writeBytes = write(connFD, "Modify Employee Menu:\n1. Name\n2. Age\n3. Gender\nChoose an option: ", 78);
    if (writeBytes == -1) {
        perror("Error while writing modify menu to client!");
        return false;
    }

    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error while getting modification choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    bzero(readBuffer, sizeof(readBuffer));

    switch (choice) {
    case 1:
        writeBytes = write(connFD, "Enter new name: ", 16);
        if (writeBytes == -1) {
            perror("Error while writing new name prompt to client!");
            return false;
        }
        readBytes = read(connFD, employee.name, sizeof(employee.name));
        if (readBytes == -1) {
            perror("Error while getting response for employee's new name from client!");
            return false;
        }
        break;
    case 2:
        writeBytes = write(connFD, "Enter new age: ", 15);
        if (writeBytes == -1) {
            perror("Error while writing new age prompt to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1) {
            perror("Error while getting response for employee's new age from client!");
            return false;
        }
        employee.age = atoi(readBuffer);
        break;
    case 3:
        writeBytes = write(connFD, "Enter new gender (M/F/O): ", 26);
        if (writeBytes == -1) {
            perror("Error while writing new gender prompt to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1) {
            perror("Error while getting response for employee's new gender from client!");
            return false;
        }
        employee.gender = readBuffer[0];
        break;
    default:
        writeBytes = write(connFD, "Invalid choice!\n", 16);
        if (writeBytes == -1) {
            perror("Error while sending invalid choice message to client!");
            return false;
        }
        return false;
    }

    // Open the employee file to write the updated record
    employeeFileDescriptor = open(EMPLOYEE_FILE, O_WRONLY);
    if (employeeFileDescriptor == -1) {
        perror("Error while opening employee file");
        return false;
    }
    
    offset = lseek(employeeFileDescriptor, employeeID * sizeof(struct Employee), SEEK_SET);
    if (offset == -1) {
        perror("Error while seeking to required employee record!");
        close(employeeFileDescriptor);
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(employeeFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1) {
        perror("Error while obtaining write lock on employee record!");
        return false;
    }

    writeBytes = write(employeeFileDescriptor, &employee, sizeof(struct Employee));
    if (writeBytes == -1) {
        perror("Error while writing updated employee info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(employeeFileDescriptor, F_SETLK, &lock);

    close(employeeFileDescriptor);

    writeBytes = write(connFD, "Employee information modified successfully!", 40);
    if (writeBytes == -1) {
        perror("Error while sending success message to client!");
        return false;
    }
    return true;
}




bool admin_operation_handler(int connFD) {
    if (login_handler(true, connFD, NULL)) {
        ssize_t writeBytes, readBytes;            
        char readBuffer[1000], writeBuffer[1000]; 

        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ADMIN_LOGIN_SUCCESS); 
        write(connFD, writeBuffer, strlen(writeBuffer)); // Send login success message
        
        while (1) {
            bzero(writeBuffer, sizeof(writeBuffer));
            strcat(writeBuffer, "\n");
            strcat(writeBuffer, ADMIN_MENU); // Prepare the admin menu
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1) {
                perror("Error while writing ADMIN_MENU to client!");
                return false;
            }

            bzero(readBuffer, sizeof(readBuffer));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer));
            if (readBytes == -1) {
                perror("Error while reading client's choice for ADMIN_MENU");
                return false;
            }

            int choice = atoi(readBuffer);
            switch (choice) {
                case 1:
                    add_employee(connFD);
                    break;
                case 2:
                    modify_customer_info(connFD);
                    break;
                case 3:
                    modify_employee_info(connFD);
                    break;
                case 4:
                    modify_employee_role(connFD);
                    break;
                case 5:
                    change_admin_password(connFD);
                    break;
                case 6: // Logout
                    writeBytes = write(connFD, "You have successfully logged out.\n", strlen("You have successfully logged out.\n"));
                    return true; // Return to the main menu, keeping the connection open
                case 7: // Exit
                    const char *exitMessage = "Exiting the application. Goodbye!🌟 type ok \n";
                    ssize_t writeBytes = write(connFD, exitMessage, strlen(exitMessage));
                    if (writeBytes == -1) {
                        perror("Error sending exit message to client");
                    }

                    // Dummy read for acknowledgment from the client
                    char readBuffer[100];
                    bzero(readBuffer, sizeof(readBuffer)); // Clear the buffer

                    // Reading the acknowledgment from the client
                    ssize_t readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
                    if (readBytes == -1) {
                        perror("Error reading acknowledgment from client");
                    } else if (readBytes > 0) {
                        readBuffer[readBytes] = '\0'; // Null-terminate the received string
                        printf("Received acknowledgment from client: %s\n", readBuffer);
                    } else {
                        printf("No acknowledgment received from client.\n");
                    }

                    // Close the client connection
                    close(connFD);
                    return false; 
                default:
                    writeBytes = write(connFD, "Invalid choice. Please try again.\n", 36);
                    if (writeBytes == -1) {
                        perror("Error sending invalid choice message to client");
                    }
                    break; // Just notify the user and stay in the menu
            }
        }
    } else {
        // ADMIN LOGIN FAILED
        const char *loginFailMessage = "Login failed. Please check your credentials and try again.\n";
        ssize_t writeBytes = write(connFD, loginFailMessage, strlen(loginFailMessage));
        if (writeBytes == -1) {
            perror("Error sending login failure message to client");
        }
        return false;
    }
    return true;
}





void change_admin_password(int connFD) {
    char newPassword[100];
    FILE *file;

    // Prompt for new password
    const char *promptMessage = "Enter new password: ";
    write(connFD, promptMessage, strlen(promptMessage));

    // Read new password from the client
    bzero(newPassword, sizeof(newPassword));
    ssize_t readBytes = read(connFD, newPassword, sizeof(newPassword) - 1);
    if (readBytes == -1) {
        perror("Error reading new password from client");
        return;
    }
    newPassword[readBytes] = '\0'; // Ensure null-termination

    // Open the credentials file for writing
    file = fopen(CREDENTIALS_FILE, "w");
    if (!file) {
        perror("Error opening credentials file");
        const char *errorMessage = "Error updating password. Please try again.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return;
    }

    // Write new credentials to the file
    fprintf(file, "#ifndef ADMIN_CREDENTIALS\n#define ADMIN_CREDENTIALS\n\n");
    fprintf(file, "#define ADMIN_LOGIN_ID \"admin\"\n#define ADMIN_PASSWORD \"%s\"\n\n", newPassword);
    fprintf(file, "#endif\n");

    // Close the file
    fclose(file);

    // Send success message back to the client
    const char *successMessage = "Password updated successfully!\n";
    write(connFD, successMessage, strlen(successMessage));
}



bool modify_customer_info(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];

    struct Customer customer;

    int customerID;

    off_t offset;
    int lockingStatus;

    writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_ID, strlen(ADMIN_MOD_CUSTOMER_ID));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_CUSTOMER_ID message to client!");
        return false;
    }
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while reading customer ID from client!");
        return false;
    }

    customerID = atoi(readBuffer);

    int customerFileDescriptor = open(CUSTOMER_FILE, O_RDONLY);
    if (customerFileDescriptor == -1)
    {
        // Customer File doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, CUSTOMER_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    
    offset = lseek(customerFileDescriptor, customerID * sizeof(struct Customer), SEEK_SET);
    if (errno == EINVAL)
    {
        // Customer record doesn't exist
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, CUSTOMER_ID_DOESNT_EXIT);
        strcat(writeBuffer, "^");
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing CUSTOMER_ID_DOESNT_EXIT message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }
    else if (offset == -1)
    {
        perror("Error while seeking to required customer record!");
        return false;
    }

    struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};

    // Lock the record to be read
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Couldn't obtain lock on customer record!");
        return false;
    }

    readBytes = read(customerFileDescriptor, &customer, sizeof(struct Customer));
    if (readBytes == -1)
    {
        perror("Error while reading customer record from the file!");
        return false;
    }

    // Unlock the record
    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLK, &lock);

    close(customerFileDescriptor);

    writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_MENU, strlen(ADMIN_MOD_CUSTOMER_MENU));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_CUSTOMER_MENU message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error while getting customer modification menu choice from client!");
        return false;
    }

    int choice = atoi(readBuffer);
    if (choice == 0)
    { // A non-numeric string was passed to atoi
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    switch (choice)
    {
    case 1:
        writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_NAME, strlen(ADMIN_MOD_CUSTOMER_NEW_NAME));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_NAME message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new name from client!");
            return false;
        }
        strcpy(customer.name, readBuffer);
        break;
    case 2:
        writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_AGE, strlen(ADMIN_MOD_CUSTOMER_NEW_AGE));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_AGE message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new age from client!");
            return false;
        }
        int updatedAge = atoi(readBuffer);
        if (updatedAge == 0)
        {
            // Either client has sent age as 0 (which is invalid) or has entered a non-numeric string
            bzero(writeBuffer, sizeof(writeBuffer));
            strcpy(writeBuffer, ERRON_INPUT_FOR_NUMBER);
            writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
            if (writeBytes == -1)
            {
                perror("Error while writing ERRON_INPUT_FOR_NUMBER message to client!");
                return false;
            }
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
            return false;
        }
        customer.age = updatedAge;
        break;
    case 3:
        writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_NEW_GENDER, strlen(ADMIN_MOD_CUSTOMER_NEW_GENDER));
        if (writeBytes == -1)
        {
            perror("Error while writing ADMIN_MOD_CUSTOMER_NEW_GENDER message to client!");
            return false;
        }
        readBytes = read(connFD, &readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error while getting response for customer's new gender from client!");
            return false;
        }
        customer.gender = readBuffer[0];
        break;
    default:
        bzero(writeBuffer, sizeof(writeBuffer));
        strcpy(writeBuffer, INVALID_MENU_CHOICE);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1)
        {
            perror("Error while writing INVALID_MENU_CHOICE message to client!");
            return false;
        }
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    customerFileDescriptor = open(CUSTOMER_FILE, O_WRONLY);
    if (customerFileDescriptor == -1)
    {
        perror("Error while opening customer file");
        return false;
    }
    offset = lseek(customerFileDescriptor, customerID * sizeof(struct Customer), SEEK_SET);
    if (offset == -1)
    {
        perror("Error while seeking to required customer record!");
        return false;
    }

    lock.l_type = F_WRLCK;
    lock.l_start = offset;
    lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
    if (lockingStatus == -1)
    {
        perror("Error while obtaining write lock on customer record!");
        return false;
    }

    writeBytes = write(customerFileDescriptor, &customer, sizeof(struct Customer));
    if (writeBytes == -1)
    {
        perror("Error while writing update customer info into file");
    }

    lock.l_type = F_UNLCK;
    fcntl(customerFileDescriptor, F_SETLKW, &lock);

    close(customerFileDescriptor);

    writeBytes = write(connFD, ADMIN_MOD_CUSTOMER_SUCCESS, strlen(ADMIN_MOD_CUSTOMER_SUCCESS));
    if (writeBytes == -1)
    {
        perror("Error while writing ADMIN_MOD_CUSTOMER_SUCCESS message to client!");
        return false;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    return true;
}



int add_employee(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[500], writeBuffer[500];
    struct Employee newEmployee;
    int  employeeFileDescriptor = open(EMPLOYEE_FILE, O_RDWR | O_CREAT | O_APPEND, 0777);

    if (employeeFileDescriptor == -1) {
        perror("Error opening employee file");
        return -1;
    }

    // Check file size to see if it's empty
    off_t fileSize = lseek(employeeFileDescriptor, 0, SEEK_END);
    if (fileSize == -1) {
        perror("Error checking file size");
        close(employeeFileDescriptor);
        return -1;
    } else if (fileSize == 0) {
        // File is empty; assign the first ID
        newEmployee.id = 0;
    } else {
        // File is not empty; seek to the last employee record
        if (lseek(employeeFileDescriptor, -sizeof(struct Employee), SEEK_END) == -1) {
            perror("Error seeking to last employee record");
            close(employeeFileDescriptor);
            return -1;
        }

        struct flock lock = {F_RDLCK, SEEK_END, -sizeof(struct Employee), sizeof(struct Employee), getpid()};
        if (fcntl(employeeFileDescriptor, F_SETLKW, &lock) == -1) {
            perror("Error obtaining read lock on last employee record");
            close(employeeFileDescriptor);
            return -1;
        }

        readBytes = read(employeeFileDescriptor, &newEmployee, sizeof(struct Employee));
        if (readBytes == -1) {
            perror("Error reading last employee record");
            close(employeeFileDescriptor);
            return -1;
        }

        lock.l_type = F_UNLCK;
        fcntl(employeeFileDescriptor, F_SETLK, &lock);
        
        // Increment ID for the new employee
        newEmployee.id++;
    }

    // Send prompt for employee name
    writeBytes = write(connFD, "Enter the employee name: ", 25);
    if (writeBytes == -1) {
        perror("Error writing employee name prompt to client!");
        return -1;
    }

    // Read employee name from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading employee name from client!");
        return -1;
    }
    strcpy(newEmployee.name, readBuffer);

    // Send prompt for employee gender
    writeBytes = write(connFD, "Enter the employee gender (M/F/O): ", 36);
    if (writeBytes == -1) {
        perror("Error writing employee gender prompt to client!");
        return -1;
    }

    // Read employee gender from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading employee gender from client!");
        return -1;
    }
    newEmployee.gender = readBuffer[0];

    // Send prompt for employee age
    writeBytes = write(connFD, "Enter the employee age: ", 25);
    if (writeBytes == -1) {
        perror("Error writing employee age prompt to client!");
        return -1;
    }

    // Read employee age from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading employee age from client!");
        return -1;
    }
    newEmployee.age = atoi(readBuffer);

    // Send prompt for employee password
    writeBytes = write(connFD, "Enter the password: ", 20);
    if (writeBytes == -1) {
        perror("Error writing password prompt to client!");
        return -1;
    }

    // Read password from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading password from client!");
        return -1;
    }
    strcpy(newEmployee.password, readBuffer);

    // Set employee type
    writeBytes = write(connFD, "Enter employee type (1 for Manager, 2 for Bank Employee): ", 58);
    if (writeBytes == -1) {
        perror("Error writing employee type prompt to client!");
        return -1;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading employee type from client!");
        return -1;
    }
    newEmployee.employeeType = atoi(readBuffer);

    // Write employee record to file
    employeeFileDescriptor = open(EMPLOYEE_FILE, O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
    if (employeeFileDescriptor == -1) {
        perror("Error while creating / opening employee file!");
        return -1;
    }

    writeBytes = write(employeeFileDescriptor, &newEmployee, sizeof(newEmployee));
    if (writeBytes == -1) {
        perror("Error while writing employee record to file!");
        close(employeeFileDescriptor);
        return -1;
    }

    close(employeeFileDescriptor); // Close the file after writing

    // Send success message
    writeBytes = write(connFD, "Employee added successfully!\n^", 30);
    if (writeBytes == -1) {
        perror("Error sending success message to client!");
        return -1;
    }
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));

    // Print employee details to the client
    int writtenBytes = snprintf(writeBuffer, sizeof(writeBuffer),
                "\nEmployee Details:\n"
                "ID: %d\n"
                "Name: %s\n"
                "Gender: %c\n"
                "Age: %d\n"
                "Login: %s-%d\n"
                "Password: %s\n"
                "Employee Type: %d (1: Manager, 2: Bank Employee)\n",
                newEmployee.id, newEmployee.name, newEmployee.gender, newEmployee.age,
                newEmployee.name, newEmployee.id, newEmployee.password, newEmployee.employeeType);

    // Check if snprintf truncated the output
    if (writtenBytes >= sizeof(writeBuffer)) {
        fprintf(stderr, "Error: Buffer overflow, data was truncated!\n");
        return -1;
    }

    // // Append the termination character '^'
    strcat(writeBuffer, "^");

    writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
    if (writeBytes == -1) {
        perror("Error sending employee details to client!");
        return -1;
    }

    // Dummy read from the client after sending the termination character
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading dummy confirmation from client!");
        return -1;
    }

    return newEmployee.id;
}



bool modify_employee_role(int connFD) {
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000];
    int employeeId, newRole;
    
    // Get the employee ID from the admin
    writeBytes = write(connFD, "Enter the Employee ID to change role: ", 39);
    if (writeBytes == -1) {
        perror("Error writing GET_EMPLOYEE_ID message to client!");
        return false;
    }
    
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading Employee ID from client!");
        return false;
    }

    employeeId = atoi(readBuffer);

    struct Employee employee;
    if (!find_employee_by_id(employeeId, &employee)) {
        write(connFD, "Employee not found.\n", 20);
        return false;
    }

    // Show the current role
    snprintf(writeBuffer, sizeof(writeBuffer), "Current Role: %d ( Manager, 2: Bank Employee)\n", employee.employeeType);
    write(connFD, writeBuffer, strlen(writeBuffer));
    
    // Get the new role from the admin
    writeBytes = write(connFD, "Enter the new role (1 for Manager, 2 for Bank Employee): ", 58);
    if (writeBytes == -1) {
        perror("Error writing GET_NEW_ROLE message to client!");
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading new role from client!");
        return false;
    }

    newRole = atoi(readBuffer);
    if (newRole < 0 || newRole > 2) {
        write(connFD, "Invalid role. Please enter 0, 1, or 2.\n", 39);
        return false;
    }

    if (update_employee_role(employeeId, newRole)) {
        write(connFD, "Employee role updated successfully.\n", 36);
        return true;
    } else {
        write(connFD, "Error updating employee role.\n", 30);
        return false;
    }
}



bool find_employee_by_id(int employeeId, struct Employee *employee) {
    FILE *file = fopen(EMPLOYEE_FILE, "r");
    if (file == NULL) {
        perror("Error opening employee file.");
        return false;
    }

    while (fread(employee, sizeof(struct Employee), 1, file)) {
        if (employee->id == employeeId) {
            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}



bool update_employee_role(int employeeId, int newRole) {
    FILE *file = fopen(EMPLOYEE_FILE, "r+");
    if (file == NULL) {
        perror("Error opening employee file.");
        return false;
    }

    struct Employee employee;
    while (fread(&employee, sizeof(struct Employee), 1, file)) {
        if (employee.id == employeeId) {
            // Found the employee, update the role
            employee.employeeType = newRole;

            // Move the file pointer back to the beginning of this record
            fseek(file, -sizeof(struct Employee), SEEK_CUR);

            // Write the updated employee record back to the file
            fwrite(&employee, sizeof(struct Employee), 1, file);

            fclose(file);
            return true;
        }
    }

    fclose(file);
    return false;
}




#endif