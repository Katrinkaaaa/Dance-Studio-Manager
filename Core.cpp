#include "DanceStudioApp.h"
#include <iostream>
#include <string>
#include <cstddef>

using namespace std; 

void DanceStudioApp::run() {
    loadAll();
    seedDataIfNeeded();

    while (true) {
        int userIndex = login();

        if (userIndex == -1) {
            saveAll();
            cout << "Goodbye.\n";
            return;
        }

        if (userIndex == -2) {
            saveAll();
            continue;
        }

        if (userIndex == -3) {
            continue;
        }

        if (users[userIndex].role == ROLE_DANCER) {
            dancerMenu(userIndex);
        }
        else if (users[userIndex].role == ROLE_COACH) {
            coachMenu(userIndex);
        }
        else if (users[userIndex].role == ROLE_ADMIN) {
            adminMenu(userIndex);
        }
    }
}

// ---------- ids ----------
int DanceStudioApp::nextUserId() const { return maxIdUsers() + 1; }
int DanceStudioApp::nextGroupId() const { return maxIdGroups() + 1; }
int DanceStudioApp::nextSessionId() const { return maxIdSessions() + 1; }
int DanceStudioApp::nextMembershipId() const { return maxIdMemberships() + 1; }
int DanceStudioApp::nextEventId() const { return maxIdEvents() + 1; }
int DanceStudioApp::nextAbsenceId() const { return maxIdAbsences() + 1; }

int DanceStudioApp::nextJoinRequestId() const { return maxIdJoinRequests() + 1; }

int DanceStudioApp::maxIdJoinRequests() const {
    int m = 0;
    for (size_t i = 0; i < joinRequests.size(); ++i) {
        if (joinRequests[i].id > m) m = joinRequests[i].id;
    }
    return m;
}

int DanceStudioApp::maxIdUsers() const {
    int m = 0;
    for (size_t i = 0; i < users.size(); ++i) if (users[i].id > m) m = users[i].id;
    return m;
}

int DanceStudioApp::maxIdGroups() const {
    int m = 0;
    for (size_t i = 0; i < groups.size(); ++i) if (groups[i].id > m) m = groups[i].id;
    return m;
}

int DanceStudioApp::maxIdSessions() const {
    int m = 0;
    for (size_t i = 0; i < sessions.size(); ++i) if (sessions[i].id > m) m = sessions[i].id;
    return m;
}

int DanceStudioApp::maxIdMemberships() const {
    int m = 0;
    for (size_t i = 0; i < memberships.size(); ++i) if (memberships[i].id > m) m = memberships[i].id;
    return m;
}

int DanceStudioApp::maxIdEvents() const {
    int m = 0;
    for (size_t i = 0; i < events.size(); ++i) if (events[i].id > m) m = events[i].id;
    return m;
}

int DanceStudioApp::maxIdAbsences() const {
    int m = 0;
    for (size_t i = 0; i < absences.size(); ++i) if (absences[i].id > m) m = absences[i].id;
    return m;
}

// ---------- find helpers ----------
int DanceStudioApp::findUserIndexById(int id) const {
    for (size_t i = 0; i < users.size(); ++i) if (users[i].id == id) return static_cast<int>(i);
    return -1;
}

int DanceStudioApp::findUserIdByPhone(const string& phone) const {
    for (size_t i = 0; i < users.size(); ++i)
        if (phone == users[i].phone) return users[i].id;
    return -1;
}

int DanceStudioApp::findGroupIndexById(int id) const {
    for (size_t i = 0; i < groups.size(); ++i) if (groups[i].id == id) return static_cast<int>(i);
    return -1;
}

int DanceStudioApp::findSessionIndexById(int id) const {
    for (size_t i = 0; i < sessions.size(); ++i) if (sessions[i].id == id) return static_cast<int>(i);
    return -1;
}

int DanceStudioApp::findMembershipIndexById(int id) const {
    for (size_t i = 0; i < memberships.size(); ++i) if (memberships[i].id == id) return static_cast<int>(i);
    return -1;
}

int DanceStudioApp::findAbsenceIndexByUserAndSession(int userId, int sessionId) const {
    for (size_t i = 0; i < absences.size(); ++i) {
        if (absences[i].userId == userId && absences[i].sessionId == sessionId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool DanceStudioApp::userInGroup(int userId, int groupId) const {
    int userIndex = findUserIndexById(userId);
    if (userIndex == -1) return false;
    for (int i = 0; i < users[userIndex].groupCount; ++i) {
        if (users[userIndex].groupIds[i] == groupId) return true;
    }
    return false;
}

bool DanceStudioApp::eventAssignedToUser(const Event& eventItem, int userId) const {
    for (int i = 0; i < eventItem.participantCount; ++i) {
        if (eventItem.participantIds[i] == userId) return true;
    }
    return false;
}

bool DanceStudioApp::coachOwnsGroup(int coachUserId, int groupId) const {
    int gi = findGroupIndexById(groupId);
    if (gi == -1) return false;
    return groups[gi].coachUserId == coachUserId;
}
bool DanceStudioApp::coachOwnsSession(int coachUserId, int sessionId) const {
    int si = findSessionIndexById(sessionId);
    if (si == -1) return false;
    return coachOwnsGroup(coachUserId, sessions[si].groupId);
}

// ---------- seed ----------
void DanceStudioApp::seedDataIfNeeded() {
    if (!users.empty() || !groups.empty()) return;

    copyText(studioInfo.contemporaryInfo, MAX_TEXT,
        "Contemporary (12+): soft, fluid, expressive dance based on technique and control.");
    copyText(studioInfo.heelsInfo, MAX_TEXT,
        "Heels (16+): confidence, posture, femininity, balance and control.");
    copyText(studioInfo.jazzFunkInfo, MAX_TEXT,
        "Jazz Funk (12+): energy, rhythm, coordination, sharp accents and stage presence.");
    copyText(studioInfo.jazzOpenInfo, MAX_TEXT,
        "Jazz Open (14+): freedom inside structure, technique, musicality, jumps and turns.");
    copyText(studioInfo.ladyStyleInfo, MAX_TEXT,
        "Lady Style (30+): feminine energy, expression, softness, strength and character.");
    copyText(studioInfo.showGroupInfo, MAX_TEXT,
        "Show Group (15+, casting): studio performance team for experienced selected dancers.");
    copyText(studioInfo.stretchingInfo, MAX_TEXT,
        "Stretching & Acrobatics: flexibility, strength and body control.");
    copyText(studioInfo.softStretchingInfo, MAX_TEXT,
        "Soft Stretching: gentle flexibility and recovery class.");

    int elizavetaId = addUserRecord("Elizaveta", "00000000", "1234", ROLE_COACH,
        "https://instagram.com/elizaveta_placeholder");
    int kamilaId = addUserRecord("Kamila", "11111111", "1234", ROLE_COACH,
        "https://instagram.com/kamila_placeholder");
    int patriciaId = addUserRecord("Patricia", "22222222", "1234", ROLE_COACH,
        "https://instagram.com/patricia_placeholder");
    int anastasiaId = addUserRecord("Anastasia", "33333333", "1234", ROLE_COACH,
        "https://instagram.com/anastasia_placeholder");
    int mariaId = addUserRecord("Maria", "44444444", "1234", ROLE_ADMIN,
        "https://instagram.com/maria_placeholder");
    int dancerId = addUserRecord("Demo Dancer", "67676767", "1234", ROLE_DANCER, "");

    int jazzOpenId = addGroupRecord("Jazz Open", mariaId, 14,
        "Open jazz group, age recommendation 10+.",
        "Evening group, regular attendance is recommended.");
    int jazzFunkBegId = addGroupRecord("Jazz Funk Beginner", patriciaId, 16,
        "Jazz Funk beginners, age recommendation 10+.",
        "Suitable for beginners.");
    int jazzFunkIntId = addGroupRecord("Jazz Funk Intermediate", patriciaId, 14,
        "Jazz Funk intermediate, age recommendation 10+.",
        "For dancers with basic experience.");
    int contempBegId = addGroupRecord("Contemporary Beginner", anastasiaId, 15,
        "Contemporary beginners, age recommendation 10+.",
        "Suitable for beginners.");
    int contempIntId = addGroupRecord("Contemporary Intermediate", anastasiaId, 13,
        "Contemporary intermediate, age recommendation 10+.",
        "Intermediate level.");
    int contempAdultsId = addGroupRecord("Contemporary Adults", anastasiaId, 12,
        "Contemporary adults, age recommendation 25+.",
        "Adult evening group.");
    int contempTechniqueId = addGroupRecord("Contemporary Technique", anastasiaId, 12,
        "Technique-based contemporary training.",
        "Focus on technique.");
    int heelsBegId = addGroupRecord("Heels Beginner", kamilaId, 14,
        "Heels beginner, age recommendation 16+.",
        "Basic level.");
    int heelsAdvId = addGroupRecord("Heels Advanced", mariaId, 12,
        "Heels advanced, age recommendation 16+.",
        "Advanced level.");
    int showGroupId = addGroupRecord("Show Group", mariaId, 10,
        "Studio main show group, selection required.",
        "Selected group.");
    int stretchId = addGroupRecord("Stretching & Acrobatics", elizavetaId, 16,
        "Stretching and acrobatics class.",
        "Flexibility and acrobatics.");
    int softStretchId = addGroupRecord("Soft Stretching", elizavetaId, 14,
        "Soft stretching class.",
        "Gentle flexibility work.");
    int ladyStyleId = addGroupRecord("Lady Style Intermediate", mariaId, 12,
        "Lady style adult group, age recommendation 25+.",
        "Adult group.");

    assignUserToGroup(dancerId, jazzFunkBegId);
    assignUserToGroup(dancerId, stretchId);

    // MONDAY
    addSessionRecord(contempTechniqueId, 1, 1800, 60, "Regular class", true);
    addSessionRecord(heelsBegId, 1, 1900, 60, "Regular class", true);
    addSessionRecord(showGroupId, 1, 2000, 90, "Regular class", true);

    // TUESDAY
    addSessionRecord(jazzOpenId, 2, 1800, 60, "Regular class", true);
    addSessionRecord(ladyStyleId, 2, 1900, 60, "Regular class", true);
    addSessionRecord(heelsAdvId, 2, 2000, 60, "Regular class", true);

    // WEDNESDAY
    addSessionRecord(contempBegId, 3, 1700, 60, "Regular class", true);
    addSessionRecord(contempIntId, 3, 1800, 60, "Regular class", true);
    addSessionRecord(contempAdultsId, 3, 1930, 60, "Regular class", true);
    addSessionRecord(showGroupId, 3, 2000, 90, "Regular class", true);

    // THURSDAY
    addSessionRecord(jazzFunkBegId, 4, 1700, 60, "Regular class", true);
    addSessionRecord(jazzFunkIntId, 4, 1800, 60, "Regular class", true);
    addSessionRecord(ladyStyleId, 4, 1900, 60, "Regular class", true);
    addSessionRecord(heelsAdvId, 4, 2000, 60, "Regular class", true);

    // FRIDAY
    addSessionRecord(stretchId, 5, 1700, 60, "Regular class", true);
    addSessionRecord(showGroupId, 5, 1800, 60, "Regular class", true);
    addSessionRecord(jazzOpenId, 5, 1900, 60, "Regular class", true);
    addSessionRecord(heelsBegId, 5, 2000, 60, "Regular class", true);

    // SATURDAY
    addSessionRecord(softStretchId, 6, 1000, 60, "Regular class", true);
    addSessionRecord(stretchId, 6, 1100, 60, "Regular class", true);
    addSessionRecord(jazzFunkBegId, 6, 1200, 60, "Regular class", true);
    addSessionRecord(jazzFunkIntId, 6, 1300, 60, "Regular class", true);

    // SUNDAY
    addSessionRecord(contempBegId, 7, 1100, 60, "Regular class", true);
    addSessionRecord(contempIntId, 7, 1200, 60, "Regular class", true);

    int concertId = addEventRecord(EVENT_CONCERT, "Reporting Concert", 20260530, 1800,
        "Main end-of-season show.");
    int competitionId = addEventRecord(EVENT_COMPETITION, "Autumn Dance Cup", 20251115, 1200,
        "Competition for selected participants.");
    int generalId = addEventRecord(EVENT_GENERAL, "Studio Open Day", 20250901, 1700,
        "Welcome event for new season.");

    assignUserToEvent(dancerId, concertId);
    assignUserToEvent(dancerId, competitionId);
    assignUserToEvent(dancerId, generalId);

    Membership m;
    m.id = nextMembershipId();
    m.userId = dancerId;
    m.type = PASS_8;
    m.purchaseDate = 20250901;
    m.validUntil = membershipValidUntil(m.purchaseDate, m.type);
    m.classesTotal = membershipClassesTotal(m.type);
    m.classesUsed = 0;
    m.price = membershipPrice(m.type);
    m.active = true;
    copyText(m.paymentLink, MAX_LINK, paymentLink);
    memberships.push_back(m);

    if (studioInfo.coachInstagramCount == 0 && studioInfo.competitionPhotoCount == 0 && studioInfo.extraInfoCount == 0) {
        copyText(studioInfo.studioDescription, MAX_TEXT, "MS Dance Group Riga is a dance studio with evening groups, performances, competitions, and memberships.");
        copyText(studioInfo.rules, MAX_TEXT, "Absence must be submitted at least 3 hours before class. Coaches can cancel or reschedule their own sessions. Memberships are activated by admin after payment.");
        copyText(studioInfo.studioInstagram, MAX_LINK, "https://instagram.com/msdancegroup.riga");
        copyText(studioInfo.studioTikTok, MAX_LINK, "https://tiktok.com/@msdancegroup.riga");

        studioInfo.coachInstagramCount = 0;
        studioInfo.competitionPhotoCount = 0;
        studioInfo.extraInfoCount = 0;
    }

    saveAll();
}

int DanceStudioApp::addUserRecord(const string& name, const string& phone, const string& password, int role, const string& instagram) {
    User u;
    u.id = nextUserId();
    copyText(u.name, MAX_NAME, name);
    copyText(u.phone, MAX_PHONE, phone);
    copyText(u.password, MAX_PASSWORD, password);
    u.role = role;
    u.status = 1;
    copyText(u.instagram, MAX_LINK, instagram);
    u.groupCount = 0;
    for (int i = 0; i < MAX_GROUPS_PER_USER; ++i) u.groupIds[i] = 0;
    users.push_back(u);
    return u.id;
}

int DanceStudioApp::addGroupRecord(const string& name, int coachUserId, int capacity,
    const string& description, const string& rules) {
    Group g;
    g.id = nextGroupId();
    copyText(g.name, MAX_NAME, name);
    g.coachUserId = coachUserId;
    g.capacity = capacity;
    copyText(g.description, MAX_TEXT, description);
    copyText(g.rules, MAX_TEXT, rules);
    groups.push_back(g);
    return g.id;
}

int DanceStudioApp::addSessionRecord(int groupId, int dayOfWeek, int time, int durationMin, const string& note, bool recurring) {
    Session s;
    s.id = nextSessionId();
    s.groupId = groupId;
    s.date = 0; // no specific date for recurring sessions
    s.dayOfWeek = dayOfWeek;
    s.time = time;
    s.durationMin = durationMin;
    s.recurring = recurring;
    s.cancelled = false;
    copyText(s.note, MAX_TEXT, note);
    sessions.push_back(s);
    return s.id;
}

int DanceStudioApp::addEventRecord(int type, const string& title, int date, int time, const string& note) {
    Event e;
    e.id = nextEventId();
    e.type = type;
    copyText(e.title, MAX_NAME, title);
    e.date = date;
    e.time = time;
    copyText(e.note, MAX_TEXT, note);
    e.participantCount = 0;
    for (int i = 0; i < MAX_PARTICIPANTS; ++i) e.participantIds[i] = 0;
    events.push_back(e);
    return e.id;
}

bool DanceStudioApp::assignUserToGroup(int userId, int groupId) {
    int ui = findUserIndexById(userId);
    if (ui == -1) return false;
    if (userInGroup(userId, groupId)) return true;
    if (users[ui].groupCount >= MAX_GROUPS_PER_USER) return false;
    users[ui].groupIds[users[ui].groupCount++] = groupId;
    return true;
}

bool DanceStudioApp::assignUserToEvent(int userId, int eventId) {
    for (size_t i = 0; i < events.size(); ++i) {
        if (events[i].id == eventId) {
            if (eventAssignedToUser(events[i], userId)) return true;
            if (events[i].participantCount >= MAX_PARTICIPANTS) return false;
            events[i].participantIds[events[i].participantCount++] = userId;
            return true;
        }
    }
    return false;
}
