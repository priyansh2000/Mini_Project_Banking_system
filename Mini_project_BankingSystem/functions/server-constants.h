#ifndef SERVER_CONSTANTS
#define SERVER_CONSTANTS

// ========== COMMON TEXT =============================

// Welcome Message
#define INITIAL_PROMPT  \
    "\n\n============================================================\n" \
                    "Welcome to Gramin Vikas Bank PUNE !\n" \
    "============================================================\n" \
                       "Who are you?\n" \
                       "1. Admin\n" \
                       "2. Customer\n" \
                       "3. Manager\n" \
                       "4. Employee\n" \
                       "5. Exit\n" \
                       "Enter the number corresponding to your choice: "
    

// LOGIN
#define LOGIN_ID "Enter your login ID: "
#define PASSWORD "Enter your password \n# "
#define INVALID_LOGIN "The login ID specified doesn't exist!$"
#define INVALID_PASSWORD "The password specified doesn't match!$"
#define SALT_BAE "666"

// GET ACCOUNT DETAILS
#define GET_ACCOUNT_NUMBER "Enter the account number of the account you're searching for: "
#define GET_CUSTOMER_ID "Enter the customer ID of the customer you're searching for: "

#define ERRON_INPUT_FOR_NUMBER "It seems you have entered an invalid number!\nYou'll now be redirected to the main menu!^"

#define INVALID_MENU_CHOICE "It seems you've made an invalid menu choice\nYou'll now be redirected to the main menu!^"

#define CUSTOMER_ID_DOESNT_EXIT "No customer could be found for the given ID."
#define CUSTOMER_LOGIN_ID_DOESNT_EXIT "No customer could be found for the given login ID$"

#define ACCOUNT_ID_DOESNT_EXIT "No account could be found for the given account number."

#define TRANSACTIONS_NOT_FOUND "No transactions were performed on this account by the customer!^"

// ====================================================

// ========== ADMIN SPECIFIC TEXT======================

// LOGIN WELCOME
#define ADMIN_LOGIN_WELCOME "Welcome dear admin!\nEnter your credentials "
#define ADMIN_LOGIN_SUCCESS "Welcome Admin!"

// ADMIN MENU
#define ADMIN_MENU \
    "\n\n========================\n" \
    "       ADMIN MENU       \n" \
    "========================\n" \
    "1. Add New Bank Employee\n" \
    "2. Modify Customer Information\n" \
    "3. Modify Employee Information\n" \
    "4. Manage User Roles\n" \
    "5. Change Admin Password\n" \
    "6. Logout\n" \
    "7. Exit\n" \
    "========================\n"

// ADD CUSTOMER
#define ADMIN_ADD_CUSTOMER_PRIMARY "Enter the details for the primary customer\n"
#define ADMIN_ADD_CUSTOMER_SECONDARY "Enter the details for the secondary customer\n"
#define ADMIN_ADD_CUSTOMER_NAME "What is the customer's name? "
#define ADMIN_ADD_CUSTOMER_GENDER "What is the customer's gender?\nEnter M for Male, F for Female, and O for Others: "
#define ADMIN_ADD_CUSTOMER_AGE "What is the customer's age? "
#define ADMIN_ADD_CUSTOMER_AUTOGEN_LOGIN "The autogenerated login ID for the customer is: "
#define ADMIN_ADD_CUSTOMER_AUTOGEN_PASSWORD "The autogenerated password for the customer is: "
#define ADMIN_ADD_CUSTOMER_WRONG_GENDER "It seems you've entered a wrong gender choice!\nYou'll now be redirected to the main menu!^"

// ADD ACCOUNT
#define ADMIN_ADD_ACCOUNT_NUMBER "The newly created account's number is: "

// DELETE ACCOUNT
#define ADMIN_DEL_ACCOUNT_NO "What is the account number of the account you want to delete? "
#define ADMIN_DEL_ACCOUNT_SUCCESS "This account has been successfully deleted.\nRedirecting you to the main menu...^"
#define ADMIN_DEL_ACCOUNT_FAILURE "This account cannot be deleted since it still has some money.\nRedirecting you to the main menu...^"

// MODIFY CUSTOMER INFO
#define ADMIN_MOD_CUSTOMER_ID "Enter the ID of the customer whose information you want to edit: "
#define ADMIN_MOD_CUSTOMER_MENU "Which information would you like to modify?\n1. Name 2. Age 3. Gender \nPress any other key to cancel: "
#define ADMIN_MOD_CUSTOMER_NEW_NAME "What's the updated value for name? "
#define ADMIN_MOD_CUSTOMER_NEW_GENDER "What's the updated value for gender? "
#define ADMIN_MOD_CUSTOMER_NEW_AGE "What's the updated value for age? "

#define ADMIN_MOD_CUSTOMER_SUCCESS "The required modification was successfully made!\nYou'll now be redirected to the main menu!^"

#define ADMIN_LOGOUT "Logging you out now, Admin! Goodbye!$"

// ====================================================

// ========== CUSTOMER SPECIFIC TEXT===================

// LOGIN WELCOME
#define CUSTOMER_LOGIN_WELCOME "Welcome dear customer! Enter your credentials to gain access to your account!"
#define CUSTOMER_LOGIN_SUCCESS "Welcome beloved customer!"
#define CUSTOMER_LOGIN_ID_DOESNT_EXIST "The customer login ID doesn't exist.\n"
#define CUSTOMER_LOGOUT "Logging you out now, dear customer! Goodbye!$"

// CUSTOMER
#define CUSTOMER_MENU \
"\n\n========================\n" \
"       CUSTOMER MENU      \n" \
"========================\n" \
"1. Get Customer Details\n" \
"2. Deposit Money\n" \
"3. Withdraw Money\n" \
"4. Get Balance\n" \
"5. Get Transaction Information\n" \
"6. Change Password\n" \
"7. Transfer Funds\n" \
"8. Apply for Loan\n" \
"9. Write Feedback\n" \
"10. Logout\n" \
"11. Exit\n" \
"========================\n" \
"Please enter your choice: "

#define ACCOUNT_DEACTIVATED "It seems your account has been deactivated!^"

#define DEPOSIT_AMOUNT "How much do you want to add to your bank? "
#define DEPOSIT_AMOUNT_INVALID "You seem to have entered an invalid amount!^"
#define DEPOSIT_AMOUNT_SUCCESS "The specified amount has been successfully added to your bank account! type ok "

#define WITHDRAW_AMOUNT "How much do you want to withdraw from your bank? "
#define WITHDRAW_AMOUNT_INVALID "You seem to have either entered an invalid amount or you don't have enough money in your bank to withdraw the specified amount.^"
#define WITHDRAW_AMOUNT_SUCCESS "The specified amount has been successfully withdrawn from your bank account! type ok " 

#define PASSWORD_CHANGE_OLD_PASS "Enter your old password: "
#define PASSWORD_CHANGE_OLD_PASS_INVALID "The entered password doesn't seem to match the old password."
#define PASSWORD_CHANGE_NEW_PASS "Enter the new password: "
#define PASSWORD_CHANGE_NEW_PASS_RE "Reenter the new password: "
#define PASSWORD_CHANGE_NEW_PASS_INVALID "The new password and the reentered passwords don't seem to match!^"
#define PASSWORD_CHANGE_SUCCESS "Password successfully changed!^"

// ====================================================

// ========== MANAGER SPECIFIC TEXT=====================

#define MANAGER_LOGIN_SUCCESS "Login successful! Welcome, Manager!\n"
#define MANAGER_MENU \
"\n\n========================\n" \
"       MANAGER MENU      \n" \
"========================\n" \
"1. Activate Customer Accounts\n" \
"2. Deactivate Customer Accounts\n" \
"3. Review Loan Applications\n" \
"4. Assign Loan to Employees\n" \
"5. View Customer Feedback\n" \
"6. Update Feedback Status\n" \
"7. Change Manager Password\n" \
"8. Logout\n" \
"9. Exit\n" \
"========================\n" \
"Please enter your choice: "

// ====================================================

// ========== EMPLOYEE SPECIFIC TEXT====================

#define EMPLOYEE_LOGIN_SUCCESS "Login successful! Welcome, Employee!\n"
#define EMPLOYEE_MENU  \
"\n\n========================\n" \
"       EMPLOYEE MENU      \n" \
"========================\n" \
"1. Add New Customer\n" \
"2. Modify Customer Details\n" \
"3. Send Loans Assigned to Me\n" \
"4. Approve or Reject Loan\n" \
"5. Check Loan Status\n" \
"6. Change Password\n" \
"7. Logout\n" \
"8. Exit\n" \
"========================\n" \
"Please enter your choice: "

// =====================================================

// ========== CUSTOMER FEEDBACK =========================

#define CUSTOMER_FEEDBACK "Please enter your feedback: "

// ==================================================
#define ACCOUNT_FILE "./records/account.bank"
#define CUSTOMER_FILE "./records/customer.bank"
#define EMPLOYEE_FILE "./records/employee.bank"
#define TRANSACTION_FILE "./records/transactions.bank"
#define LOAN_RECORD_FILE "./records/loan.bank"
#define CREDENTIALS_FILE "./functions/admin-credentials.h"

#endif















// #ifndef SERVER_CONSTANTS
// #define SERVER_CONSTANTS


// // ========== COMMON TEXT =============================

// // Welcome Message
// #define INITIAL_PROMPT "\n\nWelcome to Pune Jatan Societal Bank!\nWho are you?\n1. Admin\t2. Customer\t3. Manager\t4. Employee\nPress any other number to exit\nEnter the number corresponding to your choice: "

// // LOGIN
// #define LOGIN_ID "Enter your login ID: "
// #define PASSWORD "Enter your password \n# "
// #define INVALID_LOGIN "The login ID specified doesn't exist!$"
// #define INVALID_PASSWORD "The password specified doesn't match!$"
// #define SALT_BAE "666"

// // GET ACCOUNT DETAILS
// #define GET_ACCOUNT_NUMBER "Enter the account number of the account you're searching for: "
// #define GET_CUSTOMER_ID "Enter the customer ID of the customer you're searching for: "

// #define ERRON_INPUT_FOR_NUMBER "It seems you have passed a sequence of alphabets when a number was expected or you have entered an invalid number!\nYou'll now be redirected to the main menu!^"

// #define INVALID_MENU_CHOICE "It seems you've made an invalid menu choice\nYou'll now be redirected to the main menu!^"

// #define CUSTOMER_ID_DOESNT_EXIT "No customer could be found for the given ID."
// #define CUSTOMER_LOGIN_ID_DOESNT_EXIT "No customer could be found for the given login ID$"

// #define ACCOUNT_ID_DOESNT_EXIT "No account could be found for the given account number."

// #define TRANSACTIONS_NOT_FOUND "No transactions were performed on this account by the customer!^"

// // ====================================================

// // ========== ADMIN SPECIFIC TEXT======================

// // LOGIN WELCOME
// #define ADMIN_LOGIN_WELCOME "Welcome dear admin! With great power comes great responsibility!\nEnter your credentials to unlock this power!"
// #define ADMIN_LOGIN_SUCCESS "Welcome Admin!"

// // ADMIN MENU
// #define ADMIN_MENU \
//     "Admin Menu:\n" \
//     "1. Add New Bank Employee\n" \
//     "2. Modify Customer Information\n" \
//     "3. Modify Employee Information\n" \
//     "4. Manage User Roles\n" \
//     "5. Change Admin Password\n" \
//     "6. Logout\n" \
//     "7. Exit\n"

// // ADD CUSTOMER
// #define ADMIN_ADD_CUSTOMER_PRIMARY "Enter the details for the primary customer\n"
// #define ADMIN_ADD_CUSTOMER_SECONDARY "Enter the details for the secondary customer\n"
// #define ADMIN_ADD_CUSTOMER_NAME "What is the customer's name? "
// #define ADMIN_ADD_CUSTOMER_GENDER "What is the customer's gender?\nEnter M for Male, F for Female, and O for Others: "
// #define ADMIN_ADD_CUSTOMER_AGE "What is the customer's age? "
// #define ADMIN_ADD_CUSTOMER_AUTOGEN_LOGIN "The autogenerated login ID for the customer is: "
// #define ADMIN_ADD_CUSTOMER_AUTOGEN_PASSWORD "The autogenerated password for the customer is: "
// #define ADMIN_ADD_CUSTOMER_WRONG_GENDER "It seems you've entered a wrong gender choice!\nYou'll now be redirected to the main menu!^"

// // ADD ACCOUNT
// #define ADMIN_ADD_ACCOUNT_NUMBER "The newly created account's number is: "

// // DELETE ACCOUNT
// #define ADMIN_DEL_ACCOUNT_NO "What is the account number of the account you want to delete? "
// #define ADMIN_DEL_ACCOUNT_SUCCESS "This account has been successfully deleted.\nRedirecting you to the main menu...^"
// #define ADMIN_DEL_ACCOUNT_FAILURE "This account cannot be deleted since it still has some money.\nRedirecting you to the main menu...^"

// // MODIFY CUSTOMER INFO
// #define ADMIN_MOD_CUSTOMER_ID "Enter the ID of the customer whose information you want to edit: "
// #define ADMIN_MOD_CUSTOMER_MENU "Which information would you like to modify?\n1. Name 2. Age 3. Gender\nPress any other key to cancel: "
// #define ADMIN_MOD_CUSTOMER_NEW_NAME "What's the updated value for name? "
// #define ADMIN_MOD_CUSTOMER_NEW_GENDER "What's the updated value for gender? "
// #define ADMIN_MOD_CUSTOMER_NEW_AGE "What's the updated value for age? "

// #define ADMIN_MOD_CUSTOMER_SUCCESS "The required modification was successfully made!\nYou'll now be redirected to the main menu!^"

// #define ADMIN_LOGOUT "Logging you out now, Admin! Goodbye!$"

// // ====================================================

// // ========== CUSTOMER SPECIFIC TEXT===================

// // LOGIN WELCOME
// #define CUSTOMER_LOGIN_WELCOME "Welcome dear customer! Enter your credentials to gain access to your account!"
// #define CUSTOMER_LOGIN_SUCCESS "Welcome beloved customer!"
// #define CUSTOMER_LOGIN_ID_DOESNT_EXIST "The customer login ID doesn't exist. type ok \n"
// #define CUSTOMER_LOGOUT "Logging you out now, dear customer! Goodbye!$"

// // CUSTOMER
// #define CUSTOMER_MENU \
// "========================\n" \
// "       CUSTOMER MENU      \n" \
// "========================\n" \
// "1. Get Customer Details\n" \
// "2. Deposit Money\n" \
// "3. Withdraw Money\n" \
// "4. Get Balance\n" \
// "5. Get Transaction Information\n" \
// "6. Change Password\n" \
// "7. Transfer Funds\n" \
// "8. Apply for Loan\n" \
// "9. Write Feedback\n" \
// "10. Logout\n" \
// "11. Exit\n" \
// "========================\n" \
// "Please enter your choice: "

// #define ACCOUNT_DEACTIVATED "It seems your account has been deactivated!^"

// #define DEPOSIT_AMOUNT "How much do you want to add to your bank? "
// #define DEPOSIT_AMOUNT_INVALID "You seem to have entered an invalid amount!^"
// #define DEPOSIT_AMOUNT_SUCCESS "The specified amount has been successfully added to your bank account!^"

// #define WITHDRAW_AMOUNT "How much do you want to withdraw from your bank? "
// #define WITHDRAW_AMOUNT_INVALID "You seem to have either entered an invalid amount or you don't have enough money in your bank to withdraw the specified amount.^"
// #define WITHDRAW_AMOUNT_SUCCESS "The specified amount has been successfully withdrawn from your bank account!^"

// #define PASSWORD_CHANGE_OLD_PASS "Enter your old password: "
// #define PASSWORD_CHANGE_OLD_PASS_INVALID "The entered password doesn't seem to match the old password."
// #define PASSWORD_CHANGE_NEW_PASS "Enter the new password: "
// #define PASSWORD_CHANGE_NEW_PASS_RE "Reenter the new password: "
// #define PASSWORD_CHANGE_NEW_PASS_INVALID "The new password and the reentered passwords don't seem to match!^"
// #define PASSWORD_CHANGE_SUCCESS "Password successfully changed!^"

// // ====================================================

// // ========== MANAGER SPECIFIC TEXT=====================

// #define MANAGER_LOGIN_SUCCESS "Login successful! Welcome, Manager!\n"
// #define MANAGER_MENU \
// "========================\n" \
// "       MANAGER MENU      \n" \
// "========================\n" \
// "1. Activate Customer Accounts\n" \
// "2. Deactivate Customer Accounts\n" \
// "3. Assign Loan Application Processes to Employees\n" \
// "4. Review Customer Feedback\n" \
// "5. Change Password\n" \
// "6. Logout\n" \
// "7. Exit\n" \
// "========================\n" \
// "Please enter your choice: "

// // ====================================================

// // ========== EMPLOYEE SPECIFIC TEXT====================

// #define EMPLOYEE_LOGIN_SUCCESS "Login successful! Welcome, Employee!\n"
// #define EMPLOYEE_MENU  \
// "========================\n" \
// "       EMPLOYEE MENU      \n" \
// "========================\n" \
// "1. Add New Customer\n" \
// "2. Modify Customer Details\n" \
// "3. Send Loans Assigned to Me\n" \
// "4. Approve or Reject Loan\n" \
// "5. Check Loan Status\n" \
// "6. Change Password\n" \
// "7. Logout\n" \
// "8. Exit\n" \
// "========================\n" \
// "Please enter your choice: "

// // =====================================================

// // ========== CUSTOMER FEEDBACK =========================

// #define CUSTOMER_FEEDBACK "Please enter your feedback: "


// // ==================================================
// #define ACCOUNT_FILE "./records/account.bank"
// #define CUSTOMER_FILE "./records/customer.bank"
// #define EMPLOYEE_FILE "./records/employee.bank"
// #define TRANSACTION_FILE "./records/transactions.bank"
// #define LOAN_RECORD_FILE "./records/loan.bank"
// #define CREDENTIALS_FILE "./functions/admin-credentials.h"

// #endif














