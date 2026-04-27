#include "DanceStudioApp.h"
#include <iostream>
#include <iomanip>
#include <cstddef>

using namespace std; 

void DanceStudioApp::membershipMenuForDancer(int userIndex) {
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

void DanceStudioApp::showMembershipTypes() const {
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

void DanceStudioApp::showUserMemberships(int userId) const {
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

void DanceStudioApp::buyMembership(int userId) {
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

string DanceStudioApp::getMembershipStatusText(const Membership& m) const {
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

Membership* DanceStudioApp::findActiveMembershipForUser(int userId, int date) {
    for (size_t i = 0; i < memberships.size(); ++i) {
        if (memberships[i].userId != userId) continue;
        if (!memberships[i].active) continue;
        if (memberships[i].validUntil < date) continue;
        if (memberships[i].classesTotal != 0 && memberships[i].classesUsed >= memberships[i].classesTotal) continue;
        return &memberships[i];
    }
    return NULL;
}

void DanceStudioApp::showAllMemberships() const {
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

void DanceStudioApp::activateMembershipInteractive() {
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
