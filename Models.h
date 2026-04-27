#pragma once
#include "Constants.h"

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
