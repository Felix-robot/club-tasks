#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cctype>
#include <limits>
#include <sstream>

using namespace std;

// 定义日期常量数组
const string DAYS[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const int NUM_DAYS = 7;     // 一周天数

// 可变的楼层和座位数，以便管理员可以增加或删除座位
int FLOORS = 5;       // 楼层数（可由管理员调整）
int ROWS = 4;         // 每层的行数（可由管理员调整）
int COLS = 4;         // 每层的列数（可由管理员调整）

// 定义座位状态常量
const char EMPTY = '0';       // 空闲状态
const char RESERVED = '1';    // 已预约状态
const char CURRENT_USER = '2'; // 当前用户预约状态
const char UNAVAILABLE = 'X'; // 不可预约状态

// 定义数据文件路径
const string DATA_FILE = "library_data.txt";

// 座位数据结构
struct Seat {
    char status;       // 座位状态（空闲、已预约、当前用户预约、不可预约）
    char user;         // 预约用户标识（单个字母）
};

// 全局变量
vector<vector<vector<vector<Seat>>>> library;  // 四维向量：[天数][楼层][行][列] 存储座位信息
char currentUser = ' ';       // 当前登录用户（单个字母，管理员为'A'）
bool isAdmin = false;         // 是否为管理员用户

// 座位配置结构体
// 用于保存和管理图书馆座位的整体配置
struct SeatConfig {
    int floors;        // 楼层数
    int rows;          // 每层行数
    int cols;          // 每层列数
} seatConfig = {FLOORS, ROWS, COLS};

// 初始化座位库
// 根据当前配置调整座位库大小并设置所有座位为初始状态
void initializeLibrary() {
    // 重新调整library的四维向量大小
    library.resize(NUM_DAYS);
    for (int d = 0; d < NUM_DAYS; d++) {
        library[d].resize(FLOORS);
        for (int f = 0; f < FLOORS; f++) {
            library[d][f].resize(ROWS);
            for (int r = 0; r < ROWS; r++) {
                library[d][f][r].resize(COLS);
                for (int c = 0; c < COLS; c++) {
                    library[d][f][r][c].status = EMPTY;  // 设置为空闲状态
                    library[d][f][r][c].user = ' ';      // 清空用户标识
                }
            }
        }
    }
    // 更新座位配置结构体
    seatConfig.floors = FLOORS;
    seatConfig.rows = ROWS;
    seatConfig.cols = COLS;
}

// 保存数据到文件
// 将座位配置信息和所有座位数据写入到指定的数据文件中
void saveData() {
    // 打开文件用于写入
    ofstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "ERROR: Failed to save data." << endl;
        return;
    }

    // 首先保存座位配置信息（楼层数、行数、列数）
    file << FLOORS << " " << ROWS << " " << COLS << endl;

    // 然后保存所有座位的数据
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
// 从指定的数据文件中读取座位配置和座位信息
void loadData() {
    // 打开文件用于读取
    ifstream file(DATA_FILE);
    if (!file.is_open()) {
        // 如果文件不存在，初始化数据
        initializeLibrary();
        return;
    }

    string line;
    
    // 首先读取座位配置信息
    if (getline(file, line)) {
        istringstream iss(line);
        int floors, rows, cols;
        if (iss >> floors >> rows >> cols) {
            // 更新全局配置变量
            FLOORS = floors;
            ROWS = rows;
            COLS = cols;
        }
    }
    
    // 初始化library以适应新的大小
    initializeLibrary();

    // 然后按顺序读取每个座位的信息
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
// 参数: username - 待验证的用户名字符串
// 返回: 若用户名为纯字母（允许空格）则返回true，否则返回false
bool isValidUsername(const string &username) {
    for (char c : username) {
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

// 登录功能
// 允许用户输入用户名进行登录，支持普通用户和管理员登录
void login() {
    string username;
    cout << "Please enter username" << endl;
    cin >> username;
    // 清除输入缓冲区中的换行符，防止getline读取到空行
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    // 验证用户名是否只包含字母
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
        if (password == "666") {
            currentUser = 'A'; // 管理员用A表示
            isAdmin = true;
            cout << "Login successful." << endl;
        } else {
            cout << "Incorrect password, login failed." << endl;
        }
    } else {
        // 处理普通用户登录
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
// 清除当前用户的登录状态并保存数据
void exitLogin() {
    currentUser = ' ';
    isAdmin = false;
    cout << "Logged out." << endl;
    // 保存数据以确保所有更改被持久化
    saveData();
}

// 查找日期对应的索引
// 参数: day - 日期字符串
// 返回: 日期对应的索引，若日期无效则返回-1
int getDayIndex(const string &day) {
    // 遍历日期数组查找匹配的日期
    for (int i = 0; i < NUM_DAYS; i++) {
        if (DAYS[i] == day) {
            return i;  // 找到匹配的日期，返回索引
        }
    }
    return -1;  // 未找到匹配的日期，返回-1表示无效
}

// 显示某一天某一层的座位情况
// 参数: day - 要查询的日期
// 参数: floor - 要查询的楼层
void showSeats(const string &day, int floor) {
    // 获取日期索引并验证参数有效性
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid day or floor." << endl;
        return;
    }

    floor--; // 转换为0-based索引

    // 遍历显示每行每列的座位状态
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            // 所有用户都能看到不可预约状态
            if (library[dayIndex][floor][r][c].status == UNAVAILABLE) {
                cout << UNAVAILABLE;
            } else if (isAdmin) {
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
// 参数: day - 预约日期
// 参数: floor - 预约楼层
// 参数: row - 预约座位行号
// 参数: col - 预约座位列号
void reserveSeat(const string &day, int floor, int row, int col) {
    // 获取日期索引并验证所有参数有效性
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS || row < 1 || row > ROWS || col < 1 || col > COLS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }

    // 将1-based索引转换为0-based索引
    floor--; row--; col--;

    // 检查座位是否为空（可预约）
    if (library[dayIndex][floor][row][col].status != EMPTY) {
        cout << "ERROR" << endl;
        return;
    }

    // 检查用户在同一天是否已经预约了座位（同一用户同一天只能预约一个座位）
    for (int f = 0; f < FLOORS; f++) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                if (library[dayIndex][f][r][c].user == currentUser && library[dayIndex][f][r][c].status == RESERVED) {
                    // 如果已经有预约，则取消之前的预约
                    library[dayIndex][f][r][c].status = EMPTY;
                    library[dayIndex][f][r][c].user = ' ';
                    break;
                }
            }
        }
    }

    // 预约新座位
    library[dayIndex][floor][row][col].status = RESERVED;
    library[dayIndex][floor][row][col].user = currentUser;
    cout << "OK" << endl;
    
    // 保存数据
    saveData();
}

// 显示当前用户的预约
// 显示当前登录用户的所有座位预约信息
void showReservations() {
    bool hasReservation = false;  // 标记用户是否有预约
    
    // 遍历所有日期、楼层、行和列查找用户的预约
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < FLOORS; f++) {
            for (int r = 0; r < ROWS; r++) {
                for (int c = 0; c < COLS; c++) {
                    // 找到当前用户的预约
                    if (library[d][f][r][c].user == currentUser) {
                        // 显示预约信息：日期、楼层、座位位置
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

// 清空所有数据
// 重置所有座位状态并清空用户数据
void clearAllData() {
    // 重新初始化图书馆数据
    initializeLibrary();
    // 保存清空后的数据
    saveData();
    // 显示操作结果
    cout << "All data cleared." << endl;
}

// 管理员为用户预约座位
// 管理员专用功能：为指定用户预约座位
// 参数: day - 预约日期
// 参数: floor - 预约楼层
// 参数: row - 预约座位行号
// 参数: col - 预约座位列号
// 参数: user - 被预约的用户标识
void adminReserveSeat(const string &day, int floor, int row, int col, char user) {
    // 获取日期索引并验证所有参数有效性
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS || row < 1 || row > ROWS || col < 1 || col > COLS || !isalpha(user)) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }

    // 将1-based索引转换为0-based索引
    floor--; row--; col--;

    // 检查座位是否为空（可预约）
    if (library[dayIndex][floor][row][col].status != EMPTY) {
        cout << "ERROR" << endl;
        return;
    }

    // 检查指定用户在同一天是否已经预约了座位
    for (int f = 0; f < FLOORS; f++) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                if (library[dayIndex][f][r][c].user == user && library[dayIndex][f][r][c].status == RESERVED) {
                    // 如果已经有预约，则取消之前的预约
                    library[dayIndex][f][r][c].status = EMPTY;
                    library[dayIndex][f][r][c].user = ' ';
                    break;
                }
            }
        }
    }

    // 为指定用户预约新座位
    library[dayIndex][floor][row][col].status = RESERVED;
    library[dayIndex][floor][row][col].user = user;
    cout << "OK" << endl;
    
    // 保存数据
    saveData();
}

// 管理员取消用户预约
// 管理员专用功能：取消指定座位的预约
// 参数: day - 日期
// 参数: floor - 楼层
// 参数: row - 座位行号
// 参数: col - 座位列号
void adminCancelReservation(const string &day, int floor, int row, int col) {
    // 获取日期索引并验证所有参数有效性
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS || row < 1 || row > ROWS || col < 1 || col > COLS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }

    // 将1-based索引转换为0-based索引
    floor--; row--; col--;

    // 检查座位是否为空（没有预约可取消）
    if (library[dayIndex][floor][row][col].status == EMPTY) {
        cout << "ERROR" << endl;
        return;
    }

    library[dayIndex][floor][row][col].status = EMPTY;
    library[dayIndex][floor][row][col].user = ' ';
    cout << "OK" << endl;
    
    // 保存数据
    saveData();
}

// 增加或删除座位
// 管理员专用功能：动态调整图书馆的楼层数、行数和列数
// 参数: newFloors - 新的楼层数
// 参数: newRows - 每楼层新的行数
// 参数: newCols - 每行新的列数
void manageSeats(int newFloors, int newRows, int newCols) {
    // 验证参数有效性
    if (newFloors <= 0 || newRows <= 0 || newCols <= 0) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }
    
    // 保存当前数据到临时变量，以便在调整后恢复原有数据
    auto tempLibrary = library;
    int oldFloors = FLOORS;
    int oldRows = ROWS;
    int oldCols = COLS;
    
    // 更新全局座位数量配置
    FLOORS = newFloors;
    ROWS = newRows;
    COLS = newCols;
    
    // 重新初始化library以适应新的大小
    initializeLibrary();
    
    // 计算新旧大小的最小值，以确定可以恢复多少原有数据
    int minFloors = min(oldFloors, newFloors);
    int minRows = min(oldRows, newRows);
    int minCols = min(oldCols, newCols);
    
    // 恢复尽可能多的原有数据
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < minFloors; f++) {
            for (int r = 0; r < minRows; r++) {
                for (int c = 0; c < minCols; c++) {
                    library[d][f][r][c] = tempLibrary[d][f][r][c];
                }
            }
        }
    }
    
    // 显示操作结果并保存数据
    cout << "Seats updated successfully." << endl;
    saveData();
}

// 取消某一天所有人的预约
// 管理员专用功能：清空指定日期的所有预约
// 参数: day - 要清空预约的日期
void clearDayReservations(const string &day) {
    // 获取日期索引并验证
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1) {
        cout << "ERROR: Invalid day." << endl;
        return;
    }
    
    // 遍历该日期的所有楼层、行和列，清空所有座位的预约状态
    for (int f = 0; f < FLOORS; f++) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                library[dayIndex][f][r][c].status = EMPTY;
                library[dayIndex][f][r][c].user = ' ';
            }
        }
    }
    
    // 显示操作结果并保存数据
    cout << "All reservations for " << day << " cleared." << endl;
    saveData();
}

// 取消某一层楼的全部预约
// 管理员专用功能：清空指定楼层的所有预约
// 参数: floor - 要清空预约的楼层
void clearFloorReservations(int floor) {
    // 验证楼层号有效性
    if (floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid floor." << endl;
        return;
    }
    
    // 将1-based索引转换为0-based索引
    floor--;
    
    // 遍历该楼层的所有日期、行和列，清空所有座位的预约状态
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                library[d][floor][r][c].status = EMPTY;
                library[d][floor][r][c].user = ' ';
            }
        }
    }
    
    // 显示操作结果并保存数据
    cout << "All reservations for Floor " << (floor + 1) << " cleared." << endl;
    saveData();
}

// 设置某一天或某一层楼不可被预约
// 管理员专用功能：设置指定日期和楼层的所有座位为不可用状态
// 参数: day - 日期
// 参数: floor - 楼层
void setUnavailable(const string &day, int floor) {
    // 获取日期索引并验证参数有效性
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }
    
    // 将1-based索引转换为0-based索引
    floor--;
    
    // 遍历指定日期和楼层的所有行和列，设置座位为不可用状态
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            // 如果座位已有预约，显示警告信息
            if (library[dayIndex][floor][r][c].status == RESERVED) {
                cout << "Warning: Some seats are already reserved and will be unavailable." << endl;
            }
            // 设置座位为不可用状态并清空用户标识
            library[dayIndex][floor][r][c].status = UNAVAILABLE;
            library[dayIndex][floor][r][c].user = ' ';
        }
    }
    
    // 显示操作结果并保存数据
    cout << day << " Floor " << (floor + 1) << " is now unavailable." << endl;
    saveData();
}

// 设置某一天或某一层楼可被预约
// 管理员专用功能：设置指定日期和楼层的所有座位为可用状态
// 参数: day - 日期
// 参数: floor - 楼层
void setAvailable(const string &day, int floor) {
    // 获取日期索引并验证参数有效性
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }
    
    // 将1-based索引转换为0-based索引
    floor--;
    
    // 遍历指定日期和楼层的所有行和列，将不可用座位设为可用
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (library[dayIndex][floor][r][c].status == UNAVAILABLE) {
                library[dayIndex][floor][r][c].status = EMPTY;
                library[dayIndex][floor][r][c].user = ' ';
            }
        }
    }
    
    // 显示操作结果并保存数据
    cout << day << " Floor " << (floor + 1) << " is now available." << endl;
    saveData();
}

// 清空指定用户的数据
// 管理员专用功能：清除指定用户的所有预约记录
// 参数: user - 要清除数据的用户标识
void clearUserData(char user) {
    // 遍历所有日期、楼层、行和列，查找并清除指定用户的数据
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
    // 保存数据并显示操作结果
    saveData();
    cout << "User " << user << "'s data cleared." << endl;
}

// 解析并执行命令
// 根据用户输入的命令字符串执行相应的操作
// 参数: command - 用户输入的命令字符串
void executeCommand(const string &command) {
    // 重新加载数据以确保同步
    loadData();
    
    bool commandHandled = false;  // 标记命令是否被处理
    
    // 处理基础命令
    if (command == "Login") {
        login();  // 用户登录
        commandHandled = true;
    } else if (command == "Exit") {
        exitLogin();  // 退出登录
        commandHandled = true;
    } else if (command == "Quit") {
        // 程序将在主函数中退出
        commandHandled = true;
    } else if (currentUser == ' ') {
        // 用户未登录时，提示先登录
        cout << "Please login first." << endl;
        commandHandled = true;
    } else {
        // 处理登录后的命令
        
        // 处理显示座位命令（如："Monday Floor 2"）
        if (command.substr(0, 6) == "Monday" || command.substr(0, 7) == "Tuesday" || 
            command.substr(0, 9) == "Wednesday" || command.substr(0, 8) == "Thursday" || 
            command.substr(0, 6) == "Friday" || command.substr(0, 8) == "Saturday" || 
            command.substr(0, 6) == "Sunday") {
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
        }
        // 处理预约座位命令（如："Reserve Monday Floor 2 Seat 3 4"）
        else if (command.substr(0, 7) == "Reserve") {
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
        }
        // 处理显示预约命令
        else if (command == "Reservation") {
            showReservations();
            commandHandled = true;
        }
        // 管理员功能
        else if (command == "Clear" && isAdmin) {
            // 管理员清空所有数据
            clearAllData();
            commandHandled = true;
        } else if (command.substr(0, 6) == "Clear " && isAdmin) {
            // 管理员清空指定用户数据（如："Clear A"）
            char user = toupper(command[6]);
            if (isalpha(user)) {
                clearUserData(user);
                commandHandled = true;
            } else {
                cout << "ERROR" << endl;
                commandHandled = true;
            }
        } else if (isAdmin) {
            // 管理员专用命令
            
            // 管理员为用户预约座位（如："AdminReserve Monday 2 A 3 4"）
            if (command.substr(0, 12) == "AdminReserve") {
                try {
                    istringstream iss(command.substr(13));
                    string day, user; int floor, row, col;
                    iss >> day >> floor >> user >> row >> col;
                    adminReserveSeat(day, floor, row, col, toupper(user[0]));
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            }
            // 管理员取消用户预约（如："AdminCancel Monday 2 3 4"）
            else if (command.substr(0, 11) == "AdminCancel") {
                try {
                    istringstream iss(command.substr(12));
                    string day; int floor, row, col;
                    iss >> day >> floor >> row >> col;
                    adminCancelReservation(day, floor, row, col);
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            }
            // 管理员调整座位配置（如："ManageSeats 3 4 5"）
            else if (command.substr(0, 11) == "ManageSeats") {
                try {
                    istringstream iss(command.substr(12));
                    int floors, rows, cols;
                    iss >> floors >> rows >> cols;
                    manageSeats(floors, rows, cols);
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            }
            // 管理员清空某一天的所有预约（如："ClearDay Monday"）
            else if (command.substr(0, 8) == "ClearDay") {
                try {
                    string day = command.substr(9);
                    clearDayReservations(day);
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            }
            // 管理员清空某一楼层的所有预约（如："ClearFloor 2"）
            else if (command.substr(0, 10) == "ClearFloor") {
                try {
                    int floor = stoi(command.substr(11));
                    clearFloorReservations(floor);
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            }
            // 管理员设置某日期楼层不可预约（如："SetUnavailable Monday 2"）
            else if (command.substr(0, 13) == "SetUnavailable") {
                try {
                    istringstream iss(command.substr(14));
                    string day; int floor;
                    iss >> day >> floor;
                    setUnavailable(day, floor);
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            }
            // 管理员设置某日期楼层可预约（如："SetAvailable Monday 2"）
            else if (command.substr(0, 11) == "SetAvailable") {
                try {
                    istringstream iss(command.substr(12));
                    string day; int floor;
                    iss >> day >> floor;
                    setAvailable(day, floor);
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
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
// 程序的入口点，负责初始化系统、处理用户命令和退出逻辑
int main() {
    // 初始化座位库
    // 设置初始的楼层、行、列数，并初始化所有座位为EMPTY状态
    initializeLibrary();
    
    // 加载数据
    // 从文件中恢复之前保存的座位预约和用户数据
    loadData();
    
    // 命令循环
    // 持续接收用户输入的命令并进行处理，直到收到Quit命令
    string command;
    while (true) {
        // 显示命令提示符
        cout << "Please enter command: ";
        // 获取用户输入的整行命令
        getline(cin, command);
        
        // 去除命令前后的空格
        // 使用find_first_not_of和find_last_not_of定位第一个和最后一个非空格字符
        size_t start = command.find_first_not_of(" ");
        size_t end = command.find_last_not_of(" ");
        if (start != string::npos && end != string::npos) {
            command = command.substr(start, end - start + 1);
        } else {
            // 命令为空时显示错误信息
            cout << "ERROR" << endl;
            continue;
        }
        
        // 处理退出命令
        if (command == "Quit") {
            break;
        }
        
        // 执行其他命令
        // 将处理后的命令传递给executeCommand函数进行解析和执行
        executeCommand(command);
    }
    
    // 保存数据并退出
    // 在退出前将当前所有数据保存到文件中
    saveData();
    cout << "Program exited." << endl;
    return 0;
}