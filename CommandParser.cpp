#include "CommandParser.h"
#include "TrainSystem.h"
#include "DataStructure/List.h"
#include "DataStructure/RedBlackTree.h"

namespace trainsys {
    extern UserInfo currentUser;
    extern UserManager *userManager;
    extern RailwayGraph *railwayGraph;
    extern SchedulerManager *schedulerManager;
    extern TicketManager *ticketManager;
    extern PrioritizedWaitingList *waitingList;
    extern TripManager *tripManager;
    extern StationManager *stationManager;

    static char *argMap[200];
    extern RedBlackTree<String, StationID> stationNameToIDMapping;

    bool isStringEnding(char ch) {
        return ch == '\0' || ch == '\n' || ch == '\r';
    }

    seqList<char *> splitTokens(const char *command, char separator) {
        seqList<char *> tokens;
        int i = 0;
        while (true) {
            int j = i;
            while (!isStringEnding(command[j]) && command[j] != separator) {
                ++j;
            }
            char *token = new char[j - i + 1];
            for (int k = i; k < j; ++k) {
                token[k - i] = command[k];
            }
            token[j - i] = '\0';
            tokens.insert(tokens.length(), token);
            i = j + 1;
            if (isStringEnding(command[j])) {
                break;
            }
        }
        return tokens;
    }

    long long stringToNumber(const char *str) {
        long long number = 0;
        int i = 0;
        while (str[i] != '\0') {
            number = number * 10 + str[i] - '0';
            ++i;
        }
        return number;
    }

    int parseCommand(const char *command) {
        int exitCode = 0;
        seqList<char *> tokens = splitTokens(command, ' ');
        for (int i = 1; i < tokens.length(); i += 2) {
            if (tokens.visit(i)[0] == '-') {
                argMap[(int) tokens.visit(i)[1]] = tokens.visit(i + 1);
            } else {
                exitCode = -1;
            }
        }
        char *commandName = tokens.visit(0);
        if (exitCode != -1) {
            if (strcmp(commandName, "register") == 0) {
                addUser(stringToNumber(argMap['i']), argMap['u'], argMap['p']);
            } else if (strcmp(commandName, "login") == 0) {
                /* Question */
            } else if (strcmp(commandName, "logout") == 0) {
                /* Question */
            } else if (strcmp(commandName, "modify_password") == 0) {
                /* Question */
            } else if (strcmp(commandName, "modify_privilege") == 0) {
                /* Question */
            } else if (strcmp(commandName, "query_profile") == 0) {
                /* Question */
            } else if (strcmp(commandName, "add_train") == 0) {
                /* Question */
            } else if (strcmp(commandName, "query_train") == 0) {
                /* Question */
            } else if (strcmp(commandName, "release_ticket") == 0) {
                /* Question */
                //检查优先级还没有写
                if(!schedulerManager->existScheduler(TrainID(argMap['i']))){
                    std::cout << "Train not found." << std::endl;
                }else{
                    ticketManager->releaseTicket(schedulerManager->getScheduler(TrainID(argMap['i'])), Date(argMap['d']));
                    std::cout << "Ticket released." << std::endl;
                }
            } else if (strcmp(commandName, "expire_ticket") == 0) {
                /* Question */
            } else if (strcmp(commandName, "display_route") == 0) {
                /* Question */
            } else if (strcmp(commandName, "query_best_path") == 0) {
                /* Question */
            } else if (strcmp(commandName, "query_remaining") == 0) {
                /* Question */
            } else if (strcmp(commandName, "buy_ticket") == 0) {
                /* Question */
            } else if (strcmp(commandName, "query_order") == 0) {
                /* Question */
            } else if (strcmp(commandName, "refund_ticket") == 0) {
                /* Question */
            } else if (strcmp(commandName, "query_accessibility") == 0) {
                /* Question */
            } else if (strcmp(commandName, "exit") == 0) {
                exitCode = 1;
            } else {
                std::cout << "Invalid command: " << commandName << std::endl;
                exitCode = -1;
            }
        }
        for (int i = 0; i < tokens.length(); ++i) {
            delete [](tokens.visit(i));
        }
        return exitCode;
    }
}
