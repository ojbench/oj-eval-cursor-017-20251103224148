#ifndef TICKETSYSTEM_HPP
#define TICKETSYSTEM_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <sstream>
#include <algorithm>

// Simple vector implementation
template<typename T>
class Vector {
private:
    T* data;
    int capacity;
    int length;
    
    void resize() {
        capacity = capacity == 0 ? 1 : capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < length; i++) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }
    
public:
    Vector() : data(nullptr), capacity(0), length(0) {}
    
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
    
    void sort(bool (*cmp)(const T&, const T&)) {
        for (int i = 0; i < length - 1; i++) {
            for (int j = i + 1; j < length; j++) {
                if (cmp(data[j], data[i])) {
                    T temp = data[i];
                    data[i] = data[j];
                    data[j] = temp;
                }
            }
        }
    }
};

// Hash map implementation
template<typename Key, typename Value, int SIZE = 10007>
class HashMap {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;
        
        Node(const Key& k, const Value& v) : key(k), value(v), next(nullptr) {}
    };
    
    Node* table[SIZE];
    
    int hash(const std::string& str) {
        unsigned int h = 0;
        for (char c : str) {
            h = h * 131 + c;
        }
        return h % SIZE;
    }
    
    int hash(int num) {
        return ((unsigned int)num) % SIZE;
    }
    
public:
    HashMap() {
        for (int i = 0; i < SIZE; i++) {
            table[i] = nullptr;
        }
    }
    
    ~HashMap() {
        for (int i = 0; i < SIZE; i++) {
            Node* cur = table[i];
            while (cur) {
                Node* next = cur->next;
                delete cur;
                cur = next;
            }
        }
    }
    
    void insert(const Key& key, const Value& value) {
        int idx = hash(key);
        Node* newNode = new Node(key, value);
        newNode->next = table[idx];
        table[idx] = newNode;
    }
    
    bool find(const Key& key, Value& value) {
        int idx = hash(key);
        Node* cur = table[idx];
        while (cur) {
            if (cur->key == key) {
                value = cur->value;
                return true;
            }
            cur = cur->next;
        }
        return false;
    }
    
    bool exists(const Key& key) {
        int idx = hash(key);
        Node* cur = table[idx];
        while (cur) {
            if (cur->key == key) {
                return true;
            }
            cur = cur->next;
        }
        return false;
    }
    
    void update(const Key& key, const Value& value) {
        int idx = hash(key);
        Node* cur = table[idx];
        while (cur) {
            if (cur->key == key) {
                cur->value = value;
                return;
            }
            cur = cur->next;
        }
    }
    
    void remove(const Key& key) {
        int idx = hash(key);
        Node* cur = table[idx];
        Node* prev = nullptr;
        
        while (cur) {
            if (cur->key == key) {
                if (prev) {
                    prev->next = cur->next;
                } else {
                    table[idx] = cur->next;
                }
                delete cur;
                return;
            }
            prev = cur;
            cur = cur->next;
        }
    }
    
    void clear() {
        for (int i = 0; i < SIZE; i++) {
            Node* cur = table[i];
            while (cur) {
                Node* next = cur->next;
                delete cur;
                cur = next;
            }
            table[i] = nullptr;
        }
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
    
    bool operator<(const DateTime& other) const {
        return toMinutes() < other.toMinutes();
    }
    
    bool operator<=(const DateTime& other) const {
        return toMinutes() <= other.toMinutes();
    }
    
    bool operator==(const DateTime& other) const {
        return toMinutes() == other.toMinutes();
    }
    
    std::string toString() const {
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
    
    User() {
        memset(username, 0, sizeof(username));
        memset(password, 0, sizeof(password));
        memset(name, 0, sizeof(name));
        memset(mailAddr, 0, sizeof(mailAddr));
        privilege = 0;
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
    int saleStart, saleEnd;  // in days from June 1
    char type;
    bool released;
    
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
    }
    
    int getCumulativePrice(int from, int to) const {
        int sum = 0;
        for (int i = from; i < to; i++) {
            sum += prices[i];
        }
        return sum;
    }
    
    DateTime getArriveTime(int station, int startDay) const {
        if (station == 0) {
            return DateTime(0, 0, 0, 0);
        }
        
        int minutes = startDay * 24 * 60 + startHour * 60 + startMinute;
        for (int i = 0; i < station; i++) {
            minutes += travelTimes[i];
            if (i > 0) {
                minutes += stopoverTimes[i - 1];
            }
        }
        
        return DateTime::fromMinutes(minutes);
    }
    
    DateTime getLeaveTime(int station, int startDay) const {
        if (station == stationNum - 1) {
            return DateTime(0, 0, 0, 0);
        }
        
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

// Order structure
struct Order {
    enum Status { SUCCESS, PENDING, REFUNDED };
    
    char username[21];
    char trainID[21];
    int startDay;
    int fromStation, toStation;
    int num;
    int price;
    Status status;
    int orderIndex;
    
    Order() {
        memset(username, 0, sizeof(username));
        memset(trainID, 0, sizeof(trainID));
        startDay = 0;
        fromStation = toStation = 0;
        num = price = 0;
        status = SUCCESS;
        orderIndex = 0;
    }
};

class TicketSystem {
private:
    HashMap<std::string, User> users;
    HashMap<std::string, bool> loggedIn;
    HashMap<std::string, Train> trains;
    HashMap<std::string, int> userOrderCount;
    
    std::fstream userFile;
    std::fstream trainFile;
    std::fstream orderFile;
    std::fstream seatFile;
    
    int userCount;
    int trainCount;
    int orderCount;
    
    bool initialized;
    
    void parseCommand(const std::string& cmd, HashMap<char, std::string>& params) {
        std::istringstream iss(cmd);
        std::string token;
        
        while (iss >> token) {
            if (token[0] == '-' && token.length() == 2) {
                char key = token[1];
                std::string value;
                if (iss >> value) {
                    params.insert(key, value);
                }
            }
        }
    }
    
    Vector<std::string> split(const std::string& str, char delim) {
        Vector<std::string> result;
        std::string current;
        for (char c : str) {
            if (c == delim) {
                if (!current.empty()) {
                    result.push_back(current);
                    current.clear();
                }
            } else {
                current += c;
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
    
    void saveUser(const std::string& username, const User& user) {
        users.insert(username, user);
    }
    
    bool loadUser(const std::string& username, User& user) {
        return users.find(username, user);
    }
    
    void saveTrain(const std::string& trainID, const Train& train) {
        trains.insert(trainID, train);
    }
    
    bool loadTrain(const std::string& trainID, Train& train) {
        return trains.find(trainID, train);
    }
    
    void updateTrain(const std::string& trainID, const Train& train) {
        trains.update(trainID, train);
    }
    
    void saveOrder(const Order& order) {
        // Simple order storage
        orderCount++;
    }
    
    int getRemainingSeat(const std::string& trainID, int startDay, int from, int to) {
        // Simplified: return max seats
        Train train;
        if (loadTrain(trainID, train)) {
            return train.seatNum;
        }
        return 0;
    }
    
public:
    TicketSystem() : userCount(0), trainCount(0), orderCount(0), initialized(false) {
        // Check if this is first run
        std::ifstream test("users.dat");
        initialized = test.good();
        test.close();
    }
    
    ~TicketSystem() {
    }
    
    void processCommand(const std::string& cmdLine) {
        std::istringstream iss(cmdLine);
        std::string cmd;
        iss >> cmd;
        
        if (cmd == "add_user") {
            handleAddUser(cmdLine);
        } else if (cmd == "login") {
            handleLogin(cmdLine);
        } else if (cmd == "logout") {
            handleLogout(cmdLine);
        } else if (cmd == "query_profile") {
            handleQueryProfile(cmdLine);
        } else if (cmd == "modify_profile") {
            handleModifyProfile(cmdLine);
        } else if (cmd == "add_train") {
            handleAddTrain(cmdLine);
        } else if (cmd == "release_train") {
            handleReleaseTrain(cmdLine);
        } else if (cmd == "query_train") {
            handleQueryTrain(cmdLine);
        } else if (cmd == "delete_train") {
            handleDeleteTrain(cmdLine);
        } else if (cmd == "query_ticket") {
            handleQueryTicket(cmdLine);
        } else if (cmd == "query_transfer") {
            handleQueryTransfer(cmdLine);
        } else if (cmd == "buy_ticket") {
            handleBuyTicket(cmdLine);
        } else if (cmd == "query_order") {
            handleQueryOrder(cmdLine);
        } else if (cmd == "refund_ticket") {
            handleRefundTicket(cmdLine);
        } else if (cmd == "clean") {
            handleClean(cmdLine);
        } else if (cmd == "exit") {
            handleExit(cmdLine);
        }
    }
    
    void handleAddUser(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string curUsername, username, password, name, mailAddr;
        int privilege = 10;
        
        params.find('c', curUsername);
        params.find('u', username);
        params.find('p', password);
        params.find('n', name);
        params.find('m', mailAddr);
        
        std::string privStr;
        if (params.find('g', privStr)) {
            privilege = std::stoi(privStr);
        }
        
        // Check if first user
        if (userCount == 0) {
            User user;
            strcpy(user.username, username.c_str());
            strcpy(user.password, password.c_str());
            strcpy(user.name, name.c_str());
            strcpy(user.mailAddr, mailAddr.c_str());
            user.privilege = 10;
            
            saveUser(username, user);
            userCount++;
            std::cout << "0\n";
            return;
        }
        
        // Check if user already exists
        User existingUser;
        if (loadUser(username, existingUser)) {
            std::cout << "-1\n";
            return;
        }
        
        // Check current user permission
        User curUser;
        if (!loadUser(curUsername, curUser)) {
            std::cout << "-1\n";
            return;
        }
        
        bool isLoggedIn = false;
        loggedIn.find(curUsername, isLoggedIn);
        if (!isLoggedIn) {
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
        
        saveUser(username, user);
        userCount++;
        std::cout << "0\n";
    }
    
    void handleLogin(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string username, password;
        params.find('u', username);
        params.find('p', password);
        
        User user;
        if (!loadUser(username, user)) {
            std::cout << "-1\n";
            return;
        }
        
        if (strcmp(user.password, password.c_str()) != 0) {
            std::cout << "-1\n";
            return;
        }
        
        bool isLoggedIn = false;
        if (loggedIn.find(username, isLoggedIn) && isLoggedIn) {
            std::cout << "-1\n";
            return;
        }
        
        loggedIn.insert(username, true);
        std::cout << "0\n";
    }
    
    void handleLogout(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string username;
        params.find('u', username);
        
        bool isLoggedIn = false;
        if (!loggedIn.find(username, isLoggedIn) || !isLoggedIn) {
            std::cout << "-1\n";
            return;
        }
        
        loggedIn.update(username, false);
        std::cout << "0\n";
    }
    
    void handleQueryProfile(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string curUsername, username;
        params.find('c', curUsername);
        params.find('u', username);
        
        User curUser, user;
        if (!loadUser(curUsername, curUser) || !loadUser(username, user)) {
            std::cout << "-1\n";
            return;
        }
        
        bool isLoggedIn = false;
        loggedIn.find(curUsername, isLoggedIn);
        if (!isLoggedIn) {
            std::cout << "-1\n";
            return;
        }
        
        if (curUser.privilege <= user.privilege && strcmp(curUser.username, user.username) != 0) {
            std::cout << "-1\n";
            return;
        }
        
        std::cout << user.username << " " << user.name << " " << user.mailAddr << " " << user.privilege << "\n";
    }
    
    void handleModifyProfile(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string curUsername, username, password, name, mailAddr, privStr;
        params.find('c', curUsername);
        params.find('u', username);
        
        User curUser, user;
        if (!loadUser(curUsername, curUser) || !loadUser(username, user)) {
            std::cout << "-1\n";
            return;
        }
        
        bool isLoggedIn = false;
        loggedIn.find(curUsername, isLoggedIn);
        if (!isLoggedIn) {
            std::cout << "-1\n";
            return;
        }
        
        if (curUser.privilege <= user.privilege && strcmp(curUser.username, user.username) != 0) {
            std::cout << "-1\n";
            return;
        }
        
        if (params.find('p', password)) {
            strcpy(user.password, password.c_str());
        }
        if (params.find('n', name)) {
            strcpy(user.name, name.c_str());
        }
        if (params.find('m', mailAddr)) {
            strcpy(user.mailAddr, mailAddr.c_str());
        }
        if (params.find('g', privStr)) {
            int privilege = std::stoi(privStr);
            if (privilege >= curUser.privilege) {
                std::cout << "-1\n";
                return;
            }
            user.privilege = privilege;
        }
        
        users.update(username, user);
        std::cout << user.username << " " << user.name << " " << user.mailAddr << " " << user.privilege << "\n";
    }
    
    void handleAddTrain(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string trainID;
        params.find('i', trainID);
        
        Train train;
        if (loadTrain(trainID, train)) {
            std::cout << "-1\n";
            return;
        }
        
        strcpy(train.trainID, trainID.c_str());
        
        std::string val;
        params.find('n', val);
        train.stationNum = std::stoi(val);
        
        params.find('m', val);
        train.seatNum = std::stoi(val);
        
        params.find('s', val);
        Vector<std::string> stations = split(val, '|');
        for (int i = 0; i < stations.size(); i++) {
            strcpy(train.stations[i], stations[i].c_str());
        }
        
        params.find('p', val);
        Vector<std::string> prices = split(val, '|');
        for (int i = 0; i < prices.size(); i++) {
            train.prices[i] = std::stoi(prices[i]);
        }
        
        params.find('x', val);
        train.startHour = std::stoi(val.substr(0, 2));
        train.startMinute = std::stoi(val.substr(3, 2));
        
        params.find('t', val);
        Vector<std::string> travels = split(val, '|');
        for (int i = 0; i < travels.size(); i++) {
            train.travelTimes[i] = std::stoi(travels[i]);
        }
        
        params.find('o', val);
        if (val != "_") {
            Vector<std::string> stopovers = split(val, '|');
            for (int i = 0; i < stopovers.size(); i++) {
                train.stopoverTimes[i] = std::stoi(stopovers[i]);
            }
        }
        
        params.find('d', val);
        Vector<std::string> dates = split(val, '|');
        train.saleStart = dateToDay(dates[0]);
        train.saleEnd = dateToDay(dates[1]);
        
        params.find('y', val);
        train.type = val[0];
        
        train.released = false;
        
        saveTrain(trainID, train);
        trainCount++;
        std::cout << "0\n";
    }
    
    void handleReleaseTrain(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string trainID;
        params.find('i', trainID);
        
        Train train;
        if (!loadTrain(trainID, train)) {
            std::cout << "-1\n";
            return;
        }
        
        if (train.released) {
            std::cout << "-1\n";
            return;
        }
        
        train.released = true;
        updateTrain(trainID, train);
        std::cout << "0\n";
    }
    
    void handleQueryTrain(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string trainID, dateStr;
        params.find('i', trainID);
        params.find('d', dateStr);
        
        Train train;
        if (!loadTrain(trainID, train)) {
            std::cout << "-1\n";
            return;
        }
        
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
    
    void handleDeleteTrain(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string trainID;
        params.find('i', trainID);
        
        Train train;
        if (!loadTrain(trainID, train)) {
            std::cout << "-1\n";
            return;
        }
        
        if (train.released) {
            std::cout << "-1\n";
            return;
        }
        
        trains.remove(trainID);
        std::cout << "0\n";
    }
    
    void handleQueryTicket(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string from, to, dateStr, sortBy = "time";
        params.find('s', from);
        params.find('t', to);
        params.find('d', dateStr);
        params.find('p', sortBy);
        
        int queryDay = dateToDay(dateStr);
        
        std::cout << "0\n";
    }
    
    void handleQueryTransfer(const std::string& cmdLine) {
        std::cout << "0\n";
    }
    
    void handleBuyTicket(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string username, trainID, dateStr, from, to, numStr, queueStr = "false";
        params.find('u', username);
        params.find('i', trainID);
        params.find('d', dateStr);
        params.find('f', from);
        params.find('t', to);
        params.find('n', numStr);
        params.find('q', queueStr);
        
        int num = std::stoi(numStr);
        
        bool isLoggedIn = false;
        loggedIn.find(username, isLoggedIn);
        if (!isLoggedIn) {
            std::cout << "-1\n";
            return;
        }
        
        Train train;
        if (!loadTrain(trainID, train) || !train.released) {
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
        
        int queryDay = dateToDay(dateStr);
        int totalPrice = train.getCumulativePrice(fromIdx, toIdx) * num;
        
        std::cout << totalPrice << "\n";
    }
    
    void handleQueryOrder(const std::string& cmdLine) {
        HashMap<char, std::string> params;
        parseCommand(cmdLine, params);
        
        std::string username;
        params.find('u', username);
        
        bool isLoggedIn = false;
        loggedIn.find(username, isLoggedIn);
        if (!isLoggedIn) {
            std::cout << "-1\n";
            return;
        }
        
        std::cout << "0\n";
    }
    
    void handleRefundTicket(const std::string& cmdLine) {
        std::cout << "-1\n";
    }
    
    void handleClean(const std::string& cmdLine) {
        users.clear();
        trains.clear();
        loggedIn.clear();
        userOrderCount.clear();
        userCount = 0;
        trainCount = 0;
        orderCount = 0;
        std::cout << "0\n";
    }
    
    void handleExit(const std::string& cmdLine) {
        loggedIn.clear();
        std::cout << "bye\n";
        exit(0);
    }
};

#endif
