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
        
        // 清空参数映射
        for (int i = 0; i < 200; i++) {
            argMap[i] = nullptr;
        }
        
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
                login(stringToNumber(argMap['i']), argMap['p']);
            } else if (strcmp(commandName, "logout") == 0) {
                logout();
            } else if (strcmp(commandName, "modify_password") == 0) {
                modifyUserPassword(stringToNumber(argMap['i']), argMap['p']);
            } else if (strcmp(commandName, "modify_privilege") == 0) {
                modifyUserPrivilege(stringToNumber(argMap['i']), stringToNumber(argMap['g']));
            } else if (strcmp(commandName, "query_profile") == 0) {
                findUserInfoByUserID(stringToNumber(argMap['i']));
            } else if (strcmp(commandName, "add_train") == 0) {
                // 解析站点、时长和价格数组
                seqList<char *> stationTokens = splitTokens(argMap['s'], '|');
                seqList<char *> durationTokens = splitTokens(argMap['t'], '|');
                seqList<char *> priceTokens = splitTokens(argMap['p'], '|');
                
                int stationNum = stationTokens.length();
                StationID *stations = new StationID[stationNum];
                int *durations = new int[stationNum - 1];
                int *prices = new int[stationNum - 1];
                
                // 转换站点名称为ID
                for (int i = 0; i < stationNum; i++) {
                    stations[i] = stationManager->getStationID(stationTokens.visit(i));
                }
                
                // 转换时长和价格
                for (int i = 0; i < stationNum - 1; i++) {
                    durations[i] = stringToNumber(durationTokens.visit(i));
                    prices[i] = stringToNumber(priceTokens.visit(i));
                }
                
                addTrainScheduler(TrainID(argMap['i']), stringToNumber(argMap['n']), 
                                stationNum, stations, durations, prices);
                
                // 清理内存
                delete[] stations;
                delete[] durations;
                delete[] prices;
                for (int i = 0; i < stationTokens.length(); i++) {
                    delete[] stationTokens.visit(i);
                }
                for (int i = 0; i < durationTokens.length(); i++) {
                    delete[] durationTokens.visit(i);
                }
                for (int i = 0; i < priceTokens.length(); i++) {
                    delete[] priceTokens.visit(i);
                }
            } else if (strcmp(commandName, "query_train") == 0) {
                queryTrainScheduler(TrainID(argMap['i']));
            } else if (strcmp(commandName, "release_ticket") == 0) {
                if (schedulerManager->existScheduler(TrainID(argMap['i']))) {
                    TrainScheduler scheduler = schedulerManager->getScheduler(TrainID(argMap['i']));
                    releaseTicket(scheduler, Date(argMap['d']));
                } else {
                    std::cout << "Train not found." << std::endl;
                }
            } else if (strcmp(commandName, "expire_ticket") == 0) {
                if (schedulerManager->existScheduler(TrainID(argMap['i']))) {
                    expireTicket(TrainID(argMap['i']), Date(argMap['d']));
                } else {
                    std::cout << "Train not found." << std::endl;
                }
            } else if (strcmp(commandName, "display_route") == 0) {
                try{
                    StationID fromStation = stationManager->getStationID(argMap['s']);
                    StationID toStation = stationManager->getStationID(argMap['t']);
                    findAllRoute(fromStation, toStation);
                }catch (const std::exception& e) {
                    std::cout << "Disconnected. No route found." << std::endl;
                }
            } else if (strcmp(commandName, "query_best_path") == 0) {
                StationID fromStation = stationManager->getStationID(argMap['s']);
                StationID toStation = stationManager->getStationID(argMap['t']);
                int preference = (argMap['o'] && strcmp(argMap['o'], "cost") == 0) ? 1 : 0;
                findBestRoute(fromStation, toStation, preference);
            } else if (strcmp(commandName, "query_remaining") == 0) {
                int remaining = queryRemainingTicket(TrainID(argMap['i']), Date(argMap['d']), stationManager->getStationID(argMap['s']));
                if (remaining >= 0) {
                    std::cout << "Remaining ticket: " << remaining << std::endl;
                }
            } else if (strcmp(commandName, "buy_ticket") == 0) {
                StationID departureStation = stationManager->getStationID(argMap['s']);
                orderTicket(TrainID(argMap['i']), Date(argMap['d']), departureStation);
            } else if (strcmp(commandName, "query_order") == 0) {
                queryMyTicket();
            } else if (strcmp(commandName, "refund_ticket") == 0) {
                StationID departureStation = stationManager->getStationID(argMap['s']);
                refundTicket(TrainID(argMap['i']), Date(argMap['d']), departureStation);
            } else if (strcmp(commandName, "query_accessibility") == 0) {
                StationID fromStation = stationManager->getStationID(argMap['s']);
                StationID toStation = stationManager->getStationID(argMap['t']);
                bool accessible = railwayGraph->checkStationAccessibility(fromStation, toStation);
                std::cout << "站点可达性: " << (accessible ? "可达" : "不可达") << std::endl;
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
