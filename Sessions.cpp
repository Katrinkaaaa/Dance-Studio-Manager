#include "DanceStudioApp.h"
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <vector>

using namespace std;

void DanceStudioApp::showAllSessions() {
    if (sessions.empty()) {
        cout << "No sessions found.\n";
        return;
    }

    cout << left
        << setw(5) << "ID"
        << setw(28) << "Group"
        << setw(12) << "Day"
        << setw(8) << "Time"
        << setw(10) << "Duration"
        << setw(12) << "Recurring"
        << setw(12) << "Status"
        << "Note\n";
    cout << string(105, '-') << "\n";

    for (size_t i = 0; i < sessions.size(); ++i) {
        int gi = findGroupIndexById(sessions[i].groupId);
        string groupName = (gi == -1) ? "Unknown" : groups[gi].name;

        cout << left
            << setw(5) << sessions[i].id
            << setw(28) << groupName
            << setw(12) << dayOfWeekToString(sessions[i].dayOfWeek)
            << setw(8) << sessions[i].time
            << setw(10) << sessions[i].durationMin
            << setw(12) << (sessions[i].recurring ? "Yes" : "No")
            << setw(12) << (sessions[i].cancelled ? "Cancelled" : "Active")
            << sessions[i].note << "\n";
    }
}

void DanceStudioApp::addSessionInteractive() {
    showGroups();
    cout << "Group ID: ";
    int groupId = readInt();

    if (findGroupIndexById(groupId) == -1) {
        cout << "Group not found.\n";
        return;
    }

    cout << "Day of week (1 Monday ... 7 Sunday): ";
    int dayOfWeek = readInt();

    if (dayOfWeek < 1 || dayOfWeek > 7) {
        cout << "Invalid day of week.\n";
        return;
    }

    cout << "Time (HHMM): ";
    int time = readInt();

    cout << "Duration in minutes: ";
    int duration = readInt();

    bool recurring = readYesNo("Is this a weekly recurring session?");
    string note = readLine("Note: ");

    int id = addSessionRecord(groupId, dayOfWeek, time, duration, note, recurring);
    cout << "Session added with ID " << id << ".\n";
}

void DanceStudioApp::cancelCoachSession(int coachUserId) {
    showCoachSessions(coachUserId);
    cout << "Enter session ID to cancel: ";
    int sessionId = readInt();
    if (!coachOwnsSession(coachUserId, sessionId)) {
        cout << "You can cancel only your own sessions.\n";
        return;
    }

    int si = findSessionIndexById(sessionId);
    if (si == -1) {
        cout << "Session not found.\n";
        return;
    }

    sessions[si].cancelled = true;
    string note = readLine("Enter cancellation note: ");
    copyText(sessions[si].note, MAX_TEXT, note);
    cout << "Session cancelled.\n";
}

void DanceStudioApp::rescheduleCoachSession(int coachUserId) {
    showCoachSessions(coachUserId);
    cout << "Enter session ID to reschedule: ";
    int sessionId = readInt();

    if (!coachOwnsSession(coachUserId, sessionId)) {
        cout << "You can reschedule only your own sessions.\n";
        return;
    }

    int si = findSessionIndexById(sessionId);
    if (si == -1) {
        cout << "Session not found.\n";
        return;
    }

    cout << "Enter new day of week (1 Monday ... 7 Sunday): ";
    sessions[si].dayOfWeek = readInt();

    cout << "Enter new time (HHMM): ";
    sessions[si].time = readInt();

    string note = readLine("Enter note for reschedule: ");
    copyText(sessions[si].note, MAX_TEXT, note);
    sessions[si].cancelled = false;

    cout << "Session rescheduled.\n";
}

void DanceStudioApp::showCoachSessions(int coachUserId) {
    vector<Session> ownSessions;

    for (size_t i = 0; i < sessions.size(); ++i) {
        if (coachOwnsSession(coachUserId, sessions[i].id)) {
            ownSessions.push_back(sessions[i]);
        }
    }

    if (ownSessions.empty()) {
        cout << "No sessions found.\n";
        return;
    }

    cout << left
        << setw(5) << "ID"
        << setw(28) << "Group"
        << setw(12) << "Day"
        << setw(8) << "Time"
        << setw(10) << "Duration"
        << setw(12) << "Recurring"
        << setw(12) << "Status"
        << "Note\n";

    cout << string(105, '-') << "\n";

    for (size_t i = 0; i < ownSessions.size(); ++i) {
        int gi = findGroupIndexById(ownSessions[i].groupId);

        cout << left
            << setw(5) << ownSessions[i].id
            << setw(28) << (gi == -1 ? "Unknown" : groups[gi].name)
            << setw(12) << dayOfWeekToString(ownSessions[i].dayOfWeek)
            << setw(8) << ownSessions[i].time
            << setw(10) << ownSessions[i].durationMin
            << setw(12) << (ownSessions[i].recurring ? "Yes" : "No")
            << setw(12) << (ownSessions[i].cancelled ? "Cancelled" : "Active")
            << ownSessions[i].note << "\n";
    }
}
