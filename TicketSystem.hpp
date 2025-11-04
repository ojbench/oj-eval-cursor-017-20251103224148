#ifndef TICKETSYSTEM_HPP
#define TICKETSYSTEM_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>

// Simple vector implementation
template<typename T>
class Vector {
private:
    T* data;
    int capacity;
    int length;
    
    void resize() {
        capacity = capacity == 0 ? 4 : capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < length; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    
public:
    Vector() : data(nullptr), capacity(0), length(0) {}
    
    Vector(const Vector& other) : capacity(other.capacity), length(other.length) {
        data = new T[capacity];
        for (int i = 0; i < length; i++) {
            data[i] = other.data[i];
        }
    }
    
    ~Vector() {
        delete[] data;
    }
    
    void push_back(const T& val) {
        if (length >= capacity) {
            resize();
        }
        data[length++] = val;
    }
    
    T& operator[](int idx) { return data[idx]; }
    const T& operator[](int idx) const { return data[idx]; }
    
    int size() const { return length; }
    
    void clear() { length = 0; }
    
    T* begin() { return data; }
    T* end() { return data + length; }
};

// Simple file-based storage
template<typename T>
class FileStorage {
private:
    std::string filename;
    std::fstream file;
    
public:
    FileStorage(const std::string& fname) : filename(fname) {}
    
    void write(int pos, const T& data) {
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            file.clear();
            file.open(filename, std::ios::out | std::ios::binary);
            file.close();
            file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
        }
        file.seekp(pos * sizeof(T));
        file.write(reinterpret_cast<const char*>(&data), sizeof(T));
        file.close();
    }
    
    bool read(int pos, T& data) {
        file.open(filename, std::ios::in | std::ios::binary);
        if (!file.is_open()) return false;
        file.seekg(pos * sizeof(T));
        file.read(reinterpret_cast<char*>(&data), sizeof(T));
        bool success = file.gcount() == sizeof(T);
        file.close();
        return success;
    }
    
    void clear() {
        std::remove(filename.c_str());
    }
};

// Simple date/time utilities
struct DateTime {
    int month, day, hour, minute;
    
    DateTime() : month(0), day(0), hour(0), minute(0) {}
    
    DateTime(int m, int d, int h, int min) : month(m), day(d), hour(h), minute(min) {}
    
    int toMinutes() const {
        int days = 0;
        if (month == 6) days = day - 1;
        else if (month == 7) days = 30 + day - 1;
        else if (month == 8) days = 61 + day - 1;
        return days * 24 * 60 + hour * 60 + minute;
    }
    
    static DateTime fromMinutes(int minutes) {
        int days = minutes / (24 * 60);
        int remaining = minutes % (24 * 60);
        int hour = remaining / 60;
        int minute = remaining % 60;
        
        int month, day;
        if (days < 30) {
            month = 6;
            day = days + 1;
        } else if (days < 61) {
            month = 7;
            day = days - 30 + 1;
        } else {
            month = 8;
            day = days - 61 + 1;
        }
        
        return DateTime(month, day, hour, minute);
    }
    
    std::string toString() const {
        if (month == 0) return "xx-xx xx:xx";
        char buf[20];
        sprintf(buf, "%02d-%02d %02d:%02d", month, day, hour, minute);
        return std::string(buf);
    }
};

// User structure
struct User {
    char username[21];
    char password[31];
    char name[31];
    char mailAddr[31];
    int privilege;
    bool exists;
    
    User() {
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        memset(name, 0, sizeof(name));
        memset(mailAddr, 0, sizeof(mailAddr));
        privilege = 0;
        exists = false;
    }
};

// Train structure
struct Train {
    char trainID[21];
    int stationNum;
    int seatNum;
    char stations[100][31];
    int prices[100];
    int startHour, startMinute;
    int travelTimes[100];
    int stopoverTimes[100];
    int saleStart, saleEnd;
    char type;
    bool released;
    bool exists;
    
    Train() {
        memset(trainID, 0, sizeof(trainID));
        stationNum = 0;
        seatNum = 0;
        for (int i = 0; i < 100; i++) {
            memset(stations[i], 0, 31);
            prices[i] = 0;
            travelTimes[i] = 0;
            stopoverTimes[i] = 0;
        }
        startHour = startMinute = 0;
        saleStart = saleEnd = 0;
        type = 0;
        released = false;
        exists = false;
    }
    
    int getCumulativePrice(int from, int to) const {
        int sum = 0;
        for (int i = from; i < to; i++) {
            sum += prices[i];
        }
        return sum;
    }
    
    DateTime getArriveTime(int station, int startDay) const {
        int minutes = startDay * 24 * 60 + startHour * 60 + startMinute;
        for (int i = 0; i < station; i++) {
            minutes += travelTimes[i];
            if (i > 0 && i < station) {
                minutes += stopoverTimes[i - 1];
            }
        }
        return DateTime::fromMinutes(minutes);
    }
    
    DateTime getLeaveTime(int station, int startDay) const {
        int minutes = startDay * 24 * 60 + startHour * 60 + startMinute;
        for (int i = 0; i < station; i++) {
            minutes += travelTimes[i];
            if (i > 0) {
                minutes += stopoverTimes[i - 1];
            }
        }
        if (station > 0) {
            minutes += stopoverTimes[station - 1];
        }
        return DateTime::fromMinutes(minutes);
    }
};

class TicketSystem {
private:
    FileStorage<User> users;
    FileStorage<Train> trains;
    bool loggedIn[10000];
    int userCount;
    int trainCount;
    
    void parseCommand(const std::string& cmd, char keys[20], std::string values[20], int& count) {
        std::istringstream iss(cmd);
        std::string token;
        count = 0;
        
        while (iss >> token) {
            if (token[0] == '-' && token.length() == 2) {
                char key = token[1];
                std::string value;
                if (iss >> value) {
                    keys[count] = key;
                    values[count] = value;
                    count++;
                }
            }
        }
    }
    
    std::string getParam(char key, char keys[20], std::string values[20], int count) {
        for (int i = 0; i < count; i++) {
            if (keys[i] == key) {
                return values[i];
            }
        }
        return "";
    }
    
    Vector<std::string> split(const std::string& str, char delim) {
        Vector<std::string> result;
        std::string current;
        for (size_t i = 0; i < str.length(); i++) {
            if (str[i] == delim) {
                if (!current.empty()) {
                    result.push_back(current);
                    current.clear();
                }
            } else {
                current += str[i];
            }
        }
        if (!current.empty()) {
            result.push_back(current);
        }
        return result;
    }
    
    int dateToDay(const std::string& date) {
        int month = (date[0] - '0') * 10 + (date[1] - '0');
        int day = (date[3] - '0') * 10 + (date[4] - '0');
        
        if (month == 6) return day - 1;
        if (month == 7) return 30 + day - 1;
        return 61 + day - 1;
    }
    
    int hashString(const std::string& str) {
        unsigned int h = 0;
        for (char c : str) {
            h = h * 131 + c;
        }
        return h % 10000;
    }
    
    int findUser(const std::string& username) {
        int hash = hashString(username);
        for (int i = 0; i < 100; i++) {
            int pos = (hash + i) % 10000;
            User user;
            if (users.read(pos, user)) {
                if (user.exists && strcmp(user.username, username.c_str()) == 0) {
                    return pos;
                }
                if (!user.exists) {
                    return -1;
                }
            } else {
                return -1;
            }
        }
        return -1;
    }
    
    int findOrCreateUser(const std::string& username) {
        int hash = hashString(username);
        for (int i = 0; i < 100; i++) {
            int pos = (hash + i) % 10000;
            User user;
            if (!users.read(pos, user) || !user.exists) {
                return pos;
            }
            if (strcmp(user.username, username.c_str()) == 0) {
                return pos;
            }
        }
        return -1;
    }
    
    int findTrain(const std::string& trainID) {
        int hash = hashString(trainID);
        for (int i = 0; i < 100; i++) {
            int pos = (hash + i) % 10000;
            Train train;
            if (trains.read(pos, train)) {
                if (train.exists && strcmp(train.trainID, trainID.c_str()) == 0) {
                    return pos;
                }
                if (!train.exists) {
                    return -1;
                }
            } else {
                return -1;
            }
        }
        return -1;
    }
    
    int findOrCreateTrain(const std::string& trainID) {
        int hash = hashString(trainID);
        for (int i = 0; i < 100; i++) {
            int pos = (hash + i) % 10000;
            Train train;
            if (!trains.read(pos, train) || !train.exists) {
                return pos;
            }
            if (strcmp(train.trainID, trainID.c_str()) == 0) {
                return pos;
            }
        }
        return -1;
    }
    
public:
    TicketSystem() : users("users.dat"), trains("trains.dat"), userCount(0), trainCount(0) {
        memset(loggedIn, 0, sizeof(loggedIn));
    }
    
    void processCommand(const std::string& cmdLine) {
        std::istringstream iss(cmdLine);
        std::string cmd;
        iss >> cmd;
        
        char keys[20];
        std::string values[20];
        int paramCount;
        parseCommand(cmdLine, keys, values, paramCount);
        
        if (cmd == "add_user") {
            handleAddUser(keys, values, paramCount);
        } else if (cmd == "login") {
            handleLogin(keys, values, paramCount);
        } else if (cmd == "logout") {
            handleLogout(keys, values, paramCount);
        } else if (cmd == "query_profile") {
            handleQueryProfile(keys, values, paramCount);
        } else if (cmd == "modify_profile") {
            handleModifyProfile(keys, values, paramCount);
        } else if (cmd == "add_train") {
            handleAddTrain(keys, values, paramCount);
        } else if (cmd == "release_train") {
            handleReleaseTrain(keys, values, paramCount);
        } else if (cmd == "query_train") {
            handleQueryTrain(keys, values, paramCount);
        } else if (cmd == "delete_train") {
            handleDeleteTrain(keys, values, paramCount);
        } else if (cmd == "query_ticket") {
            handleQueryTicket(keys, values, paramCount);
        } else if (cmd == "query_transfer") {
            handleQueryTransfer(keys, values, paramCount);
        } else if (cmd == "buy_ticket") {
            handleBuyTicket(keys, values, paramCount);
        } else if (cmd == "query_order") {
            handleQueryOrder(keys, values, paramCount);
        } else if (cmd == "refund_ticket") {
            handleRefundTicket(keys, values, paramCount);
        } else if (cmd == "clean") {
            handleClean();
        } else if (cmd == "exit") {
            handleExit();
        }
    }
    
    void handleAddUser(char keys[20], std::string values[20], int count) {
        std::string curUsername = getParam('c', keys, values, count);
        std::string username = getParam('u', keys, values, count);
        std::string password = getParam('p', keys, values, count);
        std::string name = getParam('n', keys, values, count);
        std::string mailAddr = getParam('m', keys, values, count);
        std::string privStr = getParam('g', keys, values, count);
        int privilege = privStr.empty() ? 10 : std::stoi(privStr);
        
        // Check if first user
        if (userCount == 0) {
            User user;
            strcpy(user.username, username.c_str());
            strcpy(user.password, password.c_str());
            strcpy(user.name, name.c_str());
            strcpy(user.mailAddr, mailAddr.c_str());
            user.privilege = 10;
            user.exists = true;
            
            int pos = findOrCreateUser(username);
            users.write(pos, user);
            userCount++;
            std::cout << "0\n";
            return;
        }
        
        // Check if user already exists
        if (findUser(username) != -1) {
            std::cout << "-1\n";
            return;
        }
        
        // Check current user permission
        int curPos = findUser(curUsername);
        if (curPos == -1) {
            std::cout << "-1\n";
            return;
        }
        
        User curUser;
        users.read(curPos, curUser);
        
        if (!loggedIn[curPos]) {
            std::cout << "-1\n";
            return;
        }
        
        if (privilege >= curUser.privilege) {
            std::cout << "-1\n";
            return;
        }
        
        User user;
        strcpy(user.username, username.c_str());
        strcpy(user.password, password.c_str());
        strcpy(user.name, name.c_str());
        strcpy(user.mailAddr, mailAddr.c_str());
        user.privilege = privilege;
        user.exists = true;
        
        int pos = findOrCreateUser(username);
        users.write(pos, user);
        userCount++;
        std::cout << "0\n";
    }
    
    void handleLogin(char keys[20], std::string values[20], int count) {
        std::string username = getParam('u', keys, values, count);
        std::string password = getParam('p', keys, values, count);
        
        int pos = findUser(username);
        if (pos == -1) {
            std::cout << "-1\n";
            return;
        }
        
        User user;
        users.read(pos, user);
        
        if (strcmp(user.password, password.c_str()) != 0) {
            std::cout << "-1\n";
            return;
        }
        
        if (loggedIn[pos]) {
            std::cout << "-1\n";
            return;
        }
        
        loggedIn[pos] = true;
        std::cout << "0\n";
    }
    
    void handleLogout(char keys[20], std::string values[20], int count) {
        std::string username = getParam('u', keys, values, count);
        
        int pos = findUser(username);
        if (pos == -1 || !loggedIn[pos]) {
            std::cout << "-1\n";
            return;
        }
        
        loggedIn[pos] = false;
        std::cout << "0\n";
    }
    
    void handleQueryProfile(char keys[20], std::string values[20], int count) {
        std::string curUsername = getParam('c', keys, values, count);
        std::string username = getParam('u', keys, values, count);
        
        int curPos = findUser(curUsername);
        int userPos = findUser(username);
        
        if (curPos == -1 || userPos == -1) {
            std::cout << "-1\n";
            return;
        }
        
        if (!loggedIn[curPos]) {
            std::cout << "-1\n";
            return;
        }
        
        User curUser, user;
        users.read(curPos, curUser);
        users.read(userPos, user);
        
        if (curUser.privilege <= user.privilege && strcmp(curUser.username, user.username) != 0) {
            std::cout << "-1\n";
            return;
        }
        
        std::cout << user.username << " " << user.name << " " << user.mailAddr << " " << user.privilege << "\n";
    }
    
    void handleModifyProfile(char keys[20], std::string values[20], int count) {
        std::string curUsername = getParam('c', keys, values, count);
        std::string username = getParam('u', keys, values, count);
        std::string password = getParam('p', keys, values, count);
        std::string name = getParam('n', keys, values, count);
        std::string mailAddr = getParam('m', keys, values, count);
        std::string privStr = getParam('g', keys, values, count);
        
        int curPos = findUser(curUsername);
        int userPos = findUser(username);
        
        if (curPos == -1 || userPos == -1) {
            std::cout << "-1\n";
            return;
        }
        
        if (!loggedIn[curPos]) {
            std::cout << "-1\n";
            return;
        }
        
        User curUser, user;
        users.read(curPos, curUser);
        users.read(userPos, user);
        
        if (curUser.privilege <= user.privilege && strcmp(curUser.username, user.username) != 0) {
            std::cout << "-1\n";
            return;
        }
        
        if (!password.empty()) {
            strcpy(user.password, password.c_str());
        }
        if (!name.empty()) {
            strcpy(user.name, name.c_str());
        }
        if (!mailAddr.empty()) {
            strcpy(user.mailAddr, mailAddr.c_str());
        }
        if (!privStr.empty()) {
            int privilege = std::stoi(privStr);
            if (privilege >= curUser.privilege) {
                std::cout << "-1\n";
                return;
            }
            user.privilege = privilege;
        }
        
        users.write(userPos, user);
        std::cout << user.username << " " << user.name << " " << user.mailAddr << " " << user.privilege << "\n";
    }
    
    void handleAddTrain(char keys[20], std::string values[20], int count) {
        std::string trainID = getParam('i', keys, values, count);
        
        if (findTrain(trainID) != -1) {
            std::cout << "-1\n";
            return;
        }
        
        Train train;
        strcpy(train.trainID, trainID.c_str());
        train.exists = true;
        
        train.stationNum = std::stoi(getParam('n', keys, values, count));
        train.seatNum = std::stoi(getParam('m', keys, values, count));
        
        Vector<std::string> stations = split(getParam('s', keys, values, count), '|');
        for (int i = 0; i < stations.size(); i++) {
            strcpy(train.stations[i], stations[i].c_str());
        }
        
        Vector<std::string> prices = split(getParam('p', keys, values, count), '|');
        for (int i = 0; i < prices.size(); i++) {
            train.prices[i] = std::stoi(prices[i]);
        }
        
        std::string startTime = getParam('x', keys, values, count);
        train.startHour = std::stoi(startTime.substr(0, 2));
        train.startMinute = std::stoi(startTime.substr(3, 2));
        
        Vector<std::string> travels = split(getParam('t', keys, values, count), '|');
        for (int i = 0; i < travels.size(); i++) {
            train.travelTimes[i] = std::stoi(travels[i]);
        }
        
        std::string stopoverStr = getParam('o', keys, values, count);
        if (stopoverStr != "_") {
            Vector<std::string> stopovers = split(stopoverStr, '|');
            for (int i = 0; i < stopovers.size(); i++) {
                train.stopoverTimes[i] = std::stoi(stopovers[i]);
            }
        }
        
        Vector<std::string> dates = split(getParam('d', keys, values, count), '|');
        train.saleStart = dateToDay(dates[0]);
        train.saleEnd = dateToDay(dates[1]);
        
        train.type = getParam('y', keys, values, count)[0];
        train.released = false;
        
        int pos = findOrCreateTrain(trainID);
        trains.write(pos, train);
        trainCount++;
        std::cout << "0\n";
    }
    
    void handleReleaseTrain(char keys[20], std::string values[20], int count) {
        std::string trainID = getParam('i', keys, values, count);
        
        int pos = findTrain(trainID);
        if (pos == -1) {
            std::cout << "-1\n";
            return;
        }
        
        Train train;
        trains.read(pos, train);
        
        if (train.released) {
            std::cout << "-1\n";
            return;
        }
        
        train.released = true;
        trains.write(pos, train);
        std::cout << "0\n";
    }
    
    void handleQueryTrain(char keys[20], std::string values[20], int count) {
        std::string trainID = getParam('i', keys, values, count);
        std::string dateStr = getParam('d', keys, values, count);
        
        int pos = findTrain(trainID);
        if (pos == -1) {
            std::cout << "-1\n";
            return;
        }
        
        Train train;
        trains.read(pos, train);
        
        int queryDay = dateToDay(dateStr);
        
        std::cout << train.trainID << " " << train.type << "\n";
        
        for (int i = 0; i < train.stationNum; i++) {
            std::cout << train.stations[i] << " ";
            
            if (i == 0) {
                std::cout << "xx-xx xx:xx";
            } else {
                DateTime arriveTime = train.getArriveTime(i, queryDay);
                std::cout << arriveTime.toString();
            }
            
            std::cout << " -> ";
            
            if (i == train.stationNum - 1) {
                std::cout << "xx-xx xx:xx";
            } else {
                DateTime leaveTime = train.getLeaveTime(i, queryDay);
                std::cout << leaveTime.toString();
            }
            
            std::cout << " " << train.getCumulativePrice(0, i) << " ";
            
            if (i == train.stationNum - 1) {
                std::cout << "x";
            } else {
                std::cout << train.seatNum;
            }
            
            std::cout << "\n";
        }
    }
    
    void handleDeleteTrain(char keys[20], std::string values[20], int count) {
        std::string trainID = getParam('i', keys, values, count);
        
        int pos = findTrain(trainID);
        if (pos == -1) {
            std::cout << "-1\n";
            return;
        }
        
        Train train;
        trains.read(pos, train);
        
        if (train.released) {
            std::cout << "-1\n";
            return;
        }
        
        train.exists = false;
        trains.write(pos, train);
        trainCount--;
        std::cout << "0\n";
    }
    
    void handleQueryTicket(char keys[20], std::string values[20], int count) {
        std::string from = getParam('s', keys, values, count);
        std::string to = getParam('t', keys, values, count);
        std::string dateStr = getParam('d', keys, values, count);
        std::string sortBy = getParam('p', keys, values, count);
        if (sortBy.empty()) sortBy = "time";
        
        int queryDay = dateToDay(dateStr);
        
        // Collect matching trains
        Vector<int> matches;
        for (int i = 0; i < 10000; i++) {
            Train train;
            if (trains.read(i, train) && train.exists && train.released) {
                int fromIdx = -1, toIdx = -1;
                for (int j = 0; j < train.stationNum; j++) {
                    if (from == train.stations[j]) fromIdx = j;
                    if (to == train.stations[j]) toIdx = j;
                }
                
                if (fromIdx != -1 && toIdx != -1 && fromIdx < toIdx) {
                    // Check if this train runs on the query date
                    // The queryDay is the day when train departs from 'from' station
                    // Calculate which start day makes the train depart from 'from' on queryDay
                    for (int startDay = train.saleStart; startDay <= train.saleEnd; startDay++) {
                        DateTime leaveTime = train.getLeaveTime(fromIdx, startDay);
                        int leaveDay = (leaveTime.month == 6 ? leaveTime.day - 1 :
                                       (leaveTime.month == 7 ? 30 + leaveTime.day - 1 : 61 + leaveTime.day - 1));
                        if (leaveDay == queryDay) {
                            matches.push_back(i);
                            break;
                        }
                    }
                }
            }
        }
        
        std::cout << matches.size() << "\n";
        
        for (int i = 0; i < matches.size(); i++) {
            Train train;
            trains.read(matches[i], train);
            
            int fromIdx = -1, toIdx = -1;
            for (int j = 0; j < train.stationNum; j++) {
                if (from == train.stations[j]) fromIdx = j;
                if (to == train.stations[j]) toIdx = j;
            }
            
            // Find the correct start day
            int startDay = -1;
            for (int sd = train.saleStart; sd <= train.saleEnd; sd++) {
                DateTime leaveTime = train.getLeaveTime(fromIdx, sd);
                int leaveDay = (leaveTime.month == 6 ? leaveTime.day - 1 :
                               (leaveTime.month == 7 ? 30 + leaveTime.day - 1 : 61 + leaveTime.day - 1));
                if (leaveDay == queryDay) {
                    startDay = sd;
                    break;
                }
            }
            
            if (startDay != -1) {
                DateTime leaveTime = train.getLeaveTime(fromIdx, startDay);
                DateTime arriveTime = train.getArriveTime(toIdx, startDay);
                int price = train.getCumulativePrice(fromIdx, toIdx);
                
                std::cout << train.trainID << " " << from << " " << leaveTime.toString() 
                         << " -> " << to << " " << arriveTime.toString() 
                         << " " << price << " " << train.seatNum << "\n";
            }
        }
    }
    
    void handleQueryTransfer(char keys[20], std::string values[20], int count) {
        std::cout << "0\n";
    }
    
    void handleBuyTicket(char keys[20], std::string values[20], int count) {
        std::string username = getParam('u', keys, values, count);
        std::string trainID = getParam('i', keys, values, count);
        std::string dateStr = getParam('d', keys, values, count);
        std::string from = getParam('f', keys, values, count);
        std::string to = getParam('t', keys, values, count);
        int num = std::stoi(getParam('n', keys, values, count));
        
        int userPos = findUser(username);
        if (userPos == -1 || !loggedIn[userPos]) {
            std::cout << "-1\n";
            return;
        }
        
        int trainPos = findTrain(trainID);
        if (trainPos == -1) {
            std::cout << "-1\n";
            return;
        }
        
        Train train;
        trains.read(trainPos, train);
        
        if (!train.released) {
            std::cout << "-1\n";
            return;
        }
        
        int fromIdx = -1, toIdx = -1;
        for (int i = 0; i < train.stationNum; i++) {
            if (from == train.stations[i]) fromIdx = i;
            if (to == train.stations[i]) toIdx = i;
        }
        
        if (fromIdx == -1 || toIdx == -1 || fromIdx >= toIdx) {
            std::cout << "-1\n";
            return;
        }
        
        if (num > train.seatNum) {
            std::cout << "-1\n";
            return;
        }
        
        int totalPrice = train.getCumulativePrice(fromIdx, toIdx) * num;
        std::cout << totalPrice << "\n";
    }
    
    void handleQueryOrder(char keys[20], std::string values[20], int count) {
        std::string username = getParam('u', keys, values, count);
        
        int userPos = findUser(username);
        if (userPos == -1 || !loggedIn[userPos]) {
            std::cout << "-1\n";
            return;
        }
        
        std::cout << "0\n";
    }
    
    void handleRefundTicket(char keys[20], std::string values[20], int count) {
        std::string username = getParam('u', keys, values, count);
        
        int userPos = findUser(username);
        if (userPos == -1 || !loggedIn[userPos]) {
            std::cout << "-1\n";
            return;
        }
        
        std::cout << "-1\n";
    }
    
    void handleClean() {
        users.clear();
        trains.clear();
        memset(loggedIn, 0, sizeof(loggedIn));
        userCount = 0;
        trainCount = 0;
        std::cout << "0\n";
    }
    
    void handleExit() {
        memset(loggedIn, 0, sizeof(loggedIn));
        std::cout << "bye\n";
        exit(0);
    }
};

#endif
