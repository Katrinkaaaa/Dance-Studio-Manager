#include <algorithm>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

const char* USERS_FILE = "users.dat";
const char* GROUPS_FILE = "groups.dat";
const char* SESSIONS_FILE = "sessions.dat";
const char* MEMBERSHIPS_FILE = "memberships.dat";
const char* EVENTS_FILE = "events.dat";
const char* ABSENCES_FILE = "absences.dat";
const char* STUDIO_INFO_FILE = "studio_info.dat";
const char* JOIN_REQUESTS_FILE = "join_requests.dat";

const int MAX_NAME = 64;
const int MAX_PHONE = 100;
const int MAX_PASSWORD = 32;
const int MAX_LINK = 160;
const int MAX_TEXT = 220;
const int MAX_GROUPS_PER_USER = 12;
const int MAX_PARTICIPANTS = 80;
const int MAX_INFO_ITEMS = 20;
const int MAX_REQUEST_GROUPS = 10;

enum Role {
    ROLE_DANCER = 1,
    ROLE_COACH = 2,
    ROLE_ADMIN = 3
};

enum MembershipType {
    PASS_SINGLE = 1,
    PASS_4 = 2,
    PASS_8 = 3,
    PASS_12 = 4,
    PASS_16 = 5,
    PASS_UNLIMITED = 6
};

enum EventType {
    EVENT_COMPETITION = 1,
    EVENT_CONCERT = 2,
    EVENT_GENERAL = 3
};

struct User {
    int id;
    char name[MAX_NAME];
    char phone[MAX_PHONE];
    char password[MAX_PASSWORD];
    int role;
    int status;
    char instagram[MAX_LINK];
    int groupIds[MAX_GROUPS_PER_USER];
    int groupCount;
};

struct Group {
    int id;
    char name[MAX_NAME];
    int coachUserId;
    int capacity;
    char description[MAX_TEXT];
    char rules[MAX_TEXT];
};

struct Session {
    int id;
    int groupId;
    int date;          // YYYYMMDD (optional; 0 when using recurring weekly schedule)
    int dayOfWeek;     // 1 Monday ... 7 Sunday
    int time;          // HHMM
    int durationMin;
    bool recurring;    // true for weekly schedule
    bool cancelled;
    char note[MAX_TEXT];
};

struct Membership {
    int id;
    int userId;
    int type;
    int purchaseDate;    // YYYYMMDD
    int validUntil;      // YYYYMMDD
    int classesTotal;    // 0 = unlimited
    int classesUsed;
    double price;
    bool active;
    char paymentLink[MAX_LINK];
};

struct Event {
    int id;
    int type;
    char title[MAX_NAME];
    int date;            // YYYYMMDD
    int time;            // HHMM
    char note[MAX_TEXT];
    int participantIds[MAX_PARTICIPANTS];
    int participantCount;
};

struct Absence {
    int id;
    int userId;
    int sessionId;
    char reason[MAX_TEXT];
    long long submittedDateTime;   // YYYYMMDDHHMM
    bool late;
    bool deducted;
};

struct StudioInfo {
    char studioDescription[MAX_TEXT];
    char rules[MAX_TEXT];
    char studioInstagram[MAX_LINK];
    char studioTikTok[MAX_LINK];

    char contemporaryInfo[MAX_TEXT];
    char heelsInfo[MAX_TEXT];
    char jazzFunkInfo[MAX_TEXT];
    char jazzOpenInfo[MAX_TEXT];
    char ladyStyleInfo[MAX_TEXT];
    char showGroupInfo[MAX_TEXT];
    char stretchingInfo[MAX_TEXT];
    char softStretchingInfo[MAX_TEXT];

    char coachInstagramLinks[MAX_INFO_ITEMS][MAX_LINK];
    char competitionPhotoLinks[MAX_INFO_ITEMS][MAX_LINK];
    char extraInfoLinks[MAX_INFO_ITEMS][MAX_LINK];
    int coachInstagramCount;
    int competitionPhotoCount;
    int extraInfoCount;
};

struct JoinRequest {
    int id;
    char firstName[MAX_NAME];
    char lastName[MAX_NAME];
    int age;
    char experience[MAX_TEXT];
    char parentPhone[MAX_PHONE];
    char phone[MAX_PHONE];
    char password[MAX_PASSWORD];
    int groupIds[MAX_REQUEST_GROUPS];
    int groupCount;
    bool processed;
};

class DanceStudioApp {
private:
    vector<User> users;
    vector<Group> groups;
    vector<Session> sessions;
    vector<Membership> memberships;
    vector<Event> events;
    vector<Absence> absences;
    vector<JoinRequest> joinRequests;

    StudioInfo studioInfo{};

    const string studioInstagram = "https://instagram.com/msdancegroup.riga";
    const string studioTikTok = "https://tiktok.com/@msdancegroup.riga";
    const string paymentLink = "https://internetbank.example.com/studio-payment";

public:
    void run() {
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

private:
    // ---------- utility ----------
    static void copyText(char* destination, int size, const string& source) {
        strncpy_s(destination, size, source.c_str(), size - 1);
        destination[size - 1] = '\0';
    }

    static bool isValidPhone(const string& phone) {
        if (phone.size() != 8) return false;
        for (size_t i = 0; i < phone.size(); ++i) {
            if (phone[i] < '0' || phone[i] > '9') return false;
        }
        return true;
    }

    static bool isValidPassword(const string& password) {
        if (password.size() < 8) return false;
        for (size_t i = 0; i < password.size(); ++i) {
            bool digit = (password[i] >= '0' && password[i] <= '9');
            bool lower = (password[i] >= 'a' && password[i] <= 'z');
            bool upper = (password[i] >= 'A' && password[i] <= 'Z');
            if (!(digit || lower || upper)) return false;
        }
        return true;
    }

    static string toLowerText(string value) {
        for (size_t i = 0; i < value.size(); ++i) {
            if (value[i] >= 'A' && value[i] <= 'Z') {
                value[i] = static_cast<char>(value[i] - 'A' + 'a');
            }
        }
        return value;
    }

    static int readInt() {
        int value;
        while (!(cin >> value)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter an integer: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }
    static double readDouble() {
        double value;
        while (!(cin >> value)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a number: ";
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return value;
    }

    static string readLine(const string& prompt) {
        cout << prompt;
        string value;
        getline(cin, value);
        return value;
    }

    static bool readYesNo(const string& prompt) {
        while (true) {
            string value = toLowerText(readLine(prompt + " (y/n): "));
            if (value == "y" || value == "yes") return true;
            if (value == "n" || value == "no") return false;
            cout << "Please enter y or n.\n";
        }
    }

    static int getCurrentDate() {
        time_t now = time(0);
        tm localNow;
        localtime_s(&localNow, &now);

        return (localNow.tm_year + 1900) * 10000
            + (localNow.tm_mon + 1) * 100
            + localNow.tm_mday;
    }

    static int getCurrentTimeHHMM() {
        time_t now = time(0);
        tm localNow;
        localtime_s(&localNow, &now);

        return localNow.tm_hour * 100 + localNow.tm_min;
    }

    static time_t toTimeValue(int date, int time) {
        int year = date / 10000;
        int month = (date / 100) % 100;
        int day = date % 100;
        int hour = time / 100;
        int minute = time % 100;

        tm tmValue = {};
        tmValue.tm_year = year - 1900;
        tmValue.tm_mon = month - 1;
        tmValue.tm_mday = day;
        tmValue.tm_hour = hour;
        tmValue.tm_min = minute;
        tmValue.tm_sec = 0;
        tmValue.tm_isdst = -1;
        return mktime(&tmValue);
    }

    static time_t toTimeValueFromDateTime(long long dt) {
        int minute = static_cast<int>(dt % 100);
        dt /= 100;
        int hour = static_cast<int>(dt % 100);
        dt /= 100;
        int day = static_cast<int>(dt % 100);
        dt /= 100;
        int month = static_cast<int>(dt % 100);
        dt /= 100;
        int year = static_cast<int>(dt);

        tm tmValue = {};
        tmValue.tm_year = year - 1900;
        tmValue.tm_mon = month - 1;
        tmValue.tm_mday = day;
        tmValue.tm_hour = hour;
        tmValue.tm_min = minute;
        tmValue.tm_sec = 0;
        tmValue.tm_isdst = -1;
        return mktime(&tmValue);
    }

    static long long buildDateTimeNumber(int date, int time) {
        return static_cast<long long>(date) * 10000LL + static_cast<long long>(time);
    }
    static int addDaysSimple(int date, int days) {
        time_t base = toTimeValue(date, 0);
        base += static_cast<time_t>(days) * 24 * 60 * 60;
        tm tmValue;
        localtime_s(&tmValue, &base);
        return (tmValue.tm_year + 1900) * 10000 + (tmValue.tm_mon + 1) * 100 + tmValue.tm_mday;
    }

    static const char* dayOfWeekToString(int day) {
        switch (day) {
        case 1: return "Monday";
        case 2: return "Tuesday";
        case 3: return "Wednesday";
        case 4: return "Thursday";
        case 5: return "Friday";
        case 6: return "Saturday";
        case 7: return "Sunday";
        default: return "Unknown";
        }
    }

    static int nextMonthFirstDay(int date) {
        int year = date / 10000;
        int month = (date / 100) % 100;
        if (month == 12) {
            year++;
            month = 1;
        }
        else {
            month++;
        }
        return year * 10000 + month * 100 + 1;
    }

    static const char* roleToString(int role) {
        if (role == ROLE_DANCER) return "Dancer";
        if (role == ROLE_COACH) return "Coach";
        return "Admin";
    }

    static const char* membershipTypeToString(int type) {
        switch (type) {
        case PASS_SINGLE: return "Single Visit";
        case PASS_4: return "4 Classes";
        case PASS_8: return "8 Classes";
        case PASS_12: return "12 Classes";
        case PASS_16: return "16 Classes";
        case PASS_UNLIMITED: return "Unlimited";
        default: return "Unknown";
        }
    }

    static const char* eventTypeToString(int type) {
        switch (type) {
        case EVENT_COMPETITION: return "Competition";
        case EVENT_CONCERT: return "Concert";
        case EVENT_GENERAL: return "General Event";
        default: return "Unknown";
        }
    }

    static double membershipPrice(int type) {
        switch (type) {
        case PASS_SINGLE: return 15.0;
        case PASS_4: return 50.0;
        case PASS_8: return 80.0;
        case PASS_12: return 90.0;
        case PASS_16: return 110.0;
        case PASS_UNLIMITED: return 120.0;
        default: return 0.0;
        }
    }
    static int membershipClassesTotal(int type) {
        switch (type) {
        case PASS_SINGLE: return 1;
        case PASS_4: return 4;
        case PASS_8: return 8;
        case PASS_12: return 12;
        case PASS_16: return 16;
        case PASS_UNLIMITED: return 0;
        default: return 0;
        }
    }

    static int membershipValidUntil(int purchaseDate, int type) {
        if (type == PASS_UNLIMITED) return nextMonthFirstDay(purchaseDate);
        return addDaysSimple(purchaseDate, 35);
    }

    static bool compareSessionDateTime(const Session& a, const Session& b) {
        if (a.date != b.date) return a.date < b.date;
        return a.time < b.time;
    }

    static bool compareEventDateTime(const Event& a, const Event& b) {
        if (a.date != b.date) return a.date < b.date;
        return a.time < b.time;
    }

    // ---------- ids ----------
    int nextUserId() const { return maxIdUsers() + 1; }
    int nextGroupId() const { return maxIdGroups() + 1; }
    int nextSessionId() const { return maxIdSessions() + 1; }
    int nextMembershipId() const { return maxIdMemberships() + 1; }
    int nextEventId() const { return maxIdEvents() + 1; }
    int nextAbsenceId() const { return maxIdAbsences() + 1; }

    int nextJoinRequestId() const { return maxIdJoinRequests() + 1; }

    int maxIdJoinRequests() const {
        int m = 0;
        for (size_t i = 0; i < joinRequests.size(); ++i) {
            if (joinRequests[i].id > m) m = joinRequests[i].id;
        }
        return m;
    }

    int maxIdUsers() const {
        int m = 0;
        for (size_t i = 0; i < users.size(); ++i) if (users[i].id > m) m = users[i].id;
        return m;
    }

    int maxIdGroups() const {
        int m = 0;
        for (size_t i = 0; i < groups.size(); ++i) if (groups[i].id > m) m = groups[i].id;
        return m;
    }

    int maxIdSessions() const {
        int m = 0;
        for (size_t i = 0; i < sessions.size(); ++i) if (sessions[i].id > m) m = sessions[i].id;
        return m;
    }

    int maxIdMemberships() const {
        int m = 0;
        for (size_t i = 0; i < memberships.size(); ++i) if (memberships[i].id > m) m = memberships[i].id;
        return m;
    }

    int maxIdEvents() const {
        int m = 0;
        for (size_t i = 0; i < events.size(); ++i) if (events[i].id > m) m = events[i].id;
        return m;
    }

    int maxIdAbsences() const {
        int m = 0;
        for (size_t i = 0; i < absences.size(); ++i) if (absences[i].id > m) m = absences[i].id;
        return m;
    }

    // ---------- persistence ----------
    void loadAll() {
        loadUsers();
        loadGroups();
        loadSessions();
        loadMemberships();
        loadEvents();
        loadAbsences();
        loadStudioInfo();
        loadJoinRequests();
    }

    void saveAll() {
        saveUsers();
        saveGroups();
        saveSessions();
        saveMemberships();
        saveEvents();
        saveAbsences();
        saveStudioInfo();
        saveJoinRequests();
    }

    void loadUsers() {
        users.clear();
        ifstream file(USERS_FILE, ios::binary);
        if (!file.is_open()) return;
        User item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(User))) users.push_back(item);
        file.close();
    }

    void saveUsers() {
        ofstream file(USERS_FILE, ios::binary | ios::trunc);
        for (size_t i = 0; i < users.size(); ++i) {
            file.write(reinterpret_cast<const char*>(&users[i]), sizeof(User));
        }
        file.close();
    }

    void loadGroups() {
        groups.clear();
        ifstream file(GROUPS_FILE, ios::binary);
        if (!file.is_open()) return;
        Group item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(Group))) groups.push_back(item);
        file.close();
    }

    void saveGroups() {
        ofstream file(GROUPS_FILE, ios::binary | ios::trunc);
        for (size_t i = 0; i < groups.size(); ++i) {
            file.write(reinterpret_cast<const char*>(&groups[i]), sizeof(Group));
        }
        file.close();
    }

    void loadSessions() {
        sessions.clear();
        ifstream file(SESSIONS_FILE, ios::binary);
        if (!file.is_open()) return;
        Session item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(Session))) sessions.push_back(item);
        file.close();
    }
    void saveSessions() {
        ofstream file(SESSIONS_FILE, ios::binary | ios::trunc);
        for (size_t i = 0; i < sessions.size(); ++i) {
            file.write(reinterpret_cast<const char*>(&sessions[i]), sizeof(Session));
        }
        file.close();
    }

    void loadMemberships() {
        memberships.clear();
        ifstream file(MEMBERSHIPS_FILE, ios::binary);
        if (!file.is_open()) return;
        Membership item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(Membership))) memberships.push_back(item);
        file.close();
    }

    void saveMemberships() {
        ofstream file(MEMBERSHIPS_FILE, ios::binary | ios::trunc);
        for (size_t i = 0; i < memberships.size(); ++i) {
            file.write(reinterpret_cast<const char*>(&memberships[i]), sizeof(Membership));
        }
        file.close();
    }

    void loadEvents() {
        events.clear();
        ifstream file(EVENTS_FILE, ios::binary);
        if (!file.is_open()) return;
        Event item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(Event))) events.push_back(item);
        file.close();
    }

    void saveEvents() {
        ofstream file(EVENTS_FILE, ios::binary | ios::trunc);
        for (size_t i = 0; i < events.size(); ++i) {
            file.write(reinterpret_cast<const char*>(&events[i]), sizeof(Event));
        }
        file.close();
    }

    void loadAbsences() {
        absences.clear();
        ifstream file(ABSENCES_FILE, ios::binary);
        if (!file.is_open()) return;
        Absence item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(Absence))) absences.push_back(item);
        file.close();
    }

    void saveAbsences() {
        ofstream file(ABSENCES_FILE, ios::binary | ios::trunc);
        for (size_t i = 0; i < absences.size(); ++i) {
            file.write(reinterpret_cast<const char*>(&absences[i]), sizeof(Absence));
        }
        file.close();
    }

    void loadStudioInfo() {
        ifstream file(STUDIO_INFO_FILE, ios::binary);
        if (!file.is_open()) return;
        file.read(reinterpret_cast<char*>(&studioInfo), sizeof(StudioInfo));
        file.close();
    }

    void saveStudioInfo() {
        ofstream file(STUDIO_INFO_FILE, ios::binary | ios::trunc);
        file.write(reinterpret_cast<const char*>(&studioInfo), sizeof(StudioInfo));
        file.close();
    }

    // ---------- find helpers ----------
    int findUserIndexById(int id) const {
        for (size_t i = 0; i < users.size(); ++i) if (users[i].id == id) return static_cast<int>(i);
        return -1;
    }

    int findUserIdByPhone(const string& phone) const {
        for (size_t i = 0; i < users.size(); ++i)
            if (phone == users[i].phone) return users[i].id;
        return -1;
    }

    int findGroupIndexById(int id) const {
        for (size_t i = 0; i < groups.size(); ++i) if (groups[i].id == id) return static_cast<int>(i);
        return -1;
    }

    int findSessionIndexById(int id) const {
        for (size_t i = 0; i < sessions.size(); ++i) if (sessions[i].id == id) return static_cast<int>(i);
        return -1;
    }

    int findMembershipIndexById(int id) const {
        for (size_t i = 0; i < memberships.size(); ++i) if (memberships[i].id == id) return static_cast<int>(i);
        return -1;
    }

    int findAbsenceIndexByUserAndSession(int userId, int sessionId) const {
        for (size_t i = 0; i < absences.size(); ++i) {
            if (absences[i].userId == userId && absences[i].sessionId == sessionId) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool userInGroup(int userId, int groupId) const {
        int userIndex = findUserIndexById(userId);
        if (userIndex == -1) return false;
        for (int i = 0; i < users[userIndex].groupCount; ++i) {
            if (users[userIndex].groupIds[i] == groupId) return true;
        }
        return false;
    }

    bool eventAssignedToUser(const Event& eventItem, int userId) const {
        for (int i = 0; i < eventItem.participantCount; ++i) {
            if (eventItem.participantIds[i] == userId) return true;
        }
        return false;
    }

    bool coachOwnsGroup(int coachUserId, int groupId) const {
        int gi = findGroupIndexById(groupId);
        if (gi == -1) return false;
        return groups[gi].coachUserId == coachUserId;
    }
    bool coachOwnsSession(int coachUserId, int sessionId) const {
        int si = findSessionIndexById(sessionId);
        if (si == -1) return false;
        return coachOwnsGroup(coachUserId, sessions[si].groupId);
    }

    // ---------- seed ----------
    void seedDataIfNeeded() {
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

    int addUserRecord(const string& name, const string& phone, const string& password, int role, const string& instagram) {
        User u;
        u.id = nextUserId();
        copyText(u.name, MAX_NAME, name);
        copyText(u.phone, MAX_PHONE, phone);
        copyText(u.password, MAX_PASSWORD, password);
        u.role = role;
        copyText(u.instagram, MAX_LINK, instagram);
        u.groupCount = 0;
        for (int i = 0; i < MAX_GROUPS_PER_USER; ++i) u.groupIds[i] = 0;
        users.push_back(u);
        return u.id;
    }

    int addGroupRecord(const string& name, int coachUserId, int capacity,
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

    int addSessionRecord(int groupId, int dayOfWeek, int time, int durationMin, const string& note, bool recurring = true) {
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

    int addEventRecord(int type, const string& title, int date, int time, const string& note) {
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

    bool assignUserToGroup(int userId, int groupId) {
        int ui = findUserIndexById(userId);
        if (ui == -1) return false;
        if (userInGroup(userId, groupId)) return true;
        if (users[ui].groupCount >= MAX_GROUPS_PER_USER) return false;
        users[ui].groupIds[users[ui].groupCount++] = groupId;
        return true;
    }

    bool assignUserToEvent(int userId, int eventId) {
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

    // ---------- login ----------
    int login() {
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

    // ---------- registration ----------
    void registerDancer() {
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

    void loadJoinRequests() {
        joinRequests.clear();
        ifstream file(JOIN_REQUESTS_FILE, ios::binary);
        if (!file.is_open()) return;

        JoinRequest item;
        while (file.read(reinterpret_cast<char*>(&item), sizeof(JoinRequest))) {
            joinRequests.push_back(item);
        }
        file.close();
    }

    void saveJoinRequests() {
        ofstream file(JOIN_REQUESTS_FILE, ios::binary | ios::trunc);
        for (size_t i = 0; i < joinRequests.size(); ++i) {
            file.write(reinterpret_cast<const char*>(&joinRequests[i]), sizeof(JoinRequest));
        }
        file.close();
    }

    // ---------- menus ----------
    void dancerMenu(int userIndex) {
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

    void coachMenu(int userIndex) {
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

    void adminMenu(int userIndex) {
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

    void adminUsersMenu() {
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

    void adminGroupsMenu() {
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

    void adminSessionsEventsMenu() {
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

    void adminMembershipsMenu() {
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


    void adminStudioInfoMenu() {
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

    void adminCoachMenu(int adminUserId) {
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

    // ---------- display ----------
    void showAllUsers() const {
        if (users.empty()) {
            cout << "No users found.\n";
            return;
        }

        cout << left
            << setw(5) << "ID"
            << setw(22) << "Name"
            << setw(20) << "Phone"
            << setw(12) << "Role"
            << setw(12) << "Status"
            << "Instagram\n";

        cout << string(85, '-') << "\n";

        for (size_t i = 0; i < users.size(); ++i) {
            string statusStr;

            if (users[i].status == 0) statusStr = "Pending";
            else if (users[i].status == 1) statusStr = "Approved";
            else if (users[i].status == 2) statusStr = "Rejected";

            cout << left
                << setw(5) << users[i].id
                << setw(22) << users[i].name
                << setw(20) << users[i].phone
                << setw(12) << roleToString(users[i].role)
                << setw(12) << statusStr
                << users[i].instagram
                << "\n";
        }
    }

    void showAllSessions() {
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

    void showUserGroups(int userId) const {
        int ui = findUserIndexById(userId);
        if (ui == -1) return;

        if (users[ui].groupCount == 0) {
            cout << "No groups assigned.\n";
            return;
        }

        cout << "\n=== MY GROUPS ===\n";
        for (int i = 0; i < users[ui].groupCount; ++i) {
            int gi = findGroupIndexById(users[ui].groupIds[i]);
            if (gi == -1) continue;

            int coachIndex = findUserIndexById(groups[gi].coachUserId);
            cout << "Group ID: " << groups[gi].id << "\n";
            cout << "Name: " << groups[gi].name << "\n";
            cout << "Coach: " << (coachIndex == -1 ? "Unknown" : users[coachIndex].name) << "\n";
            cout << "Capacity: " << groups[gi].capacity << "\n";
            cout << "Description: " << groups[gi].description << "\n";
            cout << "Rules: " << groups[gi].rules << "\n";
            cout << "-----------------------------\n";
        }
    }

    void showCoachSessions(int coachUserId) {
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

    void showCalendarForUser(int userId, bool onlyMine, const string& styleFilter) {
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

    void calendarFilterMenu(int userId) {
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

    void showInfoSection() const {
        cout << "\n=== STUDIO INFO ===\n";
        cout << "Studio description:\n" << studioInfo.studioDescription << "\n\n";
        cout << "Rules:\n" << studioInfo.rules << "\n\n";
        cout << "Studio Instagram: " << studioInfo.studioInstagram << "\n";
        cout << "Studio TikTok: " << studioInfo.studioTikTok << "\n\n";

        cout << "=== STYLES ===\n";
        cout << "- " << studioInfo.contemporaryInfo << "\n";
        cout << "- " << studioInfo.heelsInfo << "\n";
        cout << "- " << studioInfo.jazzFunkInfo << "\n";
        cout << "- " << studioInfo.jazzOpenInfo << "\n";
        cout << "- " << studioInfo.ladyStyleInfo << "\n";
        cout << "- " << studioInfo.showGroupInfo << "\n";
        cout << "- " << studioInfo.stretchingInfo << "\n";
        cout << "- " << studioInfo.softStretchingInfo << "\n\n";

        cout << "Coach and admin Instagram links:\n";
        bool foundLinks = false;
        for (size_t i = 0; i < users.size(); ++i) {
            if ((users[i].role == ROLE_COACH || users[i].role == ROLE_ADMIN) &&
                strlen(users[i].instagram) > 0) {
                foundLinks = true;
                cout << "- " << users[i].name << ": " << users[i].instagram << "\n";
            }
        }
        if (!foundLinks) {
            cout << "No coach or admin Instagram links found.\n";
        }

        cout << "\nCompetition photo links:\n";
        if (studioInfo.competitionPhotoCount == 0) {
            cout << "No competition photo links added.\n";
        }
        else {
            for (int i = 0; i < studioInfo.competitionPhotoCount; ++i) {
                cout << i + 1 << ". " << studioInfo.competitionPhotoLinks[i] << "\n";
            }
        }

        cout << "\nExtra info links:\n";
        if (studioInfo.extraInfoCount == 0) {
            cout << "No extra info links added.\n";
        }
        else {
            for (int i = 0; i < studioInfo.extraInfoCount; ++i) {
                cout << i + 1 << ". " << studioInfo.extraInfoLinks[i] << "\n";
            }
        }
    }

    void showLevelDescriptions() const {
        cout << "\n=== GROUP LEVELS ===\n";
        cout << "OPEN - open groups for dancers of any level.\n";
        cout << "BEGINNER (BEG) - for beginner dancers (0-2 years of experience).\n";
        cout << "INTERMEDIATE (INT) - for dancers with experience (2+ years of experience).\n";
        cout << "ADVANCED (ADV) - closed groups for stronger and more experienced dancers.\n";
        cout << "SHOW GROUP - closed group for dancers selected by the studio.\n";
        cout << "----------------------------------------\n";
    }

    void showFullStylesInfo() {
        cout << "\n========== STYLES ==========\n";

        // CONTEMP
        cout << "\n--- CONTEMPORARY (12+) ---\n";
        cout << "Flow, softness, control.\n";
        cout << "Balance between freedom and discipline.\n";

        // HEELS
        cout << "\n--- HEELS (16+) ---\n";
        cout << "Confidence, femininity, control.\n";
        cout << "Balance, posture, presentation.\n";

        // JAZZ FUNK
        cout << "\n--- JAZZ FUNK (12+) ---\n";
        cout << "Energy + precision.\n";
        cout << "Sharp, musical, expressive.\n";

        // JAZZ
        cout << "\n--- JAZZ (14+) ---\n";
        cout << "Freedom inside structure.\n";
        cout << "Technique, jumps, turns, musicality.\n";

        // LADY STYLE
        cout << "\n--- LADY STYLE (30+) ---\n";
        cout << "Mix of styles with feminine energy.\n";
        cout << "Expression, сценичность.\n";

        // SHOW GROUP
        cout << "\n--- SHOW GROUP (15+) ---\n";
        cout << "High level, competitions, performances.\n";
        cout << "Only after casting.\n";

        cout << "\n";
    }

    void showAvailableGroupsForRequests() const {
        cout << "\n========== AVAILABLE GROUPS ==========\n";
        cout << "1. Contemporary Beginner (12+)\n";
        cout << "2. Contemporary Intermediate (12+)\n";
        cout << "3. Contemporary Adults (25+)\n";
        cout << "4. Heels Beginner (16+)\n";
        cout << "5. Heels Advanced (16+)\n";
        cout << "6. Jazz Funk Beginner (12+)\n";
        cout << "7. Jazz Funk Intermediate (12+)\n";
        cout << "8. Jazz Open (14+)\n";
        cout << "9. Lady Style Intermediate (30+)\n";
        cout << "10. Show Group (15+, casting)\n";
        cout << "11. Stretching & Acrobatics\n";
        cout << "12. Soft Stretching\n";
        cout << "\n";
    }

    void submitJoinRequest(int userIndex) {
        cout << "\n=== JOIN REQUEST ===\n";

        // Описание уровней
        showLevelDescriptions();

        // Спросить, хочет ли посмотреть стили
        cout << "Do you want to view full style information? (y/n): ";
        char choice;
        cin >> choice;
        cin.ignore();

        if (choice == 'y' || choice == 'Y') {
            showFullStylesInfo();
        }

        // Показать группы
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

            if (groupId < 1 || groupId > groups.size()) {
                cout << "Invalid group ID.\n";
                --i;
                continue;
            }

            selectedGroups.push_back(groupId);
        }

        // creating a request
        JoinRequest req;
        req.id = static_cast<int>(joinRequests.size()) + 1;

        // users info
        strcpy_s(req.firstName, sizeof(req.firstName), users[userIndex].name);
        strcpy_s(req.lastName, sizeof(req.lastName), "");
        req.age = 0;

        strcpy_s(req.experience, sizeof(req.experience), "");
        strcpy_s(req.parentPhone, sizeof(req.parentPhone), users[userIndex].phone);

        req.groupCount = static_cast<int>(selectedGroups.size());

        for (int i = 0; i < req.groupCount; ++i) {
            req.groupIds[i] = selectedGroups[i];
        }

        req.processed = false;

        joinRequests.push_back(req);

        cout << "Request submitted successfully! Waiting for admin approval.\n";
    }

    void showWeeklyScheduleInfo() const {
        cout << "\n========== WEEKLY SCHEDULE ==========\n";

        cout << "\nMONDAY\n";
        cout << "18:00-19:00 | Contemporary Technique\n";
        cout << "19:00-20:00 | Heels Beginner (16+)\n";
        cout << "20:00-21:30 | Show Group (15+, casting)\n";

        cout << "\nTUESDAY\n";
        cout << "18:00-19:00 | Jazz Open (14+)\n";
        cout << "19:00-20:00 | Lady Style Intermediate (30+)\n";
        cout << "20:00-21:00 | Heels Advanced (16+)\n";

        cout << "\nWEDNESDAY\n";
        cout << "17:00-18:00 | Contemporary Beginner (12+)\n";
        cout << "18:00-19:00 | Contemporary Intermediate (12+)\n";
        cout << "19:30-20:30 | Contemporary Adults (25+)\n";
        cout << "20:00-21:30 | Show Group (15+, casting)\n";

        cout << "\nTHURSDAY\n";
        cout << "17:00-18:00 | Jazz Funk Beginner (12+)\n";
        cout << "18:00-19:00 | Jazz Funk Intermediate (12+)\n";
        cout << "19:00-20:00 | Lady Style Intermediate (30+)\n";
        cout << "20:00-21:00 | Heels Advanced (16+)\n";

        cout << "\nFRIDAY\n";
        cout << "17:00-18:00 | Stretching & Acrobatics\n";
        cout << "18:00-19:30 | Show Group (15+, casting)\n";
        cout << "19:00-20:00 | Jazz Open (14+)\n";
        cout << "20:00-21:00 | Heels Beginner (16+)\n";

        cout << "\nSATURDAY\n";
        cout << "10:00-11:00 | Soft Stretching\n";
        cout << "11:00-12:00 | Stretching & Acrobatics\n";
        cout << "12:00-13:00 | Jazz Funk Beginner (12+)\n";
        cout << "13:00-14:00 | Jazz Funk Intermediate (12+)\n";

        cout << "\nSUNDAY\n";
        cout << "11:00-12:00 | Contemporary Beginner (12+)\n";
        cout << "12:00-13:00 | Contemporary Intermediate (12+)\n";

        cout << "\n";
    }

    // ---------- membership ----------
    void membershipMenuForDancer(int userIndex) {
        while (true) {
            cout << "\n=== MEMBERSHIP SECTION ===\n";
            cout << "1. Show membership types\n";
            cout << "2. Show my memberships\n";
            cout << "3. Buy membership\n";
            cout << "0. Back\n";
            cout << "Choose: ";

            int choice = readInt();
            switch (choice) {
            case 1: showMembershipTypes(); break;
            case 2: showUserMemberships(users[userIndex].id); break;
            case 3: buyMembership(users[userIndex].id); break;
            case 0: return;
            default: cout << "Invalid choice.\n";
            }
        }
    }

    void showMembershipTypes() const {
        cout << "\n=== MEMBERSHIP TYPES ===\n";
        cout << "1. Single Visit - 15 EUR\n";
        cout << "2. 4 Classes - 50 EUR\n";
        cout << "3. 8 Classes - 80 EUR\n";
        cout << "4. 12 Classes - 90 EUR\n";
        cout << "5. 16 Classes - 110 EUR\n";
        cout << "6. Unlimited - 120 EUR\n\n";
        cout << "Rules:\n";
        cout << "- 4/8/12/16 memberships are valid for 5 weeks from purchase date.\n";
        cout << "- Unlimited membership is valid from the 1st day of the month to the 1st day of the next month.\n";
    }

    void showUserMemberships(int userId) const {
        bool found = false;
        cout << "\n=== MY MEMBERSHIPS ===\n";
        for (size_t i = 0; i < memberships.size(); ++i) {
            if (memberships[i].userId == userId) {
                found = true;
                cout << "Membership ID: " << memberships[i].id << "\n";
                cout << "Type: " << membershipTypeToString(memberships[i].type) << "\n";
                cout << "Price: " << fixed << setprecision(2) << memberships[i].price << " EUR\n";
                cout << "Purchase date: " << memberships[i].purchaseDate << "\n";
                cout << "Valid until: " << memberships[i].validUntil << "\n";
                if (memberships[i].classesTotal == 0) {
                    cout << "Remaining classes: Unlimited\n";
                }
                else {
                    cout << "Remaining classes: " << (memberships[i].classesTotal - memberships[i].classesUsed) << "\n";
                }
                cout << "Status: " << getMembershipStatusText(memberships[i]) << "\n";
                cout << "Payment link: " << memberships[i].paymentLink << "\n";
                cout << "-----------------------------\n";
            }
        }
        if (!found) cout << "No memberships found.\n";
    }

    void buyMembership(int userId) {
        for (size_t i = 0; i < memberships.size(); ++i) {
            if (memberships[i].userId == userId) {
                if (memberships[i].active || !memberships[i].active) {
                    int today = getCurrentDate();

                    bool notFinishedByDate = memberships[i].validUntil >= today;
                    bool notFinishedByClasses =
                        (memberships[i].classesTotal == 0) ||
                        (memberships[i].classesUsed < memberships[i].classesTotal);

                    if (notFinishedByDate && notFinishedByClasses) {
                        cout << "You already have an active or pending membership.\n";
                        return;
                    }
                }
            }
        }

        showMembershipTypes();
        cout << "Choose membership type: ";
        int type = readInt();

        if (type < PASS_SINGLE || type > PASS_UNLIMITED) {
            cout << "Invalid membership type.\n";
            return;
        }

        int purchaseDate = getCurrentDate();

        Membership m;
        m.id = nextMembershipId();
        m.userId = userId;
        m.type = type;
        m.purchaseDate = purchaseDate;
        m.validUntil = membershipValidUntil(purchaseDate, type);
        m.classesTotal = membershipClassesTotal(type);
        m.classesUsed = 0;
        m.price = membershipPrice(type);
        m.active = false;
        copyText(m.paymentLink, MAX_LINK, paymentLink);

        memberships.push_back(m);

        cout << "\nMembership request created.\n";
        cout << "Type: " << membershipTypeToString(type) << "\n";
        cout << "Price: " << fixed << setprecision(2) << m.price << " EUR\n";
        cout << "Payment link: " << m.paymentLink << "\n";
        cout << "After payment, the administrator will activate your membership.\n";
    }

    string getMembershipStatusText(const Membership& m) const {
        int today = getCurrentDate();

        if (!m.active) {
            return "Pending activation";
        }

        if (m.validUntil < today) {
            return "Expired";
        }

        if (m.classesTotal != 0 && m.classesUsed >= m.classesTotal) {
            return "Expired";
        }

        if (m.classesTotal == 0) {
            int oneDayBefore = addDaysSimple(m.validUntil, -1);
            if (today >= oneDayBefore && today <= m.validUntil) {
                return "Reminder: 1 day left";
            }
            return "Active";
        }

        int remaining = m.classesTotal - m.classesUsed;
        if (remaining <= 1) {
            return "Reminder: 1 class left";
        }

        return "Active";
    }

    Membership* findActiveMembershipForUser(int userId, int date) {
        for (size_t i = 0; i < memberships.size(); ++i) {
            if (memberships[i].userId != userId) continue;
            if (!memberships[i].active) continue;
            if (memberships[i].validUntil < date) continue;
            if (memberships[i].classesTotal != 0 && memberships[i].classesUsed >= memberships[i].classesTotal) continue;
            return &memberships[i];
        }
        return NULL;
    }

    void showAllMemberships() const {
        if (memberships.empty()) {
            cout << "No memberships found.\n";
            return;
        }

        cout << left
            << setw(5) << "ID"
            << setw(8) << "User"
            << setw(16) << "Type"
            << setw(12) << "Purchased"
            << setw(12) << "ValidUntil"
            << setw(8) << "Used"
            << setw(8) << "Total"
            << setw(10) << "Price"
            << setw(20) << "Status"
            << "\n";

        cout << string(105, '-') << "\n";

        for (size_t i = 0; i < memberships.size(); ++i) {
            cout << left
                << setw(5) << memberships[i].id
                << setw(8) << memberships[i].userId
                << setw(16) << membershipTypeToString(memberships[i].type)
                << setw(12) << memberships[i].purchaseDate
                << setw(12) << memberships[i].validUntil
                << setw(8) << memberships[i].classesUsed
                << setw(8) << memberships[i].classesTotal
                << setw(10) << fixed << setprecision(2) << memberships[i].price
                << setw(20) << getMembershipStatusText(memberships[i])
                << "\n";
        }
    }

    void activateMembershipInteractive() {
        showAllMemberships();
        cout << "Enter membership ID to activate: ";
        int id = readInt();

        int mi = findMembershipIndexById(id);
        if (mi == -1) {
            cout << "Membership not found.\n";
            return;
        }

        memberships[mi].active = true;
        cout << "Membership activated.\n";
    }

    // ---------- absences ----------
    void submitAbsence(int userIndex) {
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
            if (m != NULL && m->classesTotal != 0) {
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

    void showCoachAbsences(int coachUserId) {
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

    // ---------- coach actions ----------
    void cancelCoachSession(int coachUserId) {
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

    void rescheduleCoachSession(int coachUserId) {
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

    // ---------- admin actions ----------
    void showJoinRequests() {
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
        void addUserInteractive(int role) {
            string name = readLine("Name: ");
            string phone = readLine("Phone: ");
            string password = readLine("Password: ");
            string instagram = readLine("Instagram link (or leave empty): ");

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

            int id = addUserRecord(name, phone, password, role, instagram);
            cout << "User added with ID " << id << ".\n";
        }

        void addToEventMenu() {
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

        void addGroupToEventInteractive() {
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

        void addAllDancersToEventInteractive() {
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

        void removeUserFromEventInteractive() {
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

        void deleteUserInteractive() {
            showAllUsers();

            cout << "Enter user ID to delete (0 - cancel): ";
            int id = readInt();

            if (id == 0) return;

            for (size_t i = 0; i < users.size(); ++i) {
                if (users[i].id == id) {

                    if (users[i].role == ROLE_ADMIN) {
                        cout << "Cannot delete admin.\n";
                        return;
                    }

                    users.erase(users.begin() + i);

                    cout << "User deleted.\n";
                    return;
                }
            }

            cout << "User not found.\n";
        }
    

    void removeUserFromGroupInteractive() {
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

    void addGroupInteractive() {
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

    void addSessionInteractive() {
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

    void assignDancerToGroupInteractive() {
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

    void showEvents() const {
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

    void showEventParticipantsInteractive() {
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

    void addEventInteractive() {
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

    void assignUserToEventInteractive() {
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

    void showGroups() const {
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

    void showUsersByRole(int role) const {
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].role == role) {
                cout << "ID: " << users[i].id
                    << " | Name: " << users[i].name
                    << " | Phone: " << users[i].phone << "\n";
            }
        }
    }

    void showCoachesAndAdmins() const {
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].role == ROLE_COACH || users[i].role == ROLE_ADMIN) {
                cout << "ID: " << users[i].id
                    << " | Name: " << users[i].name
                    << " | Role: " << roleToString(users[i].role) << "\n";
            }
        }
    }

    void editStudioInfo() {
        while (true) {
            cout << "\n=== EDIT STUDIO INFO ===\n";
            cout << "1. Edit studio description\n";
            cout << "2. Edit studio rules\n";
            cout << "3. Edit studio Instagram link\n";
            cout << "4. Edit studio TikTok link\n";
            cout << "5. Edit Contemporary info\n";
            cout << "6. Edit Heels info\n";
            cout << "7. Edit Jazz Funk info\n";
            cout << "8. Edit Jazz Open info\n";
            cout << "9. Edit Lady Style info\n";
            cout << "10. Edit Show Group info\n";
            cout << "11. Edit Stretching & Acrobatics info\n";
            cout << "12. Edit Soft Stretching info\n";
            cout << "13. Add competition photo link\n";
            cout << "14. Add extra info link\n";
            cout << "0. Back\n";
            cout << "Choose: ";

            int choice = readInt();

            if (choice == 0) return;

            if (choice == 1) {
                cout << "\nCurrent studio description:\n" << studioInfo.studioDescription << "\n\n";
                string text = readLine("New studio description: ");
                copyText(studioInfo.studioDescription, MAX_TEXT, text);
                cout << "Studio description updated.\n";
            }
            else if (choice == 2) {
                cout << "\nCurrent studio rules:\n" << studioInfo.rules << "\n\n";
                string text = readLine("New studio rules: ");
                copyText(studioInfo.rules, MAX_TEXT, text);
                cout << "Studio rules updated.\n";
            }
            else if (choice == 3) {
                string text = readLine("New studio Instagram link: ");
                copyText(studioInfo.studioInstagram, MAX_LINK, text);
                cout << "Studio Instagram updated.\n";
            }
            else if (choice == 4) {
                string text = readLine("New studio TikTok link: ");
                copyText(studioInfo.studioTikTok, MAX_LINK, text);
                cout << "Studio TikTok updated.\n";
            }
            else if (choice == 5) {
                cout << "\nCurrent Contemporary info:\n" << studioInfo.contemporaryInfo << "\n\n";
                string text = readLine("New Contemporary info: ");
                copyText(studioInfo.contemporaryInfo, MAX_TEXT, text);
                cout << "Contemporary info updated.\n";
            }
            else if (choice == 6) {
                cout << "\nCurrent Heels info:\n" << studioInfo.heelsInfo << "\n\n";
                string text = readLine("New Heels info: ");
                copyText(studioInfo.heelsInfo, MAX_TEXT, text);
                cout << "Heels info updated.\n";
            }
            else if (choice == 7) {
                cout << "\nCurrent Jazz Funk info:\n" << studioInfo.jazzFunkInfo << "\n\n";
                string text = readLine("New Jazz Funk info: ");
                copyText(studioInfo.jazzFunkInfo, MAX_TEXT, text);
                cout << "Jazz Funk info updated.\n";
            }
            else if (choice == 8) {
                cout << "\nCurrent Jazz Open info:\n" << studioInfo.jazzOpenInfo << "\n\n";
                string text = readLine("New Jazz Open info: ");
                copyText(studioInfo.jazzOpenInfo, MAX_TEXT, text);
                cout << "Jazz Open info updated.\n";
            }
            else if (choice == 9) {
                cout << "\nCurrent Lady Style info:\n" << studioInfo.ladyStyleInfo << "\n\n";
                string text = readLine("New Lady Style info: ");
                copyText(studioInfo.ladyStyleInfo, MAX_TEXT, text);
                cout << "Lady Style info updated.\n";
            }
            else if (choice == 10) {
                cout << "\nCurrent Show Group info:\n" << studioInfo.showGroupInfo << "\n\n";
                string text = readLine("New Show Group info: ");
                copyText(studioInfo.showGroupInfo, MAX_TEXT, text);
                cout << "Show Group info updated.\n";
            }
            else if (choice == 11) {
                cout << "\nCurrent Stretching & Acrobatics info:\n" << studioInfo.stretchingInfo << "\n\n";
                string text = readLine("New Stretching & Acrobatics info: ");
                copyText(studioInfo.stretchingInfo, MAX_TEXT, text);
                cout << "Stretching & Acrobatics info updated.\n";
            }
            else if (choice == 12) {
                cout << "\nCurrent Soft Stretching info:\n" << studioInfo.softStretchingInfo << "\n\n";
                string text = readLine("New Soft Stretching info: ");
                copyText(studioInfo.softStretchingInfo, MAX_TEXT, text);
                cout << "Soft Stretching info updated.\n";
            }
            else if (choice == 13) {
                if (studioInfo.competitionPhotoCount >= MAX_INFO_ITEMS) {
                    cout << "Competition photo link list is full.\n";
                }
                else {
                    string text = readLine("Competition photo link: ");
                    copyText(studioInfo.competitionPhotoLinks[studioInfo.competitionPhotoCount], MAX_LINK, text);
                    studioInfo.competitionPhotoCount++;
                    cout << "Competition photo link added.\n";
                }
            }
            else if (choice == 14) {
                if (studioInfo.extraInfoCount >= MAX_INFO_ITEMS) {
                    cout << "Extra info link list is full.\n";
                }
                else {
                    string text = readLine("Extra info link: ");
                    copyText(studioInfo.extraInfoLinks[studioInfo.extraInfoCount], MAX_LINK, text);
                    studioInfo.extraInfoCount++;
                    cout << "Extra info link added.\n";
                }
            }
            else {
                cout << "Invalid choice.\n";
            }
        }
    }

};


int main() {
    DanceStudioApp app;
    app.run();
    return 0;
}