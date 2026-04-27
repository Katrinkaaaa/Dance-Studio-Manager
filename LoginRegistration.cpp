#include "DanceStudioApp.h"
#include <iostream>
#include <vector>
#include <cstddef>

using namespace std;

int DanceStudioApp::login() {
    while (true) {
        cout << "\n=== DANCE STUDIO MANAGEMENT SYSTEM ===\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "0. Exit\n";
        cout << "Choose: ";

        int choice = readInt();

        if (choice == 0) {
            return -1;
        }

        if (choice == 2) {
            registerDancer();
            continue;
        }

        if (choice != 1) {
            cout << "Invalid choice.\n";
            continue;
        }

        string phone = readLine("Phone: ");
        string password = readLine("Password: ");

        bool phoneFound = false;

        for (size_t i = 0; i < users.size(); ++i) {
            if (phone == users[i].phone) {
                phoneFound = true;

                if (users[i].status == 0) {
                    cout << "Your registration is waiting for admin approval.\n";
                    return -3;
                }

                if (users[i].status == 2) {
                    cout << "Your registration was rejected.\n";
                    return -3;
                }

                if (password != users[i].password) {
                    cout << "Invalid phone or password.\n";
                    return -3;
                }

                cout << "\nWelcome, " << users[i].name
                    << " (" << roleToString(users[i].role) << ").\n";

                return static_cast<int>(i);
            }
        }

        if (!phoneFound) {
            cout << "Invalid phone or password.\n";
        }
    }
}

void DanceStudioApp::registerDancer() {
    cout << "\n=== REGISTRATION ===\n";

    string phone = readLine("Phone: ");
    string password = readLine("Password: ");

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

    string first = readLine("First name: ");
    string last = readLine("Last name: ");

    cout << "Age: ";
    int age = readInt();

    string exp = readLine("Experience: ");

    string parentPhone = "";
    if (age < 18) {
        parentPhone = readLine("Parent phone: ");
    }

    vector<int> selectedGroups;

    cout << "\nAvailable groups:\n";
    showGroups();

    cout << "How many groups do you want to request? ";
    int count = readInt();

    for (int i = 0; i < count; ++i) {
        cout << "Enter group ID: ";
        int groupId = readInt();

        if (findGroupIndexById(groupId) == -1) {
            cout << "Group not found.\n";
            --i;
            continue;
        }

        selectedGroups.push_back(groupId);
    }

    User u;
    u.id = nextUserId();
    copyText(u.name, MAX_NAME, first + " " + last);
    copyText(u.phone, MAX_PHONE, phone);
    copyText(u.password, MAX_PASSWORD, password);
    u.role = ROLE_DANCER;
    u.status = 0;
    copyText(u.instagram, MAX_LINK, "");
    u.groupCount = 0;

    for (int i = 0; i < MAX_GROUPS_PER_USER; ++i) {
        u.groupIds[i] = 0;
    }

    users.push_back(u);

    JoinRequest r{};
    r.id = nextJoinRequestId();
    copyText(r.firstName, MAX_NAME, first);
    copyText(r.lastName, MAX_NAME, last);
    r.age = age;
    copyText(r.experience, MAX_TEXT, exp);
    copyText(r.parentPhone, MAX_PHONE, parentPhone);
    copyText(r.phone, MAX_PHONE, phone);
    copyText(r.password, MAX_PASSWORD, password);

    r.groupCount = static_cast<int>(selectedGroups.size());
    for (int i = 0; i < r.groupCount; ++i) {
        r.groupIds[i] = selectedGroups[i];
    }

    r.processed = false;

    joinRequests.push_back(r);

    cout << "Registration submitted. Wait for admin approval.\n";
}
