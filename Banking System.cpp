#include <iostream>
#include <vector>
using namespace std;

class BankAccount {
private:
    string accountHolder;
    int accountNumber;
    double balance;

public:
    // Constructor
    BankAccount(string name, int accNum, double initialBalance) {
        accountHolder = name;
        accountNumber = accNum;
        balance = initialBalance;
    }

    // Deposit money
    void deposit(double amount) {
        if (amount > 0) {
            balance += amount;
            cout << "₹" << amount << " deposited successfully.\n";
        } else {
            cout << "Invalid deposit amount!\n";
        }
    }

    // Withdraw money
    void withdraw(double amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            cout << "₹" << amount << " withdrawn successfully.\n";
        } else {
            cout << "Insufficient balance or invalid amount!\n";
        }
    }

    // Display account details
    void displayAccount() {
        cout << "\nAccount Holder: " << accountHolder << endl;
        cout << "Account Number: " << accountNumber << endl;
        cout << "Balance: ₹" << balance << endl;
    }

    // Get account number
    int getAccountNumber() {
        return accountNumber;
    }
};

// Main function
int main() {
    vector<BankAccount> accounts;
    int choice;

    while (true) {
        cout << "\n=== Bank Management System ===\n";
        cout << "1. Create Account\n";
        cout << "2. Deposit Money\n";
        cout << "3. Withdraw Money\n";
        cout << "4. Display Account Details\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            string name;
            int accNum;
            double initialBalance;
            cout << "Enter Account Holder Name: ";
            cin.ignore();
            getline(cin, name);
            cout << "Enter Account Number: ";
            cin >> accNum;
            cout << "Enter Initial Balance: ";
            cin >> initialBalance;
            accounts.push_back(BankAccount(name, accNum, initialBalance));
            cout << "Account created successfully!\n";
        } 
        else if (choice == 2) {
            int accNum;
            double amount;
            cout << "Enter Account Number: ";
            cin >> accNum;
            bool found = false;

            for (auto &acc : accounts) {
                if (acc.getAccountNumber() == accNum) {
                    cout << "Enter Deposit Amount: ";
                    cin >> amount;
                    acc.deposit(amount);
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Account not found!\n";
            }
        } 
        else if (choice == 3) {
            int accNum;
            double amount;
            cout << "Enter Account Number: ";
            cin >> accNum;
            bool found = false;

            for (auto &acc : accounts) {
                if (acc.getAccountNumber() == accNum) {
                    cout << "Enter Withdrawal Amount: ";
                    cin >> amount;
                    acc.withdraw(amount);
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Account not found!\n";
            }
        } 
        else if (choice == 4) {
            int accNum;
            cout << "Enter Account Number: ";
            cin >> accNum;
            bool found = false;

            for (auto &acc : accounts) {
                if (acc.getAccountNumber() == accNum) {
                    acc.displayAccount();
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Account not found!\n";
            }
        } 
        else if (choice == 5) {
            cout << "Exiting the system. Thank you!\n";
            break;
        } 
        else {
            cout << "Invalid choice! Please try again.\n";
        }
    }

    return 0;
}