#pragma once

inline const char* USERS_FILE = "users.dat";
inline const char* GROUPS_FILE = "groups.dat";
inline const char* SESSIONS_FILE = "sessions.dat";
inline const char* MEMBERSHIPS_FILE = "memberships.dat";
inline const char* EVENTS_FILE = "events.dat";
inline const char* ABSENCES_FILE = "absences.dat";
inline const char* STUDIO_INFO_FILE = "studio_info.dat";
inline const char* JOIN_REQUESTS_FILE = "join_requests.dat";

inline const int MAX_NAME = 64;
inline const int MAX_PHONE = 100;
inline const int MAX_PASSWORD = 32;
inline const int MAX_LINK = 160;
inline const int MAX_TEXT = 220;
inline const int MAX_GROUPS_PER_USER = 12;
inline const int MAX_PARTICIPANTS = 80;
inline const int MAX_INFO_ITEMS = 20;
inline const int MAX_REQUEST_GROUPS = 10;

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
