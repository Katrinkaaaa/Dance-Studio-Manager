#include "DanceStudioApp.h"
#include <iostream>
#include <cstring>
#include <cstddef>

using namespace std;

void DanceStudioApp::editStudioInfo() {
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

void DanceStudioApp::showFullStylesInfo() {
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

void DanceStudioApp::showWeeklyScheduleInfo() const {
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

void DanceStudioApp::showInfoSection() const {
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

void DanceStudioApp::showLevelDescriptions() const {
    cout << "\n=== GROUP LEVELS ===\n";
    cout << "OPEN - open groups for dancers of any level.\n";
    cout << "BEGINNER (BEG) - for beginner dancers (0-2 years of experience).\n";
    cout << "INTERMEDIATE (INT) - for dancers with experience (2+ years of experience).\n";
    cout << "ADVANCED (ADV) - closed groups for stronger and more experienced dancers.\n";
    cout << "SHOW GROUP - closed group for dancers selected by the studio.\n";
    cout << "----------------------------------------\n";
}

void DanceStudioApp::showAvailableGroupsForRequests() const {
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
