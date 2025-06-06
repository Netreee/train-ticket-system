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

    UserInfo UserManager::findUser(const UserID& userID) {
        return userInfoTable.find(userID);
    }

    void UserManager::removeUser(const UserID& userID) {
        userInfoTable.remove(userID);
    }

    void UserManager::modifyUserPrivilege(const UserID& userID, int newPrivilege) {
        UserInfo info = findUser(userID);
        removeUser(userID);
        insertUser(userID, info.username, info.password, newPrivilege);
    }

    void UserManager::modifyUserPassword(const UserID& userID, const char* newPassword) {
        UserInfo info = findUser(userID);
        int len = strlen(newPassword);
        if (len > MAX_PASSWORD_LEN) len = MAX_PASSWORD_LEN;
        memcpy(info.password, newPassword, len);
        info.password[len] = '\0';
        removeUser(userID);
        insertUser(userID, info.username, info.password, info.privilege);
    }
} // namespace trainsys
