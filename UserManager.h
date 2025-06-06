#ifndef USER_MANAGER_H_
#define USER_MANAGER_H_

#include "Utils.h"
#include "UserInfo.h"
#include "DataStructure/List.h"
#include "DataStructure/BPlusTree.h"
#include "DataStructure/CachedBPlusTree.h"

namespace trainsys {
    class UserManager {
    private:
        CachedBPlusTree<UserID, UserInfo> userInfoTable;

    public:
        UserManager(const char *filename);

        ~UserManager() = default;

        void insertUser(const UserID &userID, const char *username, const char *password, int privilege);

        bool existUser(const UserID &userID);

        UserInfo findUser(const UserID &userID);

        void removeUser(const UserID &userID);

        void modifyUserPrivilege(const UserID &userID, int newPrivilege);

        void modifyUserPassword(const UserID &userID, const char *newPassword);
    };
}

#endif
