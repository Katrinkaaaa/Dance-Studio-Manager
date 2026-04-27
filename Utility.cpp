#include "DanceStudioApp.h"
#include <iostream>
#include <limits>
#include <cstring>
#include <ctime>
#include <cctype>

using namespace std;

void DanceStudioApp::copyText(char* destination, int size, const string& source) {
    strncpy_s(destination, size, source.c_str(), size - 1);
    destination[size - 1] = '\0';
}

bool DanceStudioApp::isValidPhone(const string& phone) {
    if (phone.size() != 8) return false;
    for (char c : phone) {
        if (c < '0' || c > '9') return false;
    }
    return true;
}

bool DanceStudioApp::isValidPassword(const string& password) {
    if (password.size() < 8) return false;
    for (char c : password) {
        if (!isalnum(c)) return false;
    }
    return true;
}

string DanceStudioApp::readLine(const string& prompt) {
    cout << prompt;
    string value;
    getline(cin, value);
    return value;
}

int DanceStudioApp::readInt() {
    int value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Enter integer: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

double DanceStudioApp::readDouble() {
    double value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Enter a number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

bool DanceStudioApp::readYesNo(const string& prompt) {
    while (true) {
        string value = toLowerText(readLine(prompt + " (y/n): "));
        if (value == "y" || value == "yes") return true;
        if (value == "n" || value == "no") return false;
        cout << "Please enter y or n.\n";
    }
}

string DanceStudioApp::toLowerText(string value) {
    for (char& c : value) {
        if (c >= 'A' && c <= 'Z') {
            c = static_cast<char>(c - 'A' + 'a');
        }
    }
    return value;
}

int DanceStudioApp::getCurrentDate() {
    time_t now = time(0);
    tm localNow;
    localtime_s(&localNow, &now);

    return (localNow.tm_year + 1900) * 10000
        + (localNow.tm_mon + 1) * 100
        + localNow.tm_mday;
}

int DanceStudioApp::getCurrentTimeHHMM() {
    time_t now = time(0);
    tm localNow;
    localtime_s(&localNow, &now);

    return localNow.tm_hour * 100 + localNow.tm_min;
}

time_t DanceStudioApp::toTimeValue(int date, int time) {
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

time_t DanceStudioApp::toTimeValueFromDateTime(long long dt) {
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

long long DanceStudioApp::buildDateTimeNumber(int date, int time) {
    return static_cast<long long>(date) * 10000LL + static_cast<long long>(time);
}

int DanceStudioApp::addDaysSimple(int date, int days) {
    time_t base = toTimeValue(date, 0);
    base += static_cast<time_t>(days) * 24 * 60 * 60;

    tm tmValue;
    localtime_s(&tmValue, &base);

    return (tmValue.tm_year + 1900) * 10000
        + (tmValue.tm_mon + 1) * 100
        + tmValue.tm_mday;
}

const char* DanceStudioApp::dayOfWeekToString(int day) {
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

int DanceStudioApp::nextMonthFirstDay(int date) {
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

const char* DanceStudioApp::roleToString(int role) {
    if (role == ROLE_DANCER) return "Dancer";
    if (role == ROLE_COACH) return "Coach";
    return "Admin";
}

const char* DanceStudioApp::membershipTypeToString(int type) {
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

const char* DanceStudioApp::eventTypeToString(int type) {
    switch (type) {
    case EVENT_COMPETITION: return "Competition";
    case EVENT_CONCERT: return "Concert";
    case EVENT_GENERAL: return "General Event";
    default: return "Unknown";
    }
}

double DanceStudioApp::membershipPrice(int type) {
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

int DanceStudioApp::membershipClassesTotal(int type) {
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

int DanceStudioApp::membershipValidUntil(int purchaseDate, int type) {
    if (type == PASS_UNLIMITED) return nextMonthFirstDay(purchaseDate);
    return addDaysSimple(purchaseDate, 35);
}

bool DanceStudioApp::compareSessionDateTime(const Session& a, const Session& b) {
    if (a.date != b.date) return a.date < b.date;
    return a.time < b.time;
}

bool DanceStudioApp::compareEventDateTime(const Event& a, const Event& b) {
    if (a.date != b.date) return a.date < b.date;
    return a.time < b.time;
}
