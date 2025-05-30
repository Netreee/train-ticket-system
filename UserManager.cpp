#include "UserManager.h"

namespace trainsys {
    UserManager::UserManager(const char *filename): userInfoTable(filename) {
    }

    void UserManager::insertUser(const UserID &userID, const char *username, const char* password, int privilege) {
        userInfoTable.insert(userID, UserInfo(userID, username, password, privilege));
    }

    bool UserManager::existUser(const UserID &userID) {
        return userInfoTable.contains(userID);
    }

    UserInfo UserManager::findUser(const UserID &userID) {
        /* Question */
    }

    void UserManager::removeUser(const UserID &userID) {
        /* Question */
    }

    void UserManager::modifyUserPrivilege(const UserID &userID, int newPrevilege) {
        /* Question */
    }

    void UserManager::modifyUserPassword(const UserID &userID, const char *newPassword) {
        /* Question */
    }
} // namespace trainsys
