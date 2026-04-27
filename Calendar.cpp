#include "DanceStudioApp.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <cstddef>

using namespace std; 

void DanceStudioApp::showCalendarForUser(int userId, bool onlyMine, const string& styleFilter) {
    cout << "\n=== CALENDAR ===\n";

    vector<Session> sessionItems;
    vector<Event> eventItems;

    for (size_t i = 0; i < sessions.size(); ++i) {
        int gi = findGroupIndexById(sessions[i].groupId);
        if (gi == -1) continue;

        bool include = true;

        if (onlyMine && !userInGroup(userId, sessions[i].groupId)) include = false;

        if (!styleFilter.empty()) {
            string groupName = toLowerText(groups[gi].name);
            string filter = toLowerText(styleFilter);
            if (groupName.find(filter) == string::npos) include = false;
        }

        if (include) sessionItems.push_back(sessions[i]);
    }

    for (size_t i = 0; i < events.size(); ++i) {
        bool include = true;
        if (onlyMine && !eventAssignedToUser(events[i], userId)) include = false;
        if (include) eventItems.push_back(events[i]);
    }

    sort(sessionItems.begin(), sessionItems.end(), compareSessionDateTime);
    sort(eventItems.begin(), eventItems.end(), compareEventDateTime);

    cout << "\n--- SESSIONS ---\n";
    if (sessionItems.empty()) {
        cout << "No sessions to show.\n";
    }
    else {
        for (size_t i = 0; i < sessionItems.size(); ++i) {
            int gi = findGroupIndexById(sessionItems[i].groupId);
            cout << "Session ID: " << sessionItems[i].id
                << " | Group: " << (gi == -1 ? "Unknown" : groups[gi].name)
                << " | Date: " << sessionItems[i].date
                << " | Time: " << sessionItems[i].time
                << " | Status: " << (sessionItems[i].cancelled ? "Cancelled" : "Active")
                << "\n";
        }
    }

    cout << "\n--- EVENTS ---\n";
    if (eventItems.empty()) {
        cout << "No events to show.\n";
    }
    else {
        for (size_t i = 0; i < eventItems.size(); ++i) {
            cout << "Event ID: " << eventItems[i].id
                << " | " << eventTypeToString(eventItems[i].type)
                << " | Title: " << eventItems[i].title
                << " | Date: " << eventItems[i].date
                << " | Time: " << eventItems[i].time
                << "\n";
        }
    }
}

 void DanceStudioApp::calendarFilterMenu(int userId) {
    cout << "\n=== CALENDAR FILTER ===\n";
    cout << "1. Show only my events\n";
    cout << "2. Show all events\n";
    cout << "3. Show only one style\n";
    cout << "Choose: ";
    int choice = readInt();

    if (choice == 1) {
        showCalendarForUser(userId, true, "");
    }
    else if (choice == 2) {
        showCalendarForUser(userId, false, "");
    }
    else if (choice == 3) {
        string style = readLine("Enter style keyword (e.g. jazz, heels, contemporary): ");
        showCalendarForUser(userId, false, style);
    }
    else {
        cout << "Invalid choice.\n";
    }
}
