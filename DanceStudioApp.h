#pragma once

#include <vector>
#include <string>
#include <ctime>
#include "Models.h"

class DanceStudioApp {
private:
    std::vector<User> users;
    std::vector<Group> groups;
    std::vector<Session> sessions;
    std::vector<Membership> memberships;
    std::vector<Event> events;
    std::vector<Absence> absences;
    std::vector<JoinRequest> joinRequests;

    StudioInfo studioInfo{};

    const std::string studioInstagram = "https://instagram.com/msdancegroup.riga";
    const std::string studioTikTok = "https://tiktok.com/@msdancegroup.riga";
    const std::string paymentLink = "https://internetbank.example.com/studio-payment";

public:
    void run();

private:
    // ---------- utility ----------
    static void copyText(char* destination, int size, const std::string& source);
    static bool isValidPhone(const std::string& phone);
    static bool isValidPassword(const std::string& password);
    static std::string toLowerText(std::string value);
    static int readInt();
    static double readDouble();
    static std::string readLine(const std::string& prompt);
    static bool readYesNo(const std::string& prompt);

    static int getCurrentDate();
    static int getCurrentTimeHHMM();
    static time_t toTimeValue(int date, int time);
    static time_t toTimeValueFromDateTime(long long dt);
    static long long buildDateTimeNumber(int date, int time);
    static int addDaysSimple(int date, int days);

    static const char* dayOfWeekToString(int day);
    static int nextMonthFirstDay(int date);

    static const char* roleToString(int role);
    static const char* membershipTypeToString(int type);
    static const char* eventTypeToString(int type);

    static double membershipPrice(int type);
    static int membershipClassesTotal(int type);
    static int membershipValidUntil(int purchaseDate, int type);

    static bool compareSessionDateTime(const Session& a, const Session& b);
    static bool compareEventDateTime(const Event& a, const Event& b);

    // ---------- ids ----------
    int nextUserId() const;
    int nextGroupId() const;
    int nextSessionId() const;
    int nextMembershipId() const;
    int nextEventId() const;
    int nextAbsenceId() const;
    int nextJoinRequestId() const;

    int maxIdUsers() const;
    int maxIdGroups() const;
    int maxIdSessions() const;
    int maxIdMemberships() const;
    int maxIdEvents() const;
    int maxIdAbsences() const;
    int maxIdJoinRequests() const;

    // ---------- persistence ----------
    void loadAll();
    void saveAll();

    void loadUsers();
    void saveUsers();
    void loadGroups();
    void saveGroups();
    void loadSessions();
    void saveSessions();
    void loadMemberships();
    void saveMemberships();
    void loadEvents();
    void saveEvents();
    void loadAbsences();
    void saveAbsences();
    void loadStudioInfo();
    void saveStudioInfo();
    void loadJoinRequests();
    void saveJoinRequests();

    // ---------- find helpers ----------
    int findUserIndexById(int id) const;
    int findUserIdByPhone(const std::string& phone) const;
    int findGroupIndexById(int id) const;
    int findSessionIndexById(int id) const;
    int findMembershipIndexById(int id) const;
    int findAbsenceIndexByUserAndSession(int userId, int sessionId) const;

    bool userInGroup(int userId, int groupId) const;
    bool eventAssignedToUser(const Event& eventItem, int userId) const;
    bool coachOwnsGroup(int coachUserId, int groupId) const;
    bool coachOwnsSession(int coachUserId, int sessionId) const;

    // ---------- seed ----------
    void seedDataIfNeeded();
    int addUserRecord(const std::string& name, const std::string& phone, const std::string& password, int role, const std::string& instagram);
    int addGroupRecord(const std::string& name, int coachUserId, int capacity,
        const std::string& description, const std::string& rules);
    int addSessionRecord(int groupId, int dayOfWeek, int time, int durationMin, const std::string& note, bool recurring = true);
    int addEventRecord(int type, const std::string& title, int date, int time, const std::string& note);

    bool assignUserToGroup(int userId, int groupId);
    bool assignUserToEvent(int userId, int eventId);

    // ---------- login ----------
    int login();
    void registerDancer();

    // ---------- menus ----------
    void dancerMenu(int userIndex);
    void coachMenu(int userIndex);
    void adminMenu(int userIndex);

    void adminUsersMenu();
    void adminGroupsMenu();
    void adminSessionsEventsMenu();
    void adminMembershipsMenu();
    void adminStudioInfoMenu();
    void adminCoachMenu(int adminUserId);

    // ---------- display ----------
    void showAllUsers() const;
    void showAllSessions();
    void showUserGroups(int userId) const;
    void showCoachSessions(int coachUserId);

    void showCalendarForUser(int userId, bool onlyMine, const std::string& styleFilter);
    void calendarFilterMenu(int userId);

    void showInfoSection() const;
    void showLevelDescriptions() const;
    void showFullStylesInfo();
    void showAvailableGroupsForRequests() const;
    void showWeeklyScheduleInfo() const;

    // ---------- join requests ----------
    void submitJoinRequest(int userIndex);
    void showJoinRequests();

    // ---------- membership ----------
    void membershipMenuForDancer(int userIndex);
    void showMembershipTypes() const;
    void showUserMemberships(int userId) const;
    void buyMembership(int userId);
    std::string getMembershipStatusText(const Membership& m) const;
    Membership* findActiveMembershipForUser(int userId, int date);
    void showAllMemberships() const;
    void activateMembershipInteractive();

    // ---------- absences ----------
    void submitAbsence(int userIndex);
    void showCoachAbsences(int coachUserId);

    // ---------- coach ----------
    void cancelCoachSession(int coachUserId);
    void rescheduleCoachSession(int coachUserId);

    // ---------- admin actions ----------
    void addUserInteractive(int role);
    void deleteUserInteractive();
    void removeUserFromGroupInteractive();

    void addGroupInteractive();
    void assignDancerToGroupInteractive();

    void addSessionInteractive();

    void addEventInteractive();
    void assignUserToEventInteractive();
    void addToEventMenu();
    void addGroupToEventInteractive();
    void addAllDancersToEventInteractive();
    void removeUserFromEventInteractive();
    void showEventParticipantsInteractive();

    void showEvents() const;
    void showGroups() const;
    void showUsersByRole(int role) const;
    void showCoachesAndAdmins() const;

    void editStudioInfo();
};
