#include "DanceStudioApp.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstddef>

using namespace std;

void DanceStudioApp::submitAbsence(int userIndex) {
    int userId = users[userIndex].id;
    cout << "\n=== SUBMIT ABSENCE ===\n";

    vector<Session> myUpcoming;
    for (size_t i = 0; i < sessions.size(); ++i) {
        if (userInGroup(userId, sessions[i].groupId)) {
            myUpcoming.push_back(sessions[i]);
        }
    }

    if (myUpcoming.empty()) {
        cout << "No sessions found for your groups.\n";
        return;
    }

    sort(myUpcoming.begin(), myUpcoming.end(), compareSessionDateTime);
    for (size_t i = 0; i < myUpcoming.size(); ++i) {
        int gi = findGroupIndexById(myUpcoming[i].groupId);
        cout << "Session ID: " << myUpcoming[i].id
            << " | Group: " << (gi == -1 ? "Unknown" : groups[gi].name)
            << " | Date: " << myUpcoming[i].date
            << " | Time: " << myUpcoming[i].time
            << " | Status: " << (myUpcoming[i].cancelled ? "Cancelled" : "Active")
            << "\n";
    }

    cout << "Enter session ID: ";
    int sessionId = readInt();
    int si = findSessionIndexById(sessionId);

    if (si == -1) {
        cout << "Session not found.\n";
        return;
    }
    if (!userInGroup(userId, sessions[si].groupId)) {
        cout << "This is not your session.\n";
        return;
    }
    if (sessions[si].cancelled) {
        cout << "This session is already cancelled.\n";
        return;
    }
    if (findAbsenceIndexByUserAndSession(userId, sessionId) != -1) {
        cout << "Absence for this session has already been submitted.\n";
        return;
    }

    string reason = readLine("Enter reason for absence: ");

    int currentDate = getCurrentDate();
    int currentTime = getCurrentTimeHHMM();

    time_t sessionTime = toTimeValue(sessions[si].date, sessions[si].time);
    time_t submitTime = toTimeValue(currentDate, currentTime);
    double hoursDiff = difftime(sessionTime, submitTime) / 3600.0;

    Absence a;
    a.id = nextAbsenceId();
    a.userId = userId;
    a.sessionId = sessionId;
    copyText(a.reason, MAX_TEXT, reason);
    a.submittedDateTime = buildDateTimeNumber(currentDate, currentTime);
    a.late = hoursDiff < 3.0;
    a.deducted = false;

    if (a.late) {
        Membership* m = findActiveMembershipForUser(userId, currentDate);
        if (m != nullptr && m->classesTotal != 0) {
            m->classesUsed++;
            a.deducted = true;
        }
    }

    absences.push_back(a);

    if (a.late) {
        cout << "Absence submitted late. ";
        if (a.deducted) {
            cout << "One class was deducted from your membership.\n";
        }
        else {
            cout << "No class was deducted (no active limited membership found).\n";
        }
    }
    else {
        cout << "Absence submitted on time. No class deducted.\n";
    }
}

void DanceStudioApp::showCoachAbsences(int coachUserId) {
    cout << "\n=== ABSENCE REASONS FOR MY SESSIONS ===\n";
    bool found = false;

    for (size_t i = 0; i < absences.size(); ++i) {
        int si = findSessionIndexById(absences[i].sessionId);
        if (si == -1) continue;
        if (!coachOwnsSession(coachUserId, absences[i].sessionId)) continue;

        int ui = findUserIndexById(absences[i].userId);
        int gi = findGroupIndexById(sessions[si].groupId);

        found = true;
        cout << "Absence ID: " << absences[i].id << "\n";
        cout << "Dancer: " << (ui == -1 ? "Unknown" : users[ui].name) << "\n";
        cout << "Group: " << (gi == -1 ? "Unknown" : groups[gi].name) << "\n";
        cout << "Session date: " << sessions[si].date << " " << sessions[si].time << "\n";
        cout << "Reason: " << absences[i].reason << "\n";
        cout << "Late: " << (absences[i].late ? "Yes" : "No") << "\n";
        cout << "Deducted: " << (absences[i].deducted ? "Yes" : "No") << "\n";
        cout << "-----------------------------\n";
    }

    if (!found) {
        cout << "No absences found for your sessions.\n";
    }
}
