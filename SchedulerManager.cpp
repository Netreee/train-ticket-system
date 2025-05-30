#include "SchedulerManager.h"


namespace trainsys {
    /**
     * @brief 构造函数，初始化调度管理器
     * @param filename 用于持久化存储的文件名
     * @note 
     * - 使用B+树作为底层存储结构
     * - 如果文件已存在，将从文件中加载数据
     * - 如果文件不存在，将创建新文件
     */
    SchedulerManager::SchedulerManager(const std::string &filename)
        : schedulerInfo(filename) {
    }

    /**
     * @brief 添加新的列车调度信息
     * @param trainID 列车ID，作为B+树的键值
     * @param seatNum 列车座位数
     * @param passingStationNumber 经过的站点数量
     * @param stations 站点ID数组，长度应等于passingStationNumber
     * @param duration 站点间运行时间数组，duration[i]表示从第i站到第i+1站的运行时间
     * @param price 站点间票价数组，price[i]表示从第i站到第i+1站的票价
     * @throw std::runtime_error 当站点数量超过最大限制时抛出异常（由TrainScheduler::addStation抛出）
     * @note 
     * - 如果trainID已存在，新的调度信息将覆盖旧的信息
     * - 调用者需确保stations、duration和price数组的长度正确
     * - duration和price数组的有效长度应为passingStationNumber-1
     */
    void SchedulerManager::addScheduler(const TrainID &trainID, int seatNum,
                                        int passingStationNumber, const StationID *stations, const int *duration,
                                        const int *price) {
        // 参数验证
        if (passingStationNumber <= 0) {
            throw std::invalid_argument("站点数量必须大于0");
        }
        if (passingStationNumber > MAX_PASSING_STATION_NUMBER) {
            throw std::invalid_argument("站点数量超过最大限制");
        }
        if (seatNum <= 0) {
            throw std::invalid_argument("座位数必须大于0");
        }
        if (!stations || !duration || !price) {
            throw std::invalid_argument("数组参数不能为空");
        }

        // 创建一个新的TrainScheduler对象
        TrainScheduler scheduler;
        
        // 设置列车ID和座位数
        scheduler.setTrainID(trainID);
        scheduler.setSeatNumber(seatNum);
        
        // 添加所有站点
        for (int i = 0; i < passingStationNumber; i++) {
            scheduler.addStation(stations[i]);
        }
        
        // 设置运行时间和票价
        scheduler.setDuration(duration);
        scheduler.setPrice(price);
        
        // 将调度信息插入到B+树中
        schedulerInfo.insert(trainID, scheduler);
    }

    /**
     * @brief 检查指定列车的调度信息是否存在
     * @param trainID 要查询的列车ID
     * @return 如果存在返回true，否则返回false
     * @note 
     * - 该操作的时间复杂度为O(log n)
     * - 不会抛出异常
     */
    bool SchedulerManager::existScheduler(const TrainID &trainID) {
        // 使用B+树的contains方法检查是否存在
        return schedulerInfo.contains(trainID);
    }

    /**
     * @brief 获取指定列车的调度信息
     * @param trainID 要查询的列车ID
     * @return 返回对应的TrainScheduler对象
     * @throw 如果trainID不存在，行为未定义（可能返回默认构造的对象）
     * @note 
     * - 调用此函数前应先使用existScheduler检查是否存在
     * - 返回的是对象的副本，对返回值的修改不会影响存储的数据
     * - 该操作的时间复杂度为O(log n)
     */
    TrainScheduler SchedulerManager::getScheduler(const TrainID &trainID) {
        // 使用B+树的findFirst方法获取调度信息
        return schedulerInfo.findFirst(trainID);
    }

    /**
     * @brief 删除指定列车的调度信息
     * @param trainID 要删除的列车ID
     * @note 
     * - 如果trainID不存在，操作将静默完成，不会抛出异常
     * - 删除后，该列车的所有调度信息将永久丢失
     * - 该操作的时间复杂度为O(log n)
     * - 删除操作可能触发B+树的重平衡
     */
    void SchedulerManager::removeScheduler(const TrainID &trainID) {
        // 使用B+树的removeFirst方法删除调度信息
        schedulerInfo.removeFirst(trainID);
    }
}
