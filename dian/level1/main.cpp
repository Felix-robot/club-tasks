#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

// 定义日期常量数组
const string DAYS[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const int NUM_DAYS = 7;     // 一周天数
const int FLOORS = 5;       // 楼层数
const int ROWS = 4;         // 每层的行数
const int COLS = 4;         // 每层的列数

// 定义座位状态常量
const char EMPTY = '0';       // 空闲状态
const char RESERVED = '1';    // 已预约状态
const char CURRENT_USER = '2'; // 当前用户预约状态

// 定义数据文件路径
const string DATA_FILE = "library_data.txt";

// 座位数据结构
struct Seat {
    char status;       // 座位状态（空闲、已预约、当前用户预约）
    char user;         // 预约用户标识（单个字母）
};

// 全局变量
// 四维向量：[天数][楼层][行][列] 存储座位信息
vector<vector<vector<vector<Seat>>>> library(NUM_DAYS, vector<vector<vector<Seat>>>(FLOORS, vector<vector<Seat>>(ROWS, vector<Seat>(COLS))));
char currentUser = ' ';       // 当前登录用户（单个字母，管理员为'A'）
bool isAdmin = false;         // 是否为管理员用户

// 初始化座位库
// 将所有座位状态设置为空闲，用户标识设置为空
void initializeLibrary() {
    // 遍历所有天数、楼层、行和列
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    library[d][f][r][c].status = EMPTY;  // 设置为空闲状态
                    library[d][f][r][c].user = ' ';      // 清空用户标识
                }
            }
        }
    }
}

// 保存数据到文件
// 将所有座位信息写入到指定的数据文件中
void saveData() {
    // 打开文件用于写入
    ofstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "ERROR: Failed to save data." << endl;
        return;
    }

    // 遍历所有座位并将状态和用户信息写入文件
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    // 每行写入一个座位的状态和用户标识
                    file << library[d][f][r][c].status << library[d][f][r][c].user << endl;
                }
            }
        }
    }

    // 关闭文件
    file.close();
}

// 从文件加载数据
// 从指定的数据文件中读取座位信息
void loadData() {
    // 打开文件用于读取
    ifstream file(DATA_FILE);
    if (!file.is_open()) {
        // 如果文件不存在，初始化数据
        initializeLibrary();
        return;
    }

    string line;
    // 按顺序读取每个座位的信息
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (getline(file, line)) {
                        if (line.length() >= 2) {
                            // 读取座位状态和用户标识
                            library[d][f][r][c].status = line[0];
                            library[d][f][r][c].user = line[1];
                        }
                    }
                }
            }
        }
    }

    // 关闭文件
    file.close();
}

// 检查用户名是否只包含字母
// @param username 要检查的用户名
// @return 如果用户名只包含字母和空格，返回true，否则返回false
bool isValidUsername(const string &username) {
    for (char c : username) {
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

// 用户登录功能
// 支持普通用户和管理员两种登录模式
// - 管理员登录：用户名"Admin"，密码"666"
// - 普通用户登录：单个字母作为用户名
void login() {
    string username;
    cout << "Please enter username" << endl;
    cin >> username;
    // 清除输入缓冲区中的换行符，防止后续getline读取到空行
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // 检查用户名是否只包含字母
    if (!isValidUsername(username)) {
        cout << "ERROR:ALPHABETS ONLY!" << endl;
        return;
    }

    // 处理管理员登录
    if (username == "Admin") {
        string password;
        cout << "Please enter password" << endl;
        cin >> password;
        // 清除输入缓冲区中的换行符
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        // 验证管理员密码
        if (password == "666") {
            currentUser = 'A'; // 管理员用'A'表示
            isAdmin = true;
            cout << "Login successful." << endl;
        } else {
            cout << "Incorrect password, login failed." << endl;
        }
    } else {
        // 处理普通用户登录
        if (username.length() == 1 && isalpha(username[0])) {
            currentUser = toupper(username[0]); // 转换为大写字母
            isAdmin = false;
            cout << "Login successful." << endl;
        } else {
            cout << "Invalid username format, should be a single letter." << endl;
        }
    }
}

// 退出登录功能
// 重置当前用户信息并保存数据
void exitLogin() {
    currentUser = ' ';  // 清空当前用户
    isAdmin = false;    // 重置管理员状态
    cout << "Logged out." << endl;
    // 保存当前数据到文件
    saveData();
}

// 获取日期对应的索引值
// @param day 日期字符串（如"Monday"、"Tuesday"等）
// @return 日期对应的索引值，若未找到则返回-1
int getDayIndex(const string &day) {
    // 遍历日期数组查找匹配项
    for (int i = 0; i < NUM_DAYS; i++) {
        if (DAYS[i] == day) {
            return i; // 找到匹配的日期，返回索引
        }
    }
    return -1; // 未找到匹配的日期
}

// 显示指定日期和楼层的座位情况
// @param day 日期字符串
// @param floor 楼层号（1-based）
void showSeats(const string &day, int floor) {
    // 获取日期索引并进行参数验证
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid day or floor." << endl;
        return;
    }

    floor--; // 转换为0-based索引

    // 遍历显示该楼层的所有座位
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (isAdmin) {
                // 管理员视图：可以看到所有用户的预约信息
                if (library[dayIndex][floor][r][c].status == RESERVED) {
                    cout << library[dayIndex][floor][r][c].user; // 显示预约用户
                } else {
                    cout << library[dayIndex][floor][r][c].status; // 显示座位状态
                }
            } else {
                // 普通用户视图：只能看到自己的预约和空闲/已预约状态
                if (library[dayIndex][floor][r][c].user == currentUser) {
                    cout << CURRENT_USER; // 显示当前用户的预约
                } else {
                    cout << library[dayIndex][floor][r][c].status; // 显示其他座位状态
                }
            }
        }
        cout << endl; // 每行结束后换行
    }
}

// 预约座位功能
// @param day 日期字符串
// @param floor 楼层号（1-based）
// @param row 座位行号（1-based）
// @param col 座位列号（1-based）
void reserveSeat(const string &day, int floor, int row, int col) {
    // 获取日期索引并进行参数验证
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS || row < 1 || row > ROWS || col < 1 || col > COLS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }

    // 转换为0-based索引
    floor--; row--; col--;

    // 检查座位是否空闲
    if (library[dayIndex][floor][row][col].status != EMPTY) {
        cout << "ERROR" << endl;
        return;
    }

    // 设置座位为已预约状态，并记录当前用户
    library[dayIndex][floor][row][col].status = RESERVED;
    library[dayIndex][floor][row][col].user = currentUser;
    cout << "OK" << endl;
    
    // 保存数据到文件
    saveData();
}

// 显示当前用户的所有预约
void showReservations() {
    bool hasReservation = false;
    
    // 遍历所有座位，查找当前用户的预约
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (library[d][f][r][c].user == currentUser) {
                        // 显示预约信息（转换为1-based索引）
                        cout << DAYS[d] << " Floor " << (f + 1) << " Seat " << (r + 1) << " " << (c + 1) << endl;
                        hasReservation = true;
                    }
                }
            }
        }
    }
    
    // 如果没有找到预约，显示提示信息
    if (!hasReservation) {
        cout << "No reservations." << endl;
    }
}

// 清空所有座位数据（管理员功能）
// 将所有座位重置为初始状态
void clearAllData() {
    initializeLibrary(); // 重新初始化座位库
    saveData(); // 保存清空后的数据
    cout << "All data cleared." << endl;
}

// 清空指定用户的数据（管理员功能）
// @param user 要清空数据的用户标识
void clearUserData(char user) {
    // 遍历所有座位，查找并清空指定用户的预约
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    if (library[d][f][r][c].user == user) {
                        library[d][f][r][c].status = EMPTY; // 设置为空闲状态
                        library[d][f][r][c].user = ' ';     // 清空用户标识
                    }
                }
            }
        }
    }
    saveData(); // 保存更新后的数据
    cout << "User " << user << "'s data cleared." << endl;
}

// 解析并执行用户命令
// @param command 用户输入的命令字符串
void executeCommand(const string &command) {
    // 重新加载数据以确保同步
    loadData();
    
    bool commandHandled = false;
    
    // 处理通用命令（无需登录）
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
        // 未登录状态下的提示
        cout << "Please login first." << endl;
        commandHandled = true;
    } else {
        // 处理登录后的命令
        if (command.substr(0, 6) == "Monday" || command.substr(0, 7) == "Tuesday" || 
            command.substr(0, 9) == "Wednesday" || command.substr(0, 8) == "Thursday" || 
            command.substr(0, 6) == "Friday" || command.substr(0, 8) == "Saturday" || 
            command.substr(0, 6) == "Sunday") {
            // 处理显示座位命令（如："Monday Floor 2"）
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
            // 处理预约座位命令（如："Reserve Monday Floor 2 Seat 3 4"）
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
            // 处理显示预约命令
            showReservations();
            commandHandled = true;
        } else if (command == "Clear" && isAdmin) {
            // 处理清空所有数据命令（管理员功能）
            clearAllData();
            commandHandled = true;
        } else if (command.substr(0, 6) == "Clear " && isAdmin) {
            // 处理清空指定用户数据命令（管理员功能，如："Clear A"）
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

// 主函数
// 程序入口点，处理命令输入循环
int main() {
    // 加载现有数据
    loadData();
    
    string command;
    // 主命令循环
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
        
        // 退出程序条件
        if (command == "Quit") {
            break;
        }
        
        // 执行用户命令
        executeCommand(command);
    }
    
    // 保存数据并退出
    saveData();
    cout << "Program exited." << endl;
    return 0;
}