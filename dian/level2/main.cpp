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

// 定义日期
const string DAYS[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
const int NUM_DAYS = 7;

// 可变的楼层和座位数，以便管理员可以增加或删除座位
int FLOORS = 5;
int ROWS = 4;
int COLS = 4;

// 定义座位状态
const char EMPTY = '0';       // 空闲
const char RESERVED = '1';    // 已预约
const char CURRENT_USER = '2'; // 当前用户预约
const char UNAVAILABLE = 'X'; // 不可预约

// 定义文件路径
const string DATA_FILE = "library_data.txt";

// 座位数据结构
struct Seat {
    char status;       // 座位状态
    char user;         // 预约用户
};

// 全局变量
vector<vector<vector<vector<Seat>>>> library;
char currentUser = ' ';       // 当前登录用户
bool isAdmin = false;         // 是否为管理员

// 保存座位配置
struct SeatConfig {
    int floors;
    int rows;
    int cols;
} seatConfig = {FLOORS, ROWS, COLS};

// 初始化座位
void initializeLibrary() {
    // 重新调整library的大小
    library.resize(NUM_DAYS);
    for (int d = 0; d < NUM_DAYS; d++) {
        library[d].resize(FLOORS);
        for (int f = 0; f < FLOORS; f++) {
            library[d][f].resize(ROWS);
            for (int r = 0; r < ROWS; r++) {
                library[d][f][r].resize(COLS);
                for (int c = 0; c < COLS; c++) {
                    library[d][f][r][c].status = EMPTY;
                    library[d][f][r][c].user = ' ';
                }
            }
        }
    }
    // 更新座位配置
    seatConfig.floors = FLOORS;
    seatConfig.rows = ROWS;
    seatConfig.cols = COLS;
}

// 保存数据到文件
void saveData() {
    ofstream file(DATA_FILE);
    if (!file.is_open()) {
        cout << "ERROR: Failed to save data." << endl;
        return;
    }

    // 首先保存座位配置信息
    file << FLOORS << " " << ROWS << " " << COLS << endl;

    // 然后保存座位数据
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
    
    // 首先读取座位配置信息
    if (getline(file, line)) {
        istringstream iss(line);
        int floors, rows, cols;
        if (iss >> floors >> rows >> cols) {
            FLOORS = floors;
            ROWS = rows;
            COLS = cols;
        }
    }
    
    // 初始化library以适应新的大小
    initializeLibrary();

    // 然后读取座位数据
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

// 管理员为用户预约座位
void adminReserveSeat(const string &day, int floor, int row, int col, char user) {
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS || row < 1 || row > ROWS || col < 1 || col > COLS || !isalpha(user)) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }

    floor--; row--; col--; // 转换为0-based索引

    if (library[dayIndex][floor][row][col].status != EMPTY) {
        cout << "ERROR" << endl;
        return;
    }

    // 检查用户在同一天是否已经预约了座位
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

    library[dayIndex][floor][row][col].status = RESERVED;
    library[dayIndex][floor][row][col].user = user;
    cout << "OK" << endl;
    
    // 保存数据
    saveData();
}

// 管理员取消用户预约
void adminCancelReservation(const string &day, int floor, int row, int col) {
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS || row < 1 || row > ROWS || col < 1 || col > COLS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }

    floor--; row--; col--; // 转换为0-based索引

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
void manageSeats(int newFloors, int newRows, int newCols) {
    if (newFloors <= 0 || newRows <= 0 || newCols <= 0) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }
    
    // 保存当前数据到临时变量
    auto tempLibrary = library;
    int oldFloors = FLOORS;
    int oldRows = ROWS;
    int oldCols = COLS;
    
    // 更新座位数量
    FLOORS = newFloors;
    ROWS = newRows;
    COLS = newCols;
    
    // 重新初始化library
    initializeLibrary();
    
    // 恢复尽可能多的原有数据
    int minFloors = min(oldFloors, newFloors);
    int minRows = min(oldRows, newRows);
    int minCols = min(oldCols, newCols);
    
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int f = 0; f < minFloors; f++) {
            for (int r = 0; r < minRows; r++) {
                for (int c = 0; c < minCols; c++) {
                    library[d][f][r][c] = tempLibrary[d][f][r][c];
                }
            }
        }
    }
    
    cout << "Seats updated successfully." << endl;
    saveData();
}

// 取消某一天所有人的预约
void clearDayReservations(const string &day) {
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1) {
        cout << "ERROR: Invalid day." << endl;
        return;
    }
    
    for (int f = 0; f < FLOORS; f++) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                library[dayIndex][f][r][c].status = EMPTY;
                library[dayIndex][f][r][c].user = ' ';
            }
        }
    }
    
    cout << "All reservations for " << day << " cleared." << endl;
    saveData();
}

// 取消某一层楼的全部预约
void clearFloorReservations(int floor) {
    if (floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid floor." << endl;
        return;
    }
    
    floor--; // 转换为0-based索引
    
    for (int d = 0; d < NUM_DAYS; d++) {
        for (int r = 0; r < ROWS; r++) {
            for (int c = 0; c < COLS; c++) {
                library[d][floor][r][c].status = EMPTY;
                library[d][floor][r][c].user = ' ';
            }
        }
    }
    
    cout << "All reservations for Floor " << (floor + 1) << " cleared." << endl;
    saveData();
}

// 设置某一天或某一层楼不可被预约
void setUnavailable(const string &day, int floor) {
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }
    
    floor--; // 转换为0-based索引
    
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (library[dayIndex][floor][r][c].status == RESERVED) {
                cout << "Warning: Some seats are already reserved and will be unavailable." << endl;
            }
            library[dayIndex][floor][r][c].status = UNAVAILABLE;
            library[dayIndex][floor][r][c].user = ' ';
        }
    }
    
    cout << day << " Floor " << (floor + 1) << " is now unavailable." << endl;
    saveData();
}

// 设置某一天或某一层楼可被预约
void setAvailable(const string &day, int floor) {
    int dayIndex = getDayIndex(day);
    if (dayIndex == -1 || floor < 1 || floor > FLOORS) {
        cout << "ERROR: Invalid parameters." << endl;
        return;
    }
    
    floor--; // 转换为0-based索引
    
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (library[dayIndex][floor][r][c].status == UNAVAILABLE) {
                library[dayIndex][floor][r][c].status = EMPTY;
                library[dayIndex][floor][r][c].user = ' ';
            }
        }
    }
    
    cout << day << " Floor " << (floor + 1) << " is now available." << endl;
    saveData();
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
        } else if (isAdmin) {
            // 管理员专用命令
            if (command.substr(0, 12) == "AdminReserve") {
                // 管理员为用户预约座位
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
            } else if (command.substr(0, 11) == "AdminCancel") {
                // 管理员取消用户预约
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
            } else if (command.substr(0, 11) == "ManageSeats") {
                // 管理员增加或删除座位
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
            } else if (command.substr(0, 8) == "ClearDay") {
                // 管理员取消某一天所有人的预约
                try {
                    string day = command.substr(9);
                    clearDayReservations(day);
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            } else if (command.substr(0, 10) == "ClearFloor") {
                // 管理员取消某一层楼的全部预约
                try {
                    int floor = stoi(command.substr(11));
                    clearFloorReservations(floor);
                    commandHandled = true;
                } catch (...) {
                    cout << "ERROR" << endl;
                    commandHandled = true;
                }
            } else if (command.substr(0, 13) == "SetUnavailable") {
                // 管理员设置某一天或某一层楼不可被预约
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
            } else if (command.substr(0, 11) == "SetAvailable") {
                // 管理员设置某一天或某一层楼可被预约
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

int main() {
    // 初始化座位库
    initializeLibrary();
    
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