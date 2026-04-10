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

const int MAX_NAME = 64;
const int MAX_EMAIL = 64;
const int MAX_PASSWORD = 32;
const int MAX_LINK = 160;
const int MAX_TEXT = 220;
const int MAX_GROUPS_PER_USER = 12;
const int MAX_PARTICIPANTS = 80;

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
    char email[MAX_EMAIL];
    char password[MAX_PASSWORD];
    int role;
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
    int date;      // YYYYMMDD
    int time;      // HHMM
    int durationMin;
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

class DanceStudioApp {
private:
    vector<User> users;
    vector<Group> groups;
    vector<Session> sessions;
    vector<Membership> memberships;
    vector<Event> events;
    vector<Absence> absences;

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

            if (users[userIndex].role == ROLE_DANCER) {
                dancerMenu(userIndex);
            }
            else if (users[userIndex].role == ROLE_COACH) {
                coachMenu(userIndex);
            }
            else {
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
    }

    void saveAll() {
        saveUsers();
        saveGroups();
        saveSessions();
        saveMemberships();
        saveEvents();
        saveAbsences();
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

    // ---------- find helpers ----------
    int findUserIndexById(int id) const {
        for (size_t i = 0; i < users.size(); ++i) if (users[i].id == id) return static_cast<int>(i);
        return -1;
    }

    int findUserIdByEmail(const string& email) const {
        for (size_t i = 0; i < users.size(); ++i) if (email == users[i].email) return users[i].id;
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

        int elizavetaId = addUserRecord("Elizaveta", "elizaveta@studio.com", "1234", ROLE_COACH,
            "https://instagram.com/elizaveta_placeholder");
        int kamilaId = addUserRecord("Kamila", "kamila@studio.com", "1234", ROLE_COACH,
            "https://instagram.com/kamila_placeholder");
        int patriciaId = addUserRecord("Patricia", "patricia@studio.com", "1234", ROLE_COACH,
            "https://instagram.com/patricia_placeholder");
        int anastasiaId = addUserRecord("Anastasia", "anastasia@studio.com", "1234", ROLE_COACH,
            "https://instagram.com/anastasia_placeholder");
        int mariaId = addUserRecord("Maria", "maria@studio.com", "1234", ROLE_ADMIN,
            "https://instagram.com/maria_placeholder");
        int dancerId = addUserRecord("Demo Dancer", "dancer@studio.com", "1234", ROLE_DANCER, "");

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

        addSessionRecord(jazzFunkBegId, 20250908, 1800, 60, "Regular class");
        addSessionRecord(stretchId, 20250909, 1900, 60, "Regular class");
        addSessionRecord(heelsBegId, 20250910, 1900, 60, "Regular class");
        addSessionRecord(jazzOpenId, 20250911, 1800, 60, "Regular class");
        addSessionRecord(showGroupId, 20250912, 2000, 90, "Regular class");
        addSessionRecord(ladyStyleId, 20250915, 1900, 60, "Regular class");
        addSessionRecord(contempTechniqueId, 20250916, 1800, 60, "Regular class");
        addSessionRecord(heelsAdvId, 20250917, 2000, 60, "Regular class");
        addSessionRecord(contempBegId, 20250918, 1800, 60, "Regular class");
        addSessionRecord(contempIntId, 20250919, 1900, 60, "Regular class");
        addSessionRecord(contempAdultsId, 20250920, 1930, 60, "Regular class");
        addSessionRecord(softStretchId, 20250921, 1800, 60, "Regular class");
        addSessionRecord(jazzFunkIntId, 20250922, 1900, 60, "Regular class");

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

        saveAll();
    }

    int addUserRecord(const string& name, const string& email, const string& password, int role, const string& instagram) {
        User u;
        u.id = nextUserId();
        copyText(u.name, MAX_NAME, name);
        copyText(u.email, MAX_EMAIL, email);
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
    int addSessionRecord(int groupId, int date, int time, int durationMin, const string& note) {
        Session s;
        s.id = nextSessionId();
        s.groupId = groupId;
        s.date = date;
        s.time = time;
        s.durationMin = durationMin;
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
        cout << "\n=== DANCE STUDIO MANAGEMENT SYSTEM ===\n";
        cout << "1. Login\n";
        cout << "0. Exit\n";
        cout << "Choose: ";
        int choice = readInt();

        if (choice == 0) return -1;
        if (choice != 1) return -1;

        string email = readLine("Email: ");
        string password = readLine("Password: ");

        for (size_t i = 0; i < users.size(); ++i) {
            if (email == users[i].email && password == users[i].password) {
                cout << "\nWelcome, " << users[i].name
                    << " (" << roleToString(users[i].role) << ").\n";
                return static_cast<int>(i);
            }
        }

        cout << "Invalid email or password.\n";
        return -1;
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
            cout << "6. Info section\n";
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
            cout << "1. Show all users\n";
            cout << "2. Add dancer\n";
            cout << "3. Add coach\n";
            cout << "4. Add group\n";
            cout << "5. Assign dancer to group\n";
            cout << "6. Add session\n";
            cout << "7. Add event\n";
            cout << "8. Assign user to event\n";
            cout << "9. Activate membership\n";
            cout << "10. Show all memberships\n";
            cout << "11. Show all sessions\n";
            cout << "12. Studio info links\n";
            cout << "0. Logout\n";
            cout << "Choose: ";

            int choice = readInt();
            switch (choice) {
            case 1: showAllUsers(); break;
            case 2: addUserInteractive(ROLE_DANCER); break;
            case 3: addUserInteractive(ROLE_COACH); break;
            case 4: addGroupInteractive(); break;
            case 5: assignDancerToGroupInteractive(); break;
            case 6: addSessionInteractive(); break;
            case 7: addEventInteractive(); break;
            case 8: assignUserToEventInteractive(); break;
            case 9: activateMembershipInteractive(); break;
            case 10: showAllMemberships(); break;
            case 11: showAllSessions(); break;
            case 12: showInfoSection(); break;
            case 0: saveAll(); return;
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
            << setw(28) << "Email"
            << setw(12) << "Role"
            << "Instagram\n";
        cout << string(95, '-') << "\n";

        for (size_t i = 0; i < users.size(); ++i) {
            cout << left
                << setw(5) << users[i].id
                << setw(22) << users[i].name
                << setw(28) << users[i].email
                << setw(12) << roleToString(users[i].role)
                << users[i].instagram << "\n";
        }
    }

    void showAllSessions() {
        if (sessions.empty()) {
            cout << "No sessions found.\n";
            return;
        }

        vector<Session> copy = sessions;
        sort(copy.begin(), copy.end(), compareSessionDateTime);

        cout << left
            << setw(5) << "ID"
            << setw(28) << "Group"
            << setw(12) << "Date"
            << setw(8) << "Time"
            << setw(10) << "Duration"
            << setw(12) << "Status"
            << "Note\n";
        cout << string(95, '-') << "\n";

        for (size_t i = 0; i < copy.size(); ++i) {
            int gi = findGroupIndexById(copy[i].groupId);
            string groupName = (gi == -1) ? "Unknown" : groups[gi].name;
            cout << left
                << setw(5) << copy[i].id
                << setw(28) << groupName
                << setw(12) << copy[i].date
                << setw(8) << copy[i].time
                << setw(10) << copy[i].durationMin
                << setw(12) << (copy[i].cancelled ? "Cancelled" : "Active")
                << copy[i].note << "\n";
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

        sort(ownSessions.begin(), ownSessions.end(), compareSessionDateTime);

        cout << left
            << setw(5) << "ID"
            << setw(28) << "Group"
            << setw(12) << "Date"
            << setw(8) << "Time"
            << setw(10) << "Duration"
            << setw(12) << "Status"
            << "Note\n";
        cout << string(95, '-') << "\n";

        for (size_t i = 0; i < ownSessions.size(); ++i) {
            int gi = findGroupIndexById(ownSessions[i].groupId);
            cout << left
                << setw(5) << ownSessions[i].id
                << setw(28) << (gi == -1 ? "Unknown" : groups[gi].name)
                << setw(12) << ownSessions[i].date
                << setw(8) << ownSessions[i].time
                << setw(10) << ownSessions[i].durationMin
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
        cout << "\n=== INFO SECTION ===\n";
        cout << "Studio: MS Dance Group Riga\n";
        cout << "Studio Instagram: " << studioInstagram << "\n";
        cout << "Studio TikTok: " << studioTikTok << "\n\n";

        cout << "About the studio:\n";
        cout << "- Evening dance groups for different ages and levels.\n";
        cout << "- Personal schedule, memberships, absences, competitions, and concerts.\n\n";

        cout << "Dance styles and groups:\n";
        cout << "- Jazz Open (capacity 14)\n";
        cout << "- Jazz Funk Beginner (capacity 16)\n";
        cout << "- Jazz Funk Intermediate (capacity 14)\n";
        cout << "- Contemporary Beginner (capacity 15)\n";
        cout << "- Contemporary Intermediate (capacity 13)\n";
        cout << "- Contemporary Adults (capacity 12)\n";
        cout << "- Contemporary Technique (capacity 12)\n";
        cout << "- Heels Beginner (capacity 14)\n";
        cout << "- Heels Advanced (capacity 12)\n";
        cout << "- Show Group (capacity 10)\n";
        cout << "- Stretching & Acrobatics (capacity 16)\n";
        cout << "- Soft Stretching (capacity 14)\n";
        cout << "- Lady Style Intermediate (capacity 12)\n\n";

        cout << "Rules:\n";
        cout << "- If a dancer informs about absence at least 3 hours before the session,\n";
        cout << "  the class is not deducted.\n";
        cout << "- If the absence is submitted less than 3 hours before the session,\n";
        cout << "  the class is deducted from a limited membership.\n";
        cout << "- Coaches can cancel or reschedule only their own sessions.\n";
        cout << "- Membership is activated by the administrator after payment confirmation.\n\n";

        cout << "Coaches and Instagram links:\n";
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].role == ROLE_COACH || users[i].role == ROLE_ADMIN) {
                cout << "- " << users[i].name << " -> " << users[i].instagram << "\n";
            }
        }
    }

    // ---------- membership ----------
    void membershipMenuForDancer(int userIndex) {
        while (true) {
            cout << "\n=== MEMBERSHIP SECTION ===\n";
            cout << "1. Show membership types\n";
            cout << "2. Show my memberships\n";
            cout << "3. Buy membership\n";
            cout << "4. Recommend best membership\n";
            cout << "0. Back\n";
            cout << "Choose: ";

            int choice = readInt();
            switch (choice) {
            case 1: showMembershipTypes(); break;
            case 2: showUserMemberships(users[userIndex].id); break;
            case 3: buyMembership(users[userIndex].id); break;
            case 4: recommendMembership(); break;
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
                cout << "Status: " << (memberships[i].active ? "Active" : "Pending activation") << "\n";
                cout << "Payment link: " << memberships[i].paymentLink << "\n";
                cout << "-----------------------------\n";
            }
        }
        if (!found) cout << "No memberships found.\n";
    }

    void buyMembership(int userId) {
        showMembershipTypes();
        cout << "Choose membership type: ";
        int type = readInt();
        if (type < PASS_SINGLE || type > PASS_UNLIMITED) {
            cout << "Invalid membership type.\n";
            return;
        }

        cout << "Enter purchase date (YYYYMMDD): ";
        int purchaseDate = readInt();

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

    void recommendMembership() const {
        cout << "How many training sessions do you plan to attend this month? ";
        int planned = readInt();

        double bestCost = planned * membershipPrice(PASS_SINGLE);
        int bestType = PASS_SINGLE;
        double costs[6];
        costs[0] = planned * membershipPrice(PASS_SINGLE);
        costs[1] = (planned <= 4 ? membershipPrice(PASS_4) : membershipPrice(PASS_4) + (planned - 4) * membershipPrice(PASS_SINGLE));
        costs[2] = (planned <= 8 ? membershipPrice(PASS_8) : membershipPrice(PASS_8) + (planned - 8) * membershipPrice(PASS_SINGLE));
        costs[3] = (planned <= 12 ? membershipPrice(PASS_12) : membershipPrice(PASS_12) + (planned - 12) * membershipPrice(PASS_SINGLE));
        costs[4] = (planned <= 16 ? membershipPrice(PASS_16) : membershipPrice(PASS_16) + (planned - 16) * membershipPrice(PASS_SINGLE));
        costs[5] = membershipPrice(PASS_UNLIMITED);

        int types[6] = { PASS_SINGLE, PASS_4, PASS_8, PASS_12, PASS_16, PASS_UNLIMITED };

        for (int i = 0; i < 6; ++i) {
            if (costs[i] < bestCost) {
                bestCost = costs[i];
                bestType = types[i];
            }
        }

        cout << "Recommended membership: " << membershipTypeToString(bestType)
            << " (estimated cost " << fixed << setprecision(2) << bestCost << " EUR)\n";
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
            << setw(10) << "Status"
            << "\n";
        cout << string(90, '-') << "\n";

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
                << setw(10) << (memberships[i].active ? "Active" : "Pending")
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
        cout << "Enter submission date (YYYYMMDD): ";
        int currentDate = readInt();
        cout << "Enter submission time (HHMM): ";
        int currentTime = readInt();

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
            Membership* m = findActiveMembershipForUser(userId, sessions[si].date);
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

        cout << "Enter new date (YYYYMMDD): ";
        sessions[si].date = readInt();
        cout << "Enter new time (HHMM): ";
        sessions[si].time = readInt();
        string note = readLine("Enter note for reschedule: ");
        copyText(sessions[si].note, MAX_TEXT, note);
        sessions[si].cancelled = false;
        cout << "Session rescheduled.\n";
    }

    // ---------- admin actions ----------
    void addUserInteractive(int role) {
        string name = readLine("Name: ");
        string email = readLine("Email: ");
        string password = readLine("Password: ");
        string instagram = readLine("Instagram link (or leave empty): ");

        if (findUserIdByEmail(email) != -1) {
            cout << "A user with this email already exists.\n";
            return;
        }

        int id = addUserRecord(name, email, password, role, instagram);
        cout << "User added with ID " << id << ".\n";
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

    void addSessionInteractive() {
        showGroups();
        cout << "Group ID: ";
        int groupId = readInt();
        if (findGroupIndexById(groupId) == -1) {
            cout << "Group not found.\n";
            return;
        }

        cout << "Date (YYYYMMDD): ";
        int date = readInt();
        cout << "Time (HHMM): ";
        int time = readInt();
        cout << "Duration in minutes: ";
        int duration = readInt();
        string note = readLine("Note: ");

        int id = addSessionRecord(groupId, date, time, duration, note);
        cout << "Session added with ID " << id << ".\n";
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

    // ---------- simple lists ----------
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

    void showUsersByRole(int role) const {
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].role == role) {
                cout << "ID: " << users[i].id
                    << " | Name: " << users[i].name
                    << " | Email: " << users[i].email << "\n";
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
};

int main() {
    DanceStudioApp app;
    app.run();
    return 0;
}