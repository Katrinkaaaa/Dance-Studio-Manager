#include "DanceStudioApp.h"
#include <fstream>
#include <cstddef>

using namespace std;

void DanceStudioApp::loadAll() {
    loadUsers();
    loadGroups();
    loadSessions();
    loadMemberships();
    loadEvents();
    loadAbsences();
    loadStudioInfo();
    loadJoinRequests();
}

void DanceStudioApp::saveAll() {
    saveUsers();
    saveGroups();
    saveSessions();
    saveMemberships();
    saveEvents();
    saveAbsences();
    saveStudioInfo();
    saveJoinRequests();
}

void DanceStudioApp::loadUsers() {
    users.clear();
    ifstream file(USERS_FILE, ios::binary);
    if (!file.is_open()) return;
    User item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(User))) users.push_back(item);
    file.close();
}

void DanceStudioApp::saveUsers() {
    ofstream file(USERS_FILE, ios::binary | ios::trunc);
    for (size_t i = 0; i < users.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&users[i]), sizeof(User));
    }
    file.close();
}

void DanceStudioApp::loadGroups() {
    groups.clear();
    ifstream file(GROUPS_FILE, ios::binary);
    if (!file.is_open()) return;
    Group item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(Group))) groups.push_back(item);
    file.close();
}

void DanceStudioApp::saveGroups() {
    ofstream file(GROUPS_FILE, ios::binary | ios::trunc);
    for (size_t i = 0; i < groups.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&groups[i]), sizeof(Group));
    }
    file.close();
}

void DanceStudioApp::loadSessions() {
    sessions.clear();
    ifstream file(SESSIONS_FILE, ios::binary);
    if (!file.is_open()) return;
    Session item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(Session))) sessions.push_back(item);
    file.close();
}
void DanceStudioApp::saveSessions() {
    ofstream file(SESSIONS_FILE, ios::binary | ios::trunc);
    for (size_t i = 0; i < sessions.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&sessions[i]), sizeof(Session));
    }
    file.close();
}

void DanceStudioApp::loadMemberships() {
    memberships.clear();
    ifstream file(MEMBERSHIPS_FILE, ios::binary);
    if (!file.is_open()) return;
    Membership item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(Membership))) memberships.push_back(item);
    file.close();
}

void DanceStudioApp::saveMemberships() {
    ofstream file(MEMBERSHIPS_FILE, ios::binary | ios::trunc);
    for (size_t i = 0; i < memberships.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&memberships[i]), sizeof(Membership));
    }
    file.close();
}

void DanceStudioApp::loadEvents() {
    events.clear();
    ifstream file(EVENTS_FILE, ios::binary);
    if (!file.is_open()) return;
    Event item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(Event))) events.push_back(item);
    file.close();
}

void DanceStudioApp::saveEvents() {
    ofstream file(EVENTS_FILE, ios::binary | ios::trunc);
    for (size_t i = 0; i < events.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&events[i]), sizeof(Event));
    }
    file.close();
}

void DanceStudioApp::loadAbsences() {
    absences.clear();
    ifstream file(ABSENCES_FILE, ios::binary);
    if (!file.is_open()) return;
    Absence item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(Absence))) absences.push_back(item);
    file.close();
}

void DanceStudioApp::saveAbsences() {
    ofstream file(ABSENCES_FILE, ios::binary | ios::trunc);
    for (size_t i = 0; i < absences.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&absences[i]), sizeof(Absence));
    }
    file.close();
}

void DanceStudioApp::loadStudioInfo() {
    ifstream file(STUDIO_INFO_FILE, ios::binary);
    if (!file.is_open()) return;
    file.read(reinterpret_cast<char*>(&studioInfo), sizeof(StudioInfo));
    file.close();
}

void DanceStudioApp::saveStudioInfo() {
    ofstream file(STUDIO_INFO_FILE, ios::binary | ios::trunc);
    file.write(reinterpret_cast<const char*>(&studioInfo), sizeof(StudioInfo));
    file.close();
}

void DanceStudioApp::loadJoinRequests() {
    joinRequests.clear();

    ifstream file(JOIN_REQUESTS_FILE, ios::binary);
    if (!file.is_open()) return;

    JoinRequest item;
    while (file.read(reinterpret_cast<char*>(&item), sizeof(JoinRequest))) {
        joinRequests.push_back(item);
    }

    file.close();
}

void DanceStudioApp::saveJoinRequests() {
    ofstream file(JOIN_REQUESTS_FILE, ios::binary | ios::trunc);

    for (size_t i = 0; i < joinRequests.size(); ++i) {
        file.write(reinterpret_cast<const char*>(&joinRequests[i]), sizeof(JoinRequest));
    }

    file.close();
}
