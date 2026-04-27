#include "DanceStudioApp.h"
#include <iostream>
#include <iomanip>

using namespace std;

void DanceStudioApp::showGroups() const {
    if (groups.empty()) {
        cout << "No groups found.\n";
        return;
    }

    cout << left
        << setw(5) << "ID"
        << setw(28) << "Name"
        << setw(8) << "Coach"
        << setw(10) << "Capacity"
        << "\n";
    cout << string(55, '-') << "\n";

    for (size_t i = 0; i < groups.size(); ++i) {
        cout << left
            << setw(5) << groups[i].id
            << setw(28) << groups[i].name
            << setw(8) << groups[i].coachUserId
            << setw(10) << groups[i].capacity
            << "\n";
    }
}

void DanceStudioApp::addGroupInteractive() {
    string name = readLine("Group name: ");
    showCoachesAndAdmins();
    cout << "Coach user ID: ";
    int coachUserId = readInt();

    int ui = findUserIndexById(coachUserId);
    if (ui == -1 || (users[ui].role != ROLE_COACH && users[ui].role != ROLE_ADMIN)) {
        cout << "Invalid coach/admin ID.\n";
        return;
    }

    cout << "Capacity: ";
    int capacity = readInt();
    string description = readLine("Description: ");
    string rules = readLine("Rules: ");

    int id = addGroupRecord(name, coachUserId, capacity, description, rules);
    cout << "Group added with ID " << id << ".\n";
}

void DanceStudioApp::assignDancerToGroupInteractive() {
    showUsersByRole(ROLE_DANCER);
    cout << "Dancer user ID: ";
    int userId = readInt();

    int ui = findUserIndexById(userId);
    if (ui == -1 || users[ui].role != ROLE_DANCER) {
        cout << "Invalid dancer ID.\n";
        return;
    }

    showGroups();
    cout << "Group ID: ";
    int groupId = readInt();

    if (findGroupIndexById(groupId) == -1) {
        cout << "Group not found.\n";
        return;
    }

    if (!assignUserToGroup(userId, groupId)) {
        cout << "Could not assign dancer to group.\n";
        return;
    }

    cout << "Dancer assigned to group.\n";
}

void DanceStudioApp::removeUserFromGroupInteractive() {
    showUsersByRole(ROLE_DANCER);
    cout << "Enter dancer user ID: ";
    int userId = readInt();

    int ui = findUserIndexById(userId);
    if (ui == -1 || users[ui].role != ROLE_DANCER) {
        cout << "Dancer not found.\n";
        return;
    }

    if (users[ui].groupCount == 0) {
        cout << "This dancer has no groups.\n";
        return;
    }

    cout << "\nDancer groups:\n";
    for (int i = 0; i < users[ui].groupCount; ++i) {
        int gi = findGroupIndexById(users[ui].groupIds[i]);
        if (gi != -1) {
            cout << groups[gi].id << ". " << groups[gi].name << "\n";
        }
    }

    cout << "Enter group ID to remove: ";
    int groupId = readInt();

    int pos = -1;
    for (int i = 0; i < users[ui].groupCount; ++i) {
        if (users[ui].groupIds[i] == groupId) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        cout << "This dancer is not in that group.\n";
        return;
    }

    for (int i = pos; i < users[ui].groupCount - 1; ++i) {
        users[ui].groupIds[i] = users[ui].groupIds[i + 1];
    }

    users[ui].groupIds[users[ui].groupCount - 1] = 0;
    users[ui].groupCount--;

    cout << "Dancer removed from group.\n";
}
