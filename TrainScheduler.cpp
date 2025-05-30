#include "TrainScheduler.h"

// 列车调度类，储存单个列车信息

namespace trainsys {
    /**
     * @brief 构造函数，初始化一个空的列车调度对象
     * @note 初始化站点数量和座位数为0
     */
    TrainScheduler::TrainScheduler() {
        this->passingStationNum = 0;
        this->seatNum = 0;
    }

    /**
     * @brief 析构函数
     * @note 使用固定数组，无需额外清理
     */
    TrainScheduler::~TrainScheduler() {
    }

    /**
     * @brief 在列车路线末尾添加新站点
     * @param station 要添加的站点ID
     * @throw std::runtime_error 当站点数量达到最大限制时抛出异常
     * @note 新添加站点的运行时间和票价初始化为0
     */
    void TrainScheduler::addStation(const StationID &station) {
        if (this->passingStationNum >= MAX_PASSING_STATION_NUMBER) {
            throw std::runtime_error("Station array is full");
        }
        this->stations[this->passingStationNum] = station;
        this->duration[this->passingStationNum] = 0;
        this->price[this->passingStationNum] = 0;
        this->passingStationNum++;
    }

    /**
     * @brief 在指定位置插入新站点
     * @param i 插入位置的索引（0-based）
     * @param station 要插入的站点ID
     * @throw std::runtime_error 当索引无效或站点数量达到最大限制时抛出异常
     * @note 
     * - 索引i可以等于passingStationNum，表示在末尾插入
     * - 新插入站点的运行时间和票价初始化为0
     * - 会导致i及其后续站点的信息后移
     */
    void TrainScheduler::insertStation(int i, const StationID &station) {
        if (i < 0 || i > this->passingStationNum) {
            throw std::runtime_error("Invalid insert position");
        }
        if (this->passingStationNum >= MAX_PASSING_STATION_NUMBER) {
            throw std::runtime_error("Station array is full");
        }
        
        // 将i及其后面的元素后移一位
        for (int j = this->passingStationNum; j > i; j--) {
            this->stations[j] = this->stations[j - 1];
            this->duration[j] = this->duration[j - 1];
            this->price[j] = this->price[j - 1];
        }
        
        this->stations[i] = station;
        this->duration[i] = 0;  // 新插入站点的运行时间初始化为0
        this->price[i] = 0;     // 新插入站点的票价初始化为0
        this->passingStationNum++;
    }

    /**
     * @brief 删除指定位置的站点
     * @param i 要删除的站点索引（0-based）
     * @throw std::runtime_error 当索引无效时抛出异常
     * @note 删除后，后续站点的信息会前移
     */
    void TrainScheduler::removeStation(int i) {
        if (i < 0 || i >= this->passingStationNum) {
            throw std::runtime_error("Invalid station index");
        }
        
        // 将i后面的元素前移一位
        for (int j = i; j < this->passingStationNum - 1; j++) {
            this->stations[j] = this->stations[j + 1];
            this->duration[j] = this->duration[j + 1];
            this->price[j] = this->price[j + 1];
        }
        this->passingStationNum--;
    }

    /**
     * @brief 查找指定站点的索引
     * @param stationID 要查找的站点ID
     * @return 返回站点的索引，如果未找到则返回-1
     */
    int TrainScheduler::findStation(const StationID &stationID) {
        for (int i = 0; i < this->passingStationNum; i++) {
            if (this->stations[i] == stationID) {
                return i;
            }
        }
        return -1;  // 未找到返回-1
    }

    /**
     * @brief 遍历并打印所有站点信息
     * @note 按顺序打印每个站点的ID、到下一站的运行时间和票价
     */
    void TrainScheduler::traverseStation() {
        for (int i = 0; i < this->passingStationNum; i++) {
            std::cout << "Station " << i << ": " << this->stations[i] 
                      << ", Duration: " << this->duration[i]
                      << ", Price: " << this->price[i] << std::endl;
        }
    }

    /**
     * @brief 设置列车ID
     * @param id 新的列车ID
     */
    void TrainScheduler::setTrainID(const TrainID &id) {
        this->trainID = id;
    }

    /**
     * @brief 批量设置站点间票价
     * @param price 票价数组，长度应不小于当前站点数
     * @note price[i]表示从第i站到第i+1站的票价
     */
    void TrainScheduler::setPrice(const int price[]) {
        for (int i = 0; i < this->passingStationNum - 1; i++) {
            this->price[i] = price[i];
        }
    }

    /**
     * @brief 批量设置站点间运行时间
     * @param duration 运行时间数组，长度应不小于当前站点数
     * @note duration[i]表示从第i站到第i+1站的运行时间
     */
    void TrainScheduler::setDuration(const int duration[]) {
        for (int i = 0; i < this->passingStationNum - 1; i++) {
            this->duration[i] = duration[i];
        }
    }

    /**
     * @brief 设置列车座位数
     * @param seatNum 新的座位数
     */
    void TrainScheduler::setSeatNumber(int seatNum) {
        this->seatNum = seatNum;
    }

    /**
     * @brief 获取列车ID
     * @return 返回列车ID
     */
    TrainID TrainScheduler::getTrainID() const {
        return this->trainID;
    }

    /**
     * @brief 获取座位数
     * @return 返回座位数
     */
    int TrainScheduler::getSeatNum() const {
        return this->seatNum;
    }

    /**
     * @brief 获取当前站点数量
     * @return 返回站点数量
     */
    int TrainScheduler::getPassingStationNum() const {
        return this->passingStationNum;
    }

    /**
     * @brief 获取指定索引的站点ID
     * @param i 站点索引（0-based）
     * @return 返回站点ID
     * @throw std::runtime_error 当索引无效时抛出异常
     */
    StationID TrainScheduler::getStation(int i) const {
        if (i < 0 || i >= this->passingStationNum) {
            throw std::runtime_error("Invalid station index");
        }
        return this->stations[i];
    }

    /**
     * @brief 获取指定索引的运行时间
     * @param i 站点索引（0-based）
     * @return 返回从第i站到第i+1站的运行时间
     * @throw std::runtime_error 当索引无效时抛出异常
     */
    int TrainScheduler::getDuration(int i) const {
        if (i < 0 || i >= this->passingStationNum) {
            throw std::runtime_error("Invalid duration index");
        }
        return this->duration[i];
    }

    /**
     * @brief 获取指定索引的票价
     * @param i 站点索引（0-based）
     * @return 返回从第i站到第i+1站的票价
     * @throw std::runtime_error 当索引无效时抛出异常
     */
    int TrainScheduler::getPrice(int i) const {
        if (i < 0 || i >= this->passingStationNum) {
            throw std::runtime_error("Invalid price index");
        }
        return this->price[i];
    }

    /**
     * @brief 批量获取所有站点ID
     * @param stations 用于存储站点ID的数组，调用者需确保数组大小足够
     * @note 
     * - 这是一个批量获取接口，不返回值而是通过参数传出数据
     * - 调用者必须确保传入的数组大小不小于当前站点数量
     * - 函数会将所有站点ID按顺序复制到传入的数组中
     */
    void TrainScheduler::getStations(StationID *stations) const {
        for (int i = 0; i < this->passingStationNum; i++) {
            stations[i] = this->stations[i];
        }
    }

    /**
     * @brief 批量获取所有运行时间
     * @param duration 用于存储运行时间的数组，调用者需确保数组大小足够
     * @note 
     * - 这是一个批量获取接口，不返回值而是通过参数传出数据
     * - 调用者必须确保传入的数组大小不小于当前站点数量
     * - duration[i]表示从第i站到第i+1站的运行时间
     */
    void TrainScheduler::getDuration(int *duration) const {
        for (int i = 0; i < this->passingStationNum; i++) {
            duration[i] = this->duration[i];
        }
    }

    /**
     * @brief 批量获取所有票价
     * @param price 用于存储票价的数组，调用者需确保数组大小足够
     * @note 
     * - 这是一个批量获取接口，不返回值而是通过参数传出数据
     * - 调用者必须确保传入的数组大小不小于当前站点数量
     * - price[i]表示从第i站到第i+1站的票价
     */
    void TrainScheduler::getPrice(int *price) const {
        for (int i = 0; i < this->passingStationNum; i++) {
            price[i] = this->price[i];
        }
    }

    /**
     * @brief 判断两个列车调度对象是否相等
     * @param rhs 要比较的另一个对象
     * @return 如果两个对象的trainID相同则返回true
     * @note 只比较trainID，不比较其他属性
     */
    bool TrainScheduler::operator==(const TrainScheduler &rhs) const {
        return this->trainID == rhs.trainID;
    }

    /**
     * @brief 判断两个列车调度对象是否不相等
     * @param rhs 要比较的另一个对象
     * @return 如果两个对象的trainID不同则返回true
     */
    bool TrainScheduler::operator!=(const TrainScheduler &rhs) const {
        return !(*this == rhs);
    }

    /**
     * @brief 比较两个列车调度对象的大小
     * @param rhs 要比较的另一个对象
     * @return 如果当前对象的trainID小于rhs的trainID则返回true
     * @note 用于在有序容器中存储TrainScheduler对象
     */
    bool TrainScheduler::operator<(const TrainScheduler &rhs) const {
        return this->trainID < rhs.trainID;
    }

    /**
     * @brief 输出流运算符重载
     * @param os 输出流对象
     * @param trainScheduler 要输出的列车调度对象
     * @return 返回输出流对象
     * @note 按照特定格式输出列车信息，包括：
     * - 列车ID
     * - 座位数
     * - 所有站点信息（包括站点ID、运行时间和票价）
     */
    std::ostream &operator<<(std::ostream &os, const TrainScheduler &trainScheduler) {
        os << "Train " << trainScheduler.trainID << ":\n";
        os << "Seat Number: " << trainScheduler.seatNum << "\n";
        os << "Stations:\n";
        for (int i = 0; i < trainScheduler.passingStationNum; i++) {
            os << "  " << i + 1 << ". " << trainScheduler.stations[i];
            if (i < trainScheduler.passingStationNum - 1) {
                os << " -> Duration: " << trainScheduler.duration[i]
                   << ", Price: " << trainScheduler.price[i];
            }
            os << "\n";
        }
        return os;
    }
}
