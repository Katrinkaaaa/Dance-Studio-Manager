#include "DanceStudioApp.h"
#include <iostream>
#include <cstddef>

using namespace std;

void DanceStudioApp::showEvents() const {
    if (events.empty()) {
        cout << "No events found.\n";
        return;
    }

    for (size_t i = 0; i < events.size(); ++i) {
        cout << "Event ID: " << events[i].id
            << " | " << eventTypeToString(events[i].type)
            << " | Title: " << events[i].title
            << " | Date: " << events[i].date
            << " | Time: " << events[i].time
            << "\n";
    }
}

void DanceStudioApp::addEventInteractive() {
    cout << "Type (1-Competition, 2-Concert, 3-General): ";
    int type = readInt();
    if (type < EVENT_COMPETITION || type > EVENT_GENERAL) {
        cout << "Invalid event type.\n";
        return;
    }

    string title = readLine("Title: ");
    cout << "Date (YYYYMMDD): ";
    int date = readInt();
    cout << "Time (HHMM): ";
    int time = readInt();
    string note = readLine("Note: ");

    int id = addEventRecord(type, title, date, time, note);
    cout << "Event added with ID " << id << ".\n";
}

void DanceStudioApp::assignUserToEventInteractive() {
    showAllUsers();
    cout << "User ID: ";
    int userId = readInt();
    if (findUserIndexById(userId) == -1) {
        cout << "User not found.\n";
        return;
    }

    showEvents();
    cout << "Event ID: ";
    int eventId = readInt();

    if (!assignUserToEvent(userId, eventId)) {
        cout << "Could not assign user to event.\n";
        return;
    }

    cout << "User assigned to event.\n";
}

void DanceStudioApp::addToEventMenu() {
    while (true) {
        cout << "\n=== ADD TO EVENT ===\n";
        cout << "1. Add one user\n";
        cout << "2. Add full group\n";
        cout << "3. Add all dancers in studio\n";
        cout << "0. Back\n";
        cout << "Choose: ";

        int choice = readInt();

        switch (choice) {
        case 1: assignUserToEventInteractive(); break;
        case 2: addGroupToEventInteractive(); break;
        case 3: addAllDancersToEventInteractive(); break;
        case 0: return;
        default: cout << "Invalid choice.\n";
        }
    }
}

void DanceStudioApp::removeUserFromEventInteractive() {
    showEvents();
    cout << "Event ID: ";
    int eventId = readInt();

    int eventIndex = -1;
    for (size_t i = 0; i < events.size(); ++i) {
        if (events[i].id == eventId) {
            eventIndex = static_cast<int>(i);
            break;
        }
    }

    if (eventIndex == -1) {
        cout << "Event not found.\n";
        return;
    }

    cout << "\nParticipants:\n";
    for (int i = 0; i < events[eventIndex].participantCount; ++i) {
        int ui = findUserIndexById(events[eventIndex].participantIds[i]);
        if (ui != -1) {
            cout << users[ui].id << ". " << users[ui].name << "\n";
        }
    }

    cout << "User ID to remove: ";
    int userId = readInt();

    int pos = -1;
    for (int i = 0; i < events[eventIndex].participantCount; ++i) {
        if (events[eventIndex].participantIds[i] == userId) {
            pos = i;
            break;
        }
    }

    if (pos == -1) {
        cout << "User is not in this event.\n";
        return;
    }

    for (int i = pos; i < events[eventIndex].participantCount - 1; ++i) {
        events[eventIndex].participantIds[i] = events[eventIndex].participantIds[i + 1];
    }

    events[eventIndex].participantIds[events[eventIndex].participantCount - 1] = 0;
    events[eventIndex].participantCount--;

    cout << "User removed from event.\n";
}

void DanceStudioApp::addGroupToEventInteractive() {
    showEvents();
    cout << "Event ID: ";
    int eventId = readInt();

    int eventIndex = -1;
    for (size_t i = 0; i < events.size(); ++i) {
        if (events[i].id == eventId) {
            eventIndex = static_cast<int>(i);
            break;
        }
    }

    if (eventIndex == -1) {
        cout << "Event not found.\n";
        return;
    }

    while (true) {
        cout << "\nAvailable groups (0 to finish):\n";
        showGroups();
        cout << "Group ID: ";
        int groupId = readInt();

        if (groupId == 0) {
            cout << "Finished adding groups to event.\n";
            return;
        }

        int gi = findGroupIndexById(groupId);
        if (gi == -1) {
            cout << "Group not found.\n";
            continue;
        }

        int added = 0;
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].role != ROLE_DANCER) continue;
            if (!userInGroup(users[i].id, groupId)) continue;

            if (!eventAssignedToUser(events[eventIndex], users[i].id)) {
                if (events[eventIndex].participantCount < MAX_PARTICIPANTS) {
                    events[eventIndex].participantIds[events[eventIndex].participantCount++] = users[i].id;
                    added++;
                }
            }
        }

        cout << "Added " << added << " dancers from group \"" << groups[gi].name << "\".\n";
    }
}

void DanceStudioApp::addAllDancersToEventInteractive() {
    showEvents();
    cout << "Event ID: ";
    int eventId = readInt();

    int eventIndex = -1;
    for (size_t i = 0; i < events.size(); ++i) {
        if (events[i].id == eventId) {
            eventIndex = static_cast<int>(i);
            break;
        }
    }

    if (eventIndex == -1) {
        cout << "Event not found.\n";
        return;
    }

    int added = 0;
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i].role != ROLE_DANCER) continue;

        if (!eventAssignedToUser(events[eventIndex], users[i].id)) {
            if (events[eventIndex].participantCount < MAX_PARTICIPANTS) {
                events[eventIndex].participantIds[events[eventIndex].participantCount++] = users[i].id;
                added++;
            }
        }
    }

    cout << "Added " << added << " dancers from the whole studio to the event.\n";
}

void DanceStudioApp::showEventParticipantsInteractive() {
    showEvents();
    cout << "Event ID: ";
    int eventId = readInt();

    int eventIndex = -1;
    for (size_t i = 0; i < events.size(); ++i) {
        if (events[i].id == eventId) {
            eventIndex = static_cast<int>(i);
            break;
        }
    }

    if (eventIndex == -1) {
        cout << "Event not found.\n";
        return;
    }

    cout << "\n=== EVENT PARTICIPANTS ===\n";
    cout << "Event: " << events[eventIndex].title << "\n";

    if (events[eventIndex].participantCount == 0) {
        cout << "No participants in this event.\n";
        return;
    }

    for (int i = 0; i < events[eventIndex].participantCount; ++i) {
        int ui = findUserIndexById(events[eventIndex].participantIds[i]);
        if (ui != -1) {
            cout << users[ui].id << ". " << users[ui].name
                << " | Phone: " << users[ui].phone << "\n";
        }
    }
}