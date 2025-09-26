#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

// 定义日期
const string DAYS[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const int NUM_DAYS = 7;
const int FLOORS = 5;
const int ROWS = 4;
const int COLS = 4;

// 定义座位状态
const char EMPTY = '0';       // 空闲
const char RESERVED = '1';    // 已预约
const char CURRENT_USER = '2'; // 当前用户预约

// 定义文件路径
const string DATA_FILE = "library_data.txt";

// 座位数据结构
struct Seat {
    char status;       // 座位状态
    char user;         // 预约用户
};

// 全局变量
vector<vector<vector<vector<Seat>>>> library(NUM_DAYS, vector<vector<vector<Seat>>>(FLOORS, vector<vector<Seat>>(ROWS, vector<Seat>(COLS))));
char currentUser = ' ';       // 当前登录用户
bool isAdmin = false;         // 是否为管理员

// 初始化座位
void initializeLibrary() {
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    library[d][f][r][c].status = EMPTY;
                    library[d][f][r][c].user = ' ';
                }
            }
        }
    }
}

// 保存数据到文件
void saveData() {
    ofstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "ERROR: Failed to save data." << endl;
        return;
    }

    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    file << library[d][f][r][c].status << library[d][f][r][c].user << endl;
                }
            }
        }
    }

    file.close();
}

// 从文件加载数据
void loadData() {
    ifstream file(DATA_FILE);
    if (!file.is_open()) {
        // 如果文件不存在，初始化数据
        initializeLibrary();
        return;
    }

    string line;
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (getline(file, line)) {
                        if (line.length() >= 2) {
                            library[d][f][r][c].status = line[0];
                            library[d][f][r][c].user = line[1];
                        }
                    }
                }
            }
        }
    }

    file.close();
}

// 检查用户名是否只包含字母
bool isValidUsername(const string &username) {
    for (char c : username) {
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

// 登录功能
void login() {
    string username;
    cout << "Please enter username" << endl;
    cin >> username;
    // 清除输入缓冲区中的换行符，防止getline读取到空行
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    if (!isValidUsername(username)) {
        cout << "ERROR:ALPHABETS ONLY!" << endl;
        return;
    }

    if (username == "Admin") {
        string password;
        cout << "Please enter password" << endl;
        cin >> password;
        // 清除输入缓冲区中的换行符
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (password == "666") {
            currentUser = 'A'; // 管理员用A表示
            isAdmin = true;
            cout << "Login successful." << endl;
        } else {
            cout << "Incorrect password, login failed." << endl;
        }
    } else {
        if (username.length() == 1 && isalpha(username[0])) {
            currentUser = toupper(username[0]);
            isAdmin = false;
            cout << "Login successful." << endl;
        } else {
            cout << "Invalid username format, should be a single letter." << endl;
        }
    }
}

// 退出登录
void exitLogin() {
    currentUser = ' ';
    isAdmin = false;
    cout << "Logged out." << endl;
    // 保存数据
    saveData();
}

// 查找日期对应的索引
int getDayIndex(const string &day) {
    for (int i = 0; i < NUM_DAYS; i++) {
        if (DAYS[i] == day) {
            return i;
        }
    }
    return -1;
}

// 显示某一天某一层的座位情况
void showSeats(const string &day, int floor) {
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid day or floor." << endl;
        return;
    }

    floor--; // 转换为0-based索引

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (isAdmin) {
                // 管理员可以看到所有用户的预约信息
                if (library[dayIndex][floor][r][c].status == RESERVED) {
                    cout << library[dayIndex][floor][r][c].user;
                } else {
                    cout << library[dayIndex][floor][r][c].status;
                }
            } else {
                // 普通用户只能看到自己的预约和空闲/已预约状态
                if (library[dayIndex][floor][r][c].user == currentUser) {
                    cout << CURRENT_USER;
                } else {
                    cout << library[dayIndex][floor][r][c].status;
                }
            }
        }
        cout << endl;
    }
}

// 预约座位
void reserveSeat(const string &day, int floor, int row, int col) {
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS || row < 1 || row > ROWS || col < 1 || col > COLS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }

    floor--; row--; col--; // 转换为0-based索引

    if (library[dayIndex][floor][row][col].status != EMPTY) {
        cout << "ERROR" << endl;
        return;
    }

    library[dayIndex][floor][row][col].status = RESERVED;
    library[dayIndex][floor][row][col].user = currentUser;
    cout << "OK" << endl;
    
    // 保存数据
    saveData();
}

// 显示当前用户的预约
void showReservations() {
    bool hasReservation = false;
    
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (library[d][f][r][c].user == currentUser) {
                        cout << DAYS[d] << " Floor " << (f + 1) << " Seat " << (r + 1) << " " << (c + 1) << endl;
                        hasReservation = true;
                    }
                }
            }
        }
    }
    
    if (!hasReservation) {
        cout << "No reservations." << endl;
    }
}

// 清空所有数据
void clearAllData() {
    initializeLibrary();
    saveData();
    cout << "All data cleared." << endl;
}

// 清空指定用户的数据
void clearUserData(char user) {
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (library[d][f][r][c].user == user) {
                        library[d][f][r][c].status = EMPTY;
                        library[d][f][r][c].user = ' ';
                    }
                }
            }
        }
    }
    saveData();
    cout << "User " << user << "'s data cleared." << endl;
}

// 解析并执行命令
void executeCommand(const string &command) {
    // 重新加载数据以确保同步
    loadData();
    
    bool commandHandled = false;
    
    if (command == "Login") {
        login();
        commandHandled = true;
    } else if (command == "Exit") {
        exitLogin();
        commandHandled = true;
    } else if (command == "Quit") {
        // 程序将在主函数中退出
        commandHandled = true;
    } else if (currentUser == ' ') {
        cout << "Please login first." << endl;
        commandHandled = true;
    } else {
        // 处理登录后的命令
        if (command.substr(0, 6) == "Monday" || command.substr(0, 7) == "Tuesday" || 
            command.substr(0, 9) == "Wednesday" || command.substr(0, 8) == "Thursday" || 
            command.substr(0, 6) == "Friday" || command.substr(0, 8) == "Saturday" || 
            command.substr(0, 6) == "Sunday") {
            // 显示座位命令
            size_t floorPos = command.find("Floor");
            if (floorPos != string::npos) {
                string day = command.substr(0, floorPos - 1);
                int floor = stoi(command.substr(floorPos + 5));
                showSeats(day, floor);
                commandHandled = true;
            } else {
                cout << "ERROR" << endl;
                commandHandled = true;
            }
        } else if (command.substr(0, 7) == "Reserve") {
            // 预约座位命令
            size_t dayPos = command.find(" ") + 1;
            size_t floorPos = command.find("Floor");
            size_t seatPos = command.find("Seat");
            
            if (dayPos != string::npos && floorPos != string::npos && seatPos != string::npos) {
                string day = command.substr(dayPos, floorPos - dayPos - 1);
                int floor = stoi(command.substr(floorPos + 5, seatPos - floorPos - 6));
                
                size_t spacePos = command.find(" ", seatPos + 5);
                if (spacePos != string::npos) {
                    try {
                        int row = stoi(command.substr(seatPos + 5, spacePos - seatPos - 5));
                        int col = stoi(command.substr(spacePos + 1));
                        reserveSeat(day, floor, row, col);
                        commandHandled = true;
                    } catch (...) {
                        cout << "ERROR" << endl;
                        commandHandled = true;
                    }
                } else {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            } else {
                cout << "ERROR" << endl;
                commandHandled = true;
            }
        } else if (command == "Reservation") {
            showReservations();
            commandHandled = true;
        } else if (command == "Clear" && isAdmin) {
            clearAllData();
            commandHandled = true;
        } else if (command.substr(0, 6) == "Clear " && isAdmin) {
            char user = toupper(command[6]);
            if (isalpha(user)) {
                clearUserData(user);
                commandHandled = true;
            } else {
                cout << "ERROR" << endl;
                commandHandled = true;
            }
        } else {
            // 对于登录后不符合任何已知格式的命令，输出ERROR
            cout << "ERROR" << endl;
            commandHandled = true;
        }
    }
    
    // 如果命令没有被处理，输出ERROR
    if (!commandHandled && command != "Quit") {
        cout << "ERROR" << endl;
    }
}

int main() {
    // 加载数据
    loadData();
    
    string command;
    while (true) {
        cout << "Please enter command: ";
        getline(cin, command);
        
        // 去除命令前后的空格
        size_t start = command.find_first_not_of(" ");
        size_t end = command.find_last_not_of(" ");
        if (start != string::npos && end != string::npos) {
            command = command.substr(start, end - start + 1);
        } else {
            cout << "ERROR" << endl;
            continue;
        }
        
        if (command == "Quit") {
            break;
        }
        
        executeCommand(command);
    }
    
    // 保存数据并退出
    saveData();
    cout << "Program exited." << endl;
    return 0;
}