#include "DanceStudioApp.h"
#include <iostream>

using namespace std; 

void DanceStudioApp::dancerMenu(int userIndex) {
    while (true) {
        cout << "\n=== DANCER MENU ===\n";
        cout << "1. My calendar\n";
        cout << "2. All events / style filter\n";
        cout << "3. My groups\n";
        cout << "4. Membership section\n";
        cout << "5. Submit absence\n";
        cout << "6. Studio info\n";
        cout << "7. Styles info\n";
        cout << "8. Weekly schedule\n";
        cout << "9. Join group request\n";
        cout << "0. Logout\n";
        cout << "Choose: ";

        int choice = readInt();
        switch (choice) {
        case 1: showCalendarForUser(users[userIndex].id, true, ""); break;
        case 2: calendarFilterMenu(users[userIndex].id); break;
        case 3: showUserGroups(users[userIndex].id); break;
        case 4: membershipMenuForDancer(userIndex); break;
        case 5: submitAbsence(userIndex); break;
        case 6: showInfoSection(); break;
        case 7: showFullStylesInfo(); break;
        case 8: showWeeklyScheduleInfo(); break;
        case 9: submitJoinRequest(userIndex); break;
        case 0: saveAll(); return;
        default: cout << "Invalid choice.\n";
        }
    }
}

void DanceStudioApp::coachMenu(int userIndex) {
    while (true) {
        cout << "\n=== COACH MENU ===\n";
        cout << "1. My sessions\n";
        cout << "2. Cancel my session\n";
        cout << "3. Reschedule my session\n";
        cout << "4. View absence reasons\n";
        cout << "5. My Instagram link\n";
        cout << "0. Logout\n";
        cout << "Choose: ";
        int choice = readInt();
        switch (choice) {
        case 1: showCoachSessions(users[userIndex].id); break;
        case 2: cancelCoachSession(users[userIndex].id); break;
        case 3: rescheduleCoachSession(users[userIndex].id); break;
        case 4: showCoachAbsences(users[userIndex].id); break;
        case 5:
            cout << "Instagram: " << users[userIndex].instagram << "\n";
            break;
        case 0: saveAll(); return;
        default: cout << "Invalid choice.\n";
        }
    }
}

void DanceStudioApp::adminMenu(int userIndex) {
    while (true) {
        cout << "\n=== ADMIN MENU ===\n";
        cout << "1. Users\n";
        cout << "2. Groups\n";
        cout << "3. Sessions and events\n";
        cout << "4. Memberships\n";
        cout << "5. Studio info\n";
        cout << "6. Join requests\n";
        cout << "7. Schedule\n";
        cout << "8. Coach actions\n";
        cout << "0. Logout\n";
        cout << "Choose: ";

        int choice = readInt();

        switch (choice) {
        case 1: adminUsersMenu(); break;
        case 2: adminGroupsMenu(); break;
        case 3: adminSessionsEventsMenu(); break;
        case 4: adminMembershipsMenu(); break;
        case 5: adminStudioInfoMenu(); break;
        case 6: showJoinRequests(); break;
        case 7: showWeeklyScheduleInfo(); break;
        case 8: adminCoachMenu(users[userIndex].id); break;
        case 0: saveAll(); return;
        default: cout << "Invalid choice.\n";
        }
    }
}

void DanceStudioApp::adminUsersMenu() {
    while (true) {
        cout << "\n=== USERS ===\n";
        cout << "1. Show all users\n";
        cout << "2. Add coach\n";
        cout << "3. Delete user\n";
        cout << "4. Remove dancer from group\n";
        cout << "0. Back\n";
        cout << "Choose: ";

        int choice = readInt();

        switch (choice) {
        case 1: showAllUsers(); break;
        case 2: addUserInteractive(ROLE_COACH); break;
        case 3: deleteUserInteractive(); break;
        case 4: removeUserFromGroupInteractive(); break;
        case 0: return;
        default: cout << "Invalid choice.\n";
        }
    }
}

void DanceStudioApp::adminGroupsMenu() {
    while (true) {
        cout << "\n=== GROUPS ===\n";
        cout << "1. Add group\n";
        cout << "2. Assign dancer to group\n";
        cout << "0. Back\n";
        cout << "Choose: ";

        int choice = readInt();

        switch (choice) {
        case 1: addGroupInteractive(); break;
        case 2: assignDancerToGroupInteractive(); break;
        case 0: return;
        default: cout << "Invalid choice.\n";
        }
    }
}

void DanceStudioApp::adminSessionsEventsMenu() {
    while (true) {
        cout << "\n=== SESSIONS AND EVENTS ===\n";
        cout << "1. Add session\n";
        cout << "2. Show all sessions\n";
        cout << "3. Add event\n";
        cout << "4. Add to event\n";
        cout << "5. Remove user from event\n";
        cout << "6. Show event participants\n";
        cout << "0. Back\n";
        cout << "Choose: ";

        int choice = readInt();

        switch (choice) {
        case 1: addSessionInteractive(); break;
        case 2: showAllSessions(); break;
        case 3: addEventInteractive(); break;
        case 4: addToEventMenu(); break;
        case 5: removeUserFromEventInteractive(); break;
        case 6: showEventParticipantsInteractive(); break;
        case 0: return;
        default: cout << "Invalid choice.\n";
        }
    }
}

void DanceStudioApp::adminMembershipsMenu() {
    while (true) {
        cout << "\n=== MEMBERSHIPS ===\n";
        cout << "1. Activate membership\n";
        cout << "2. Show all memberships\n";
        cout << "0. Back\n";
        cout << "Choose: ";

        int choice = readInt();

        switch (choice) {
        case 1: activateMembershipInteractive(); break;
        case 2: showAllMemberships(); break;
        case 0: return;
        default: cout << "Invalid choice.\n";
        }
    }
}


void DanceStudioApp::adminStudioInfoMenu() {
    while (true) {
        cout << "\n=== STUDIO INFO ===\n";
        cout << "1. Show studio info\n";
        cout << "2. Edit studio info\n";
        cout << "0. Back\n";
        cout << "Choose: ";

        int choice = readInt();

        switch (choice) {
        case 1: showInfoSection(); break;
        case 2: editStudioInfo(); break;
        case 0: return;
        default: cout << "Invalid choice.\n";
        }
    }
}

void DanceStudioApp::adminCoachMenu(int adminUserId) {
    while (true) {
        cout << "\n=== COACH ACTIONS ===\n";
        cout << "1. My sessions\n";
        cout << "2. Cancel session\n";
        cout << "3. Reschedule session\n";
        cout << "4. View absence reasons\n";
        cout << "0. Back\n";
        cout << "Choose: ";

        int choice = readInt();

        switch (choice) {
        case 1: showCoachSessions(adminUserId); break;
        case 2: cancelCoachSession(adminUserId); break;
        case 3: rescheduleCoachSession(adminUserId); break;
        case 4: showCoachAbsences(adminUserId); break;
        case 0: return;
        default: cout << "Invalid choice.\n";
        }
    }
}
