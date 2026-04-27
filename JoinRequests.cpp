#include "DanceStudioApp.h"
#include <iostream>
#include <cstring>
#include <cstddef>
#include <vector>

using namespace std; 

void DanceStudioApp::submitJoinRequest(int userIndex) {
    cout << "\n=== JOIN REQUEST ===\n";

    showLevelDescriptions();

    cout << "Do you want to view full style information? (y/n): ";
    char choice;
    cin >> choice;
    cin.ignore();

    if (choice == 'y' || choice == 'Y') {
        showFullStylesInfo();
    }

    // show groups
    showAvailableGroupsForRequests();

    cout << "How many groups do you want to apply for? ";
    int count = readInt();

    if (count <= 0) {
        cout << "Invalid number.\n";
        return;
    }

    vector<int> selectedGroups;

    for (int i = 0; i < count; ++i) {
        cout << "Enter group number: ";
        int groupId = readInt();

        if (findGroupIndexById(groupId) == -1) {
            cout << "Invalid group ID.\n";
            --i;
            continue;
        }

        selectedGroups.push_back(groupId);
    }

    // creating a request
    JoinRequest req;
    req.id = nextJoinRequestId();

    // users info
    strcpy_s(req.firstName, sizeof(req.firstName), users[userIndex].name);
    strcpy_s(req.lastName, sizeof(req.lastName), "");
    req.age = 0;

    strcpy_s(req.experience, sizeof(req.experience), "");
    strcpy_s(req.parentPhone, sizeof(req.parentPhone), users[userIndex].phone);

    strcpy_s(req.phone, sizeof(req.phone), users[userIndex].phone);
    strcpy_s(req.password, sizeof(req.password), users[userIndex].password);

    req.groupCount = static_cast<int>(selectedGroups.size());

    for (int i = 0; i < req.groupCount; ++i) {
        req.groupIds[i] = selectedGroups[i];
    }

    req.processed = false;

    joinRequests.push_back(req);

    cout << "Request submitted successfully! Waiting for admin approval.\n";
}

void DanceStudioApp::showJoinRequests() {
    cout << "\n=== JOIN REQUESTS ===\n";

    bool found = false;
    for (size_t i = 0; i < joinRequests.size(); ++i) {
        if (joinRequests[i].processed) continue;

        found = true;
        cout << "Request ID: " << joinRequests[i].id << "\n";
        cout << "Name: " << joinRequests[i].firstName << " " << joinRequests[i].lastName << "\n";
        cout << "Age: " << joinRequests[i].age << "\n";
        cout << "Experience: " << joinRequests[i].experience << "\n";

        if (strlen(joinRequests[i].parentPhone) > 0) {
            cout << "Parent phone: " << joinRequests[i].parentPhone << "\n";
        }

        cout << "Requested groups:\n";
        for (int j = 0; j < joinRequests[i].groupCount; ++j) {
            int gi = findGroupIndexById(joinRequests[i].groupIds[j]);
            if (gi != -1) {
                cout << "- " << groups[gi].name << "\n";
            }
        }

        cout << "-----------------------------\n";
    }

    if (!found) {
        cout << "No pending join requests.\n";
        return;
    }

    cout << "\nEnter request ID to process (0 to back): ";
    int requestId = readInt();

    if (requestId == 0) return;

    int requestIndex = -1;
    for (size_t i = 0; i < joinRequests.size(); ++i) {
        if (joinRequests[i].id == requestId && !joinRequests[i].processed) {
            requestIndex = static_cast<int>(i);
            break;
        }
    }

    if (requestIndex == -1) {
        cout << "Request not found.\n";
        return;
    }

    cout << "1. Approve\n";
    cout << "2. Reject\n";
    cout << "Choose: ";
    int action = readInt();

    if (action == 1) {
        JoinRequest& r = joinRequests[requestIndex];

        int userId = findUserIdByPhone(r.phone);
        int ui = findUserIndexById(userId);

        if (ui == -1) {
            cout << "User for this request not found.\n";
            return;
        }

        users[ui].status = 1;

        cout << "\nRequested groups:\n";
        for (int j = 0; j < r.groupCount; ++j) {
            int gi = findGroupIndexById(r.groupIds[j]);
            if (gi != -1) {
                cout << groups[gi].id << ". " << groups[gi].name << "\n";
            }
        }

        cout << "How many groups do you approve for this dancer? ";
        int approvedCount = readInt();

        for (int j = 0; j < approvedCount; ++j) {
            cout << "Enter approved group ID: ";
            int groupId = readInt();
            assignUserToGroup(users[ui].id, groupId);
        }

        r.processed = true;
        cout << "Request approved.\n";
    }
    else if (action == 2) {
        JoinRequest& r = joinRequests[requestIndex];

        int userId = findUserIdByPhone(r.phone);
        int ui = findUserIndexById(userId);

        if (ui != -1) {
            users[ui].status = 2;
        }

        r.processed = true;
        cout << "Request rejected.\n";
    }
    else {
        cout << "Invalid action.\n";
    }
}
