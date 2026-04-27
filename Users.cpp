#include "DanceStudioApp.h"
#include <iostream>
#include <iomanip>

using namespace std; 

void DanceStudioApp::showAllUsers() const {
    if (users.empty()) {
        cout << "No users found.\n";
        return;
    }

    cout << left
        << setw(5) << "ID"
        << setw(22) << "Name"
        << setw(20) << "Phone"
        << setw(12) << "Role"
        << setw(12) << "Status"
        << "Instagram\n";

    cout << string(85, '-') << "\n";

    for (size_t i = 0; i < users.size(); ++i) {
        string statusStr;

        if (users[i].status == 0) statusStr = "Pending";
        else if (users[i].status == 1) statusStr = "Approved";
        else if (users[i].status == 2) statusStr = "Rejected";

        cout << left
            << setw(5) << users[i].id
            << setw(22) << users[i].name
            << setw(20) << users[i].phone
            << setw(12) << roleToString(users[i].role)
            << setw(12) << statusStr
            << users[i].instagram
            << "\n";
    }
}

void DanceStudioApp::addUserInteractive(int role) {
    string name = readLine("Name: ");
    string phone = readLine("Phone: ");
    string password = readLine("Password: ");
    string instagram = readLine("Instagram link (or leave empty): ");

    if (!isValidPhone(phone)) {
        cout << "Phone must contain exactly 8 digits.\n";
        return;
    }

    if (!isValidPassword(password)) {
        cout << "Password must contain at least 8 letters or digits.\n";
        return;
    }

    if (findUserIdByPhone(phone) != -1) {
        cout << "A user with this phone already exists.\n";
        return;
    }

    int id = addUserRecord(name, phone, password, role, instagram);
    cout << "User added with ID " << id << ".\n";
}

void DanceStudioApp::deleteUserInteractive() {
    showAllUsers();

    cout << "Enter user ID to delete (0 - cancel): ";
    int id = readInt();

    if (id == 0) return;

    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].id == id) {

            if (users[i].role == ROLE_ADMIN) {
                cout << "Cannot delete admin.\n";
                return;
            }

            users.erase(users.begin() + i);

            cout << "User deleted.\n";
            return;
        }
    }

    cout << "User not found.\n";
}

void DanceStudioApp::showUsersByRole(int role) const {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].role == role) {
            cout << "ID: " << users[i].id
                << " | Name: " << users[i].name
                << " | Phone: " << users[i].phone << "\n";
        }
    }
}
