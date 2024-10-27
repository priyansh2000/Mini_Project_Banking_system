#ifndef CUSTOMER_FUNCTIONS
#define CUSTOMER_FUNCTIONS

#include <sys/ipc.h>
#include <sys/sem.h>

#include "./server-constants.h"
#include "../recordtypes/loan.h"
#include "../recordtypes/feedback.h"
#include "../recordtypes/transaction.h"

struct Customer loggedInCustomer;

int semIdentifier;

bool lock_critical_section(struct sembuf *semOp)
{
    semOp->sem_flg = SEM_UNDO;
    semOp->sem_op = -1;
    semOp->sem_num = 0;
    int semopStatus = semop(semIdentifier, semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while locking critical section");
        return false;
    }
    return true;
}


bool unlock_critical_section(struct sembuf *semOp)
{
    semOp->sem_op = 1;
    int semopStatus = semop(semIdentifier, semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while operating on semaphore!");
        _exit(1);
    }
    return true;
}

// Function Prototypes =================================

bool customer_operation_handler(int connFD);
bool deposit(int connFD);
bool withdraw(int connFD);
bool get_balance(int connFD);
bool transfer_funds(int connFD);
bool update_account_in_file(struct Account *account);
bool change_password(int connFD);
bool lock_critical_section(struct sembuf *semOp);
bool unlock_critical_section(struct sembuf *sem_op);
void write_transaction_to_array(int *transactionArray, int ID);
bool write_feedback(int connFD);
// int write_transaction_to_file(int accountNumber, long int oldBalance, long int newBalance, int operation);
int write_transaction_to_file(int accountNumber, long beforeBalance, long afterBalance, int operation, int receiverAccountNumber); 
bool apply_loan(int connFD);

// =====================================================

// Function Definition =================================








bool customer_operation_handler(int connFD) {
    ssize_t writeBytes, readBytes;            // Number of bytes read from/written to the client
    char readBuffer[1000], writeBuffer[1000]; // Buffers for reading & writing to the client                // Assume you have a Customer struct defined

    // Loop until a successful login or exit
    while (1) {
        // Prompt for login ID
        writeBytes = write(connFD, "🔑 Enter your login ID: ", strlen("🔑 Enter your login ID: "));
        if (writeBytes == -1) {
            perror("Error while writing prompt for login ID to client");
            return false; // Exit on write error
        }

        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1) {
            perror("Error while reading login ID from client");
            return false; // Exit on read error
        }
        readBuffer[readBytes] = '\0'; // Null-terminate the string

        // Validate the login ID (replace this with your actual validation logic)
        if (login_handler(false, connFD, &loggedInCustomer)) {
            // Proceed to login success
            writeBytes = write(connFD, CUSTOMER_LOGIN_SUCCESS, strlen(CUSTOMER_LOGIN_SUCCESS));
            if (writeBytes == -1) {
                perror("Error while writing login success message to client");
                return false; // Exit on write error
            }
            break; // Exit the loop for successful login
        } else {
            // Inform the user of the failure
            writeBytes = write(connFD, CUSTOMER_LOGIN_ID_DOESNT_EXIST, strlen(CUSTOMER_LOGIN_ID_DOESNT_EXIST));
            if (writeBytes == -1) {
                perror("Error while sending login failure message to client");
            }
        }
    }

    // Now, you can proceed with the customer menu
    while (1) {
        // Display customer menu
        bzero(writeBuffer, sizeof(writeBuffer));
        strcat(writeBuffer, "\n");
        strcat(writeBuffer, CUSTOMER_MENU);
        writeBytes = write(connFD, writeBuffer, strlen(writeBuffer));
        if (writeBytes == -1) {
            perror("Error while writing CUSTOMER_MENU to client!");
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));

        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1) {
            perror("Error while reading client's choice for CUSTOMER_MENU");
            return false;
        }
        readBuffer[readBytes] = '\0'; // Null-terminate the string

        int choice = atoi(readBuffer);
        switch (choice) {
            case 1:
                get_customer_details(connFD, loggedInCustomer.id);
                break;
            case 2:
                deposit(connFD);
                break;
            case 3:
                withdraw(connFD);
                break;
            case 4:
                get_balance(connFD);
                break;
            case 5:
                get_transaction_details(connFD, loggedInCustomer.account);
                break;
            case 6:
                change_password(connFD);
                break;
            case 7:
                transfer_funds(connFD);
                break;
            case 8:
                apply_loan(connFD);
                break;
            case 9:
                write_feedback(connFD);
                break;
            case 10: // Logout
                logout_handler(connFD, loggedInCustomer.id);
                return true; // Indicate that we want to return to the initial prompt
            case 11: // Exit
                logout_handler(connFD, loggedInCustomer.id);

                // Send exit message to client
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
                return false; // Return false to indicate that the session should end
            default:
                write(connFD, "Invalid choice! Please try again.\n", 36);
        }
    }

    return true; // This line might be redundant depending on your logic
}




bool deposit(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;
    long int depositAmount = 0;

    // Lock the critical section
    // struct sembuf semOp;
    // lock_critical_section(&semOp);

    if (!get_account_details(connFD, &account)) {
        // unlock_critical_section(&semOp);
        return false;
    }

    if (!account.active) {
        write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // unlock_critical_section(&semOp);
        return false;
    }

    // Ask for the deposit amount
    writeBytes = write(connFD, DEPOSIT_AMOUNT, strlen(DEPOSIT_AMOUNT));
    if (writeBytes == -1) {
        perror("Error writing DEPOSIT_AMOUNT prompt to client!");
        // unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading deposit amount from client!");
        // unlock_critical_section(&semOp);
        return false;
    }

    depositAmount = atol(readBuffer);
    if (depositAmount <= 0) {
        writeBytes = write(connFD, DEPOSIT_AMOUNT_INVALID, strlen(DEPOSIT_AMOUNT_INVALID));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // unlock_critical_section(&semOp);
        return false;
    }

    // Log the transaction before updating the account balance
    // Here, the receiverAccNumber is the same as the sender's accountNumber
    int newTransactionID = write_transaction_to_file(account.accountNumber, account.balance, account.balance + depositAmount, 0 ,account.accountNumber);
    if (newTransactionID < 0) {
        // unlock_critical_section(&semOp);
        return false; // Handle transaction logging failure
    }

    // Update account balance
    account.balance += depositAmount;

    // Update the account file with the new balance
    int accountFileDescriptor = open(ACCOUNT_FILE, O_RDWR);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating!");
        // unlock_critical_section(&semOp);
        return false;
    }

    off_t offset = lseek(accountFileDescriptor, account.accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking in account file!");
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on account record!");
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    writeBytes = write(accountFileDescriptor, &account, sizeof(struct Account));
    if (writeBytes == -1) {
        perror("Error writing updated account balance to file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    // Unlock the account record
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);
    close(accountFileDescriptor);

    write(connFD, DEPOSIT_AMOUNT_SUCCESS, strlen(DEPOSIT_AMOUNT_SUCCESS));
    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    get_balance(connFD);
    // unlock_critical_section(&semOp);
    return true;
}




bool withdraw(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;

    long int withdrawAmount = 0;

    // Lock the critical section
    // struct sembuf semOp;
    // lock_critical_section(&semOp);

    // Get account details
    if (!get_account_details(connFD, &account)) {
        // unlock_critical_section(&semOp);
        perror("Failed to get account details");
        return false;
    }

    if (!account.active) {
        write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // unlock_critical_section(&semOp);
        return false;
    }

    // Ask for the withdraw amount
    writeBytes = write(connFD, WITHDRAW_AMOUNT, strlen(WITHDRAW_AMOUNT));
    if (writeBytes == -1) {
        perror("Error writing WITHDRAW_AMOUNT message to client!");
        // unlock_critical_section(&semOp);
        return false;
    }

    // Read the withdraw amount from client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading withdraw amount from client!");
        // unlock_critical_section(&semOp);
        return false;
    }

    withdrawAmount = atol(readBuffer);
    printf("Withdraw Amount Requested: %ld\n", withdrawAmount); // Debug log

    // Validate withdraw amount
    if (withdrawAmount > 0 && account.balance >= withdrawAmount) {
        // Log the transaction before updating the account balance
        // Pass account.accountNumber for both sender and receiver since it's a withdrawal
        int newTransactionID = write_transaction_to_file(account.accountNumber, account.balance, account.balance - withdrawAmount, 1, account.accountNumber);
        if (newTransactionID < 0) {
            // unlock_critical_section(&semOp);
            return false; // Handle transaction logging failure
        }

        // Update account balance
        account.balance -= withdrawAmount;

        // Update the account file with the new balance
        int accountFileDescriptor = open(ACCOUNT_FILE, O_RDWR);
        if (accountFileDescriptor == -1) {
            perror("Error opening account file for updating!");
            // unlock_critical_section(&semOp);
            return false;
        }

        // Seek to the position of the account
        off_t offset = lseek(accountFileDescriptor, account.accountNumber * sizeof(struct Account), SEEK_SET);
        if (offset == -1) {
            perror("Error seeking in account file!");
            close(accountFileDescriptor);
            // unlock_critical_section(&semOp);
            return false;
        }

        // Lock the account record for writing
        struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
        if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
            perror("Error obtaining write lock on account record!");
            close(accountFileDescriptor);
            // unlock_critical_section(&semOp);
            return false;
        }

        // Write the updated account balance to the file
        writeBytes = write(accountFileDescriptor, &account, sizeof(struct Account));
        if (writeBytes == -1) {
            perror("Error writing updated account balance to file!");
            lock.l_type = F_UNLCK; // Unlock on error
            fcntl(accountFileDescriptor, F_SETLK, &lock);
            close(accountFileDescriptor);
            // unlock_critical_section(&semOp);
            return false;
        }

        // Unlock the account record
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);

        // Notify client of success
        write(connFD, WITHDRAW_AMOUNT_SUCCESS, strlen(WITHDRAW_AMOUNT_SUCCESS));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

        // Get the updated balance for the client
        get_balance(connFD);
        // unlock_critical_section(&semOp);
        return true;
    } else {
        // Invalid withdrawal amount
        writeBytes = write(connFD, WITHDRAW_AMOUNT_INVALID, strlen(WITHDRAW_AMOUNT_INVALID));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // unlock_critical_section(&semOp);
        return false;
    }
}



bool get_balance(int connFD)
{
    char buffer[1000];
    struct Account account;
    account.accountNumber = loggedInCustomer.account;
    if (get_account_details(connFD, &account))
    {
        bzero(buffer, sizeof(buffer));
        if (account.active)
        {
            sprintf(buffer, "You have ₹ %ld imaginary money in our bank!\ntype ok ", account.balance);
            write(connFD, buffer, strlen(buffer));
        }
        else
            write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, buffer, sizeof(buffer)); // Dummy read
    }
    else
    {
        // ERROR while getting balance
        return false;
    }
}



bool transfer_funds(int connFD) {
    char readBuffer[1000];
    ssize_t readBytes, writeBytes;
    struct Account senderAccount, receiverAccount;
    senderAccount.accountNumber = loggedInCustomer.account;
    long transferAmount = 0;

    // Lock the critical section
    // struct sembuf semOp;
    // lock_critical_section(&semOp);

    // Get sender's account details
    if (!get_account_details(connFD, &senderAccount)) {
        // unlock_critical_section(&semOp);
        return false;
    }

    if (!senderAccount.active) {
        write(connFD, "Account deactivated", 20);
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // unlock_critical_section(&semOp);
        return false;
    }

    // Ask for receiver's account ID
    writeBytes = write(connFD, "Enter the account ID of the recipient: ", 40);
    if (writeBytes == -1) {
        perror("Error writing recipient account prompt!");
        // unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading recipient account ID!");
        // unlock_critical_section(&semOp);
        return false;
    }

    int receiverAccountNumber = atoi(readBuffer);

    // Check if the receiver is the same as the sender
    if (receiverAccountNumber == senderAccount.accountNumber) {
        write(connFD, "Cannot transfer funds to the same account!", 42);
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // unlock_critical_section(&semOp);
        return false;
    }

    // Get recipient's account details
    receiverAccount.accountNumber = receiverAccountNumber;
    if (!get_account_details(receiverAccountNumber, &receiverAccount)) {
        write(connFD, "Recipient account not found!", 29);
        // unlock_critical_section(&semOp);
        return false;
    }

    if (!receiverAccount.active) {
        write(connFD, "Recipient account is deactivated!\ntype ok ", strlen("Recipient account is deactivated!\ntype ok "));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // unlock_critical_section(&semOp);
        return false;
    }

    // Ask for the transfer amount
    writeBytes = write(connFD, "Enter the amount to transfer: ", 30);
    if (writeBytes == -1) {
        perror("Error writing transfer amount prompt!");
        // unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading transfer amount!");
        // unlock_critical_section(&semOp);
        return false;
    }

    transferAmount = atol(readBuffer);

    if (transferAmount <= 0 || senderAccount.balance < transferAmount) {
        writeBytes = write(connFD, "Insufficient funds or invalid amount!", 37);
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        // unlock_critical_section(&semOp);
        return false;
    }

    // Log the transaction for sender (Withdraw operation)
    int newSenderTransactionID = write_transaction_to_file(
        senderAccount.accountNumber, 
        senderAccount.balance, 
        senderAccount.balance - transferAmount, 
        2, 
        receiverAccount.accountNumber  // Receiver's account for sender's transaction
    );


    if (newSenderTransactionID < 0) {
        // unlock_critical_section(&semOp);
        return false; // Handle transaction logging failure
    }

    // Deduct from sender's account
    senderAccount.balance -= transferAmount;

    // Update sender's account in file
    int accountFileDescriptor = open(ACCOUNT_FILE, O_WRONLY);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating sender!");
        // unlock_critical_section(&semOp);
        return false;
    }

    off_t offset = lseek(accountFileDescriptor, senderAccount.accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking in account file for sender!");
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Account), getpid()};
    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on sender account record!");
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    writeBytes = write(accountFileDescriptor, &senderAccount, sizeof(struct Account));
    if (writeBytes == -1) {
        perror("Error writing updated balance into sender account file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    // Unlock the sender account record
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);
    close(accountFileDescriptor);  // Close after updating sender

    // Reopen the file for the receiver's account
    accountFileDescriptor = open(ACCOUNT_FILE, O_WRONLY);
    if (accountFileDescriptor == -1) {
        perror("Error opening account file for updating receiver!");
        // unlock_critical_section(&semOp);
        return false;
    }

    // Log the transaction for receiver (Deposit operation)
    int newReceiverTransactionID = write_transaction_to_file(
        receiverAccount.accountNumber, 
        receiverAccount.balance, 
        receiverAccount.balance + transferAmount, 
        2, 
        senderAccount.accountNumber  // Sender's account for receiver's transaction
    );


    if (newReceiverTransactionID < 0) {
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false; // Handle transaction logging failure
    }

    // Add to receiver's account
    receiverAccount.balance += transferAmount;

    // Update receiver's account in file
    offset = lseek(accountFileDescriptor, receiverAccount.accountNumber * sizeof(struct Account), SEEK_SET);
    if (offset == -1) {
        perror("Error seeking in account file for receiver!");
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    lock.l_type = F_WRLCK;
    if (fcntl(accountFileDescriptor, F_SETLKW, &lock) == -1) {
        perror("Error obtaining write lock on receiver account record!");
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    writeBytes = write(accountFileDescriptor, &receiverAccount, sizeof(struct Account));
    if (writeBytes == -1) {
        perror("Error writing updated balance into receiver account file!");
        lock.l_type = F_UNLCK;
        fcntl(accountFileDescriptor, F_SETLK, &lock);
        close(accountFileDescriptor);
        // unlock_critical_section(&semOp);
        return false;
    }

    // Unlock the receiver account record
    lock.l_type = F_UNLCK;
    fcntl(accountFileDescriptor, F_SETLK, &lock);
    close(accountFileDescriptor);

    // Notify sender and receiver
    writeBytes = write(connFD, "Transfer successful! type ok ", 30);
    if (writeBytes == -1) {
        perror("Error sending transfer success message!");
        // unlock_critical_section(&semOp);
        return false;
    }

    read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

    // unlock_critical_section(&semOp);
    return true;
}



bool change_password(int connFD)
{
    ssize_t readBytes, writeBytes;
    char readBuffer[1000], writeBuffer[1000], hashedPassword[1000];

    char newPassword[1000];

    // Lock the critical section
    struct sembuf semOp = {0, -1, SEM_UNDO};
    int semopStatus = semop(semIdentifier, &semOp, 1);
    if (semopStatus == -1)
    {
        perror("Error while locking critical section");
        return false;
    }

    writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS, strlen(PASSWORD_CHANGE_OLD_PASS));
    if (writeBytes == -1)
    {
        perror("Error writing PASSWORD_CHANGE_OLD_PASS message to client!");
        unlock_critical_section(&semOp);
        return false;
    }

    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1)
    {
        perror("Error reading old password response from client");
        unlock_critical_section(&semOp);
        return false;
    }
    
    if (strcmp(readBuffer, loggedInCustomer.password) == 0)  // Direct comparison of old password
    {
        // Password matches with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS, strlen(PASSWORD_CHANGE_NEW_PASS));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS message to client!");
            unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password response from client");
            unlock_critical_section(&semOp);
            return false;
        }

        strcpy(newPassword, readBuffer);  // Store the new password directly

        writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_RE, strlen(PASSWORD_CHANGE_NEW_PASS_RE));
        if (writeBytes == -1)
        {
            perror("Error writing PASSWORD_CHANGE_NEW_PASS_RE message to client!");
            unlock_critical_section(&semOp);
            return false;
        }
        bzero(readBuffer, sizeof(readBuffer));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer));
        if (readBytes == -1)
        {
            perror("Error reading new password reenter response from client");
            unlock_critical_section(&semOp);
            return false;
        }

        if (strcmp(readBuffer, newPassword) == 0)  // Direct comparison of new and reentered password
        {
            // New & reentered passwords match
            strcpy(loggedInCustomer.password, newPassword);  // Store the new password directly

            int customerFileDescriptor = open(CUSTOMER_FILE, O_WRONLY);
            if (customerFileDescriptor == -1)
            {
                perror("Error opening customer file!");
                unlock_critical_section(&semOp);
                return false;
            }

            off_t offset = lseek(customerFileDescriptor, loggedInCustomer.id * sizeof(struct Customer), SEEK_SET);
            if (offset == -1)
            {
                perror("Error seeking to the customer record!");
                unlock_critical_section(&semOp);
                return false;
            }

            struct flock lock = {F_WRLCK, SEEK_SET, offset, sizeof(struct Customer), getpid()};
            int lockingStatus = fcntl(customerFileDescriptor, F_SETLKW, &lock);
            if (lockingStatus == -1)
            {
                perror("Error obtaining write lock on customer record!");
                unlock_critical_section(&semOp);
                return false;
            }

            writeBytes = write(customerFileDescriptor, &loggedInCustomer, sizeof(struct Customer));
            if (writeBytes == -1)
            {
                perror("Error storing updated customer password into customer record!");
                unlock_critical_section(&semOp);
                return false;
            }

            lock.l_type = F_UNLCK;
            lockingStatus = fcntl(customerFileDescriptor, F_SETLK, &lock);

            close(customerFileDescriptor);

            writeBytes = write(connFD, PASSWORD_CHANGE_SUCCESS, strlen(PASSWORD_CHANGE_SUCCESS));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read

            unlock_critical_section(&semOp);

            return true;
        }
        else
        {
            // New & reentered passwords don't match
            writeBytes = write(connFD, PASSWORD_CHANGE_NEW_PASS_INVALID, strlen(PASSWORD_CHANGE_NEW_PASS_INVALID));
            readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        }
    }
    else
    {
        // Password doesn't match with old password
        writeBytes = write(connFD, PASSWORD_CHANGE_OLD_PASS_INVALID, strlen(PASSWORD_CHANGE_OLD_PASS_INVALID));
        readBytes = read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
    }


    unlock_critical_section(&semOp);

    return false;
}







bool apply_loan(int connFD) {
    char readBuffer[500];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;

    // Retrieve account details
    if (!get_account_details(connFD, &account)) {
        return false;
    }

    // Check if the account is active
    if (!account.active) {
        write(connFD, "Account is deactivated.\n", strlen("Account is deactivated.\n"));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read to handle client acknowledgment
        return false;
    }

    int loanAmount;

    // Ask the client to enter the loan amount
    const char *promptMessage = "Please enter the loan amount you wish to apply for: ";
    writeBytes = write(connFD, promptMessage, strlen(promptMessage));
    if (writeBytes == -1) {
        perror("Error sending prompt to client");
        return false;
    }

    // Read the loan amount from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer) - 1);
    if (readBytes == -1) {
        perror("Error reading loan amount from client");
        return false;
    }
    readBuffer[readBytes] = '\0'; // Null-terminate

    // Convert the loan amount to an integer
    loanAmount = atoi(readBuffer);
    if (loanAmount <= 0) {
        const char *errorMessage = "Invalid loan amount provided. Please enter a valid amount.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return false;
    }

    // Open the loans file for reading and writing, create if it does not exist
    int loanFileDescriptor = open(LOAN_RECORD_FILE, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (loanFileDescriptor == -1) {
        perror("Error opening loans file for reading/writing");
        const char *errorMessage = "Internal error. Cannot access loan records.\n";
        write(connFD, errorMessage, strlen(errorMessage));
        return false;
    }

    struct Loan newLoan, lastLoan;
    off_t fileSize = lseek(loanFileDescriptor, 0, SEEK_END);
    if (fileSize == 0) {
        // File is empty, set the first loan ID to 0
        newLoan.loanID = 0;
    } else {
        // Move to the last loan record
        int offset = lseek(loanFileDescriptor, -sizeof(struct Loan), SEEK_END);
        if (offset == -1) {
            perror("Error seeking to last loan record");
            close(loanFileDescriptor);
            return false;
        }

        // Read the last loan record to determine the new loan ID
        readBytes = read(loanFileDescriptor, &lastLoan, sizeof(struct Loan));
        if (readBytes == -1 || readBytes != sizeof(struct Loan)) {
            perror("Error reading last loan record");
            close(loanFileDescriptor);
            return false;
        }

        // Set the new loan ID
        newLoan.loanID = lastLoan.loanID + 1;
    }

    // Set the new loan details
    newLoan.amount = loanAmount;
    newLoan.custID = loggedInCustomer.account;
    newLoan.status = 0; // Initial status: unassigned
    newLoan.empID = -1; // No employee assigned yet

    // Write lock to ensure exclusive access while adding the new loan
    struct flock writeLock = {F_WRLCK, SEEK_END, 0, 0, getpid()};
    if (fcntl(loanFileDescriptor, F_SETLKW, &writeLock) == -1) {
        perror("Error obtaining write lock on loan file");
        close(loanFileDescriptor);
        return false;
    }

    // Write the new loan record to the file
    if (write(loanFileDescriptor, &newLoan, sizeof(struct Loan)) == -1) {
        perror("Error writing new loan record to file");
        writeLock.l_type = F_UNLCK;
        fcntl(loanFileDescriptor, F_SETLK, &writeLock);
        close(loanFileDescriptor);
        return false;
    }

    // Release the write lock
    writeLock.l_type = F_UNLCK;
    fcntl(loanFileDescriptor, F_SETLK, &writeLock);

    // Close the file descriptor
    close(loanFileDescriptor);

    // // Send confirmation back to the client
    const char *successMessage = "Loan application has been successfully submitted.\n";
    write(connFD, successMessage, strlen(successMessage));

    return true;
}



bool write_feedback(int connFD) {
    char readBuffer[500];
    ssize_t readBytes, writeBytes;

    struct Account account;
    account.accountNumber = loggedInCustomer.account;

    // Retrieve account details
    if (!get_account_details(connFD, &account)) {
        return false;
    }

    // Check if the account is active
    if (!account.active) {
        write(connFD, ACCOUNT_DEACTIVATED, strlen(ACCOUNT_DEACTIVATED));
        read(connFD, readBuffer, sizeof(readBuffer)); // Dummy read
        return false;
    }

    struct Feedback newFeedback, oldFeedback;
    newFeedback.account = loggedInCustomer.account; // Setting account number
    newFeedback.state = 0; // Setting state

    // Open the feedback file with append mode
    int feedbackFileDescriptor = open("records/feedback.bank", O_CREAT | O_RDWR | O_APPEND, S_IRWXU);
    if (feedbackFileDescriptor == -1) {
        perror("Error opening feedback file!");
        return false;
    }

    // Check if the file is empty to determine the newFeedback ID
    off_t fileSize = lseek(feedbackFileDescriptor, 0, SEEK_END);
    if (fileSize == 0) {
        // File is empty, set the first feedback ID to 0
        newFeedback.id = 0;
    } else {
        // Move to the last feedback record
        int offset = lseek(feedbackFileDescriptor, -sizeof(struct Feedback), SEEK_END);
        if (offset == -1) {
            perror("Error seeking to last Feedback record!");
            close(feedbackFileDescriptor);
            return false;
        }

        // Set up a read lock on the last feedback record
        struct flock lock = {F_RDLCK, SEEK_SET, offset, sizeof(struct Feedback), getpid()};
        int lockingStatus = fcntl(feedbackFileDescriptor, F_SETLKW, &lock);
        if (lockingStatus == -1) {
            perror("Error obtaining read lock on Feedback record!");
            close(feedbackFileDescriptor);
            return false;
        }

        // Read the last feedback record to get the ID
        readBytes = read(feedbackFileDescriptor, &oldFeedback, sizeof(struct Feedback));
        if (readBytes == -1) {
            perror("Error while reading Feedback record from file!");
            lock.l_type = F_UNLCK;
            fcntl(feedbackFileDescriptor, F_SETLK, &lock);
            close(feedbackFileDescriptor);
            return false;
        }

        // Release the read lock
        lock.l_type = F_UNLCK;
        fcntl(feedbackFileDescriptor, F_SETLK, &lock);

        // Set the new feedback ID based on the last ID
        newFeedback.id = oldFeedback.id + 1;
    }

    // Ask for feedback from the client
    writeBytes = write(connFD, CUSTOMER_FEEDBACK, strlen(CUSTOMER_FEEDBACK));
    if (writeBytes == -1) {
        perror("Error writing CUSTOMER_FEEDBACK prompt to client!");
        close(feedbackFileDescriptor);
        return false;
    }

    // Read the feedback message from the client
    bzero(readBuffer, sizeof(readBuffer));
    readBytes = read(connFD, readBuffer, sizeof(readBuffer));
    if (readBytes == -1) {
        perror("Error reading feedback from client!");
        close(feedbackFileDescriptor);
        return false;
    }

    // Copy the feedback message, ensuring it is null-terminated
    strncpy(newFeedback.message, readBuffer, sizeof(newFeedback.message) - 1);
    newFeedback.message[sizeof(newFeedback.message) - 1] = '\0';

    // Explicitly seek to the end before writing
    lseek(feedbackFileDescriptor, 0, SEEK_END);

    // Write lock to ensure exclusive access while adding the new feedback
    struct flock writeLock = {F_WRLCK, SEEK_END, 0, 0, getpid()};
    if (fcntl(feedbackFileDescriptor, F_SETLKW, &writeLock) == -1) {
        perror("Error obtaining write lock on Feedback file!");
        close(feedbackFileDescriptor);
        return false;
    }

    // Write the new feedback record to the file
    if (write(feedbackFileDescriptor, &newFeedback, sizeof(struct Feedback)) == -1) {
        perror("Error writing new Feedback record to file!");
        writeLock.l_type = F_UNLCK;
        fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);
        close(feedbackFileDescriptor);
        return false;
    }

    // Release the write lock
    writeLock.l_type = F_UNLCK;
    fcntl(feedbackFileDescriptor, F_SETLK, &writeLock);

    // Close the file descriptor
    close(feedbackFileDescriptor);

    return true;
}



int get_new_transaction_id() {
    static int transactionID = 0;
    // Increment the transaction ID for each new transaction
    return ++transactionID;
}



int write_transaction_to_file(int accountNumber, long beforeBalance, long afterBalance, int operation, int receiverAccountNumber) {
    struct Transaction transaction;
    // Get the unique transaction ID
    transaction.transactionID = get_new_transaction_id();
    transaction.accountNumber = accountNumber;
    transaction.operation = operation; // 0 = Withdraw, 1 = Deposit
    transaction.oldBalance = beforeBalance;
    transaction.newBalance = afterBalance;
    transaction.receiverAccNumber = receiverAccountNumber;
    transaction.transactionTime = time(NULL); // Get current time

    // Write the transaction to the file in binary
    int transactionFileDescriptor = open(TRANSACTION_FILE, O_CREAT | O_APPEND | O_WRONLY, 0777);
    if (transactionFileDescriptor == -1) {
        perror("Error opening transaction file");
        return -1;
    }

    if (write(transactionFileDescriptor, &transaction, sizeof(struct Transaction)) == -1) {
        perror("Error writing transaction to file");
        close(transactionFileDescriptor);
        return -1;
    }

    close(transactionFileDescriptor);
    return transaction.transactionID; // Return the transaction ID
}





#endif