# 图书馆座位预约系统

这是一个用C++实现的简易图书馆座位预约和管理系统，具有用户预约和管理员管理功能。

## 功能说明

### 基本设置
- 图书馆默认为5层，每层有4×4个座位（管理员可调整）
- 用户用A-Z的字母表示，管理员用户名为Admin（密码666）
- 可以预约下周七天的座位（Monday到Sunday）
- 座位状态：0（空闲），1（已预约），2（被当前用户预约），X（不可预约），A-Z（被该用户预约，仅管理员可见）
- 每个用户在同一天只能预约一个座位，若成功预约第二个座位，则自动取消第一个座位的预约

### 核心功能

#### Level 1-1：登录功能
- `Login`：登录系统
- `Exit`：退出当前登录
- `Quit`：退出程序

#### Level 1-2：查询和预约功能
- `Monday Floor n`（或其他日期，n为1-5）：显示某一天某一层的座位情况
- `Reserve Monday Floor n Seat m k`（m、k为1-4）：预约座位
- `Reservation`：显示当前用户的预约

#### Level 1-3：数据存储功能
- 数据会保存在`library_data.txt`文件中，程序重启后数据不会丢失
- 管理员可以使用以下命令：
  - `Clear`：清空所有用户数据
  - `Clear A`：清空用户A的数据（以此类推）
  - `AdminReserve day floor user row col`：为指定用户预约座位
  - `AdminCancel day floor row col`：取消指定座位的预约
  - `ManageSeats floors rows cols`：调整图书馆的层数和每层的座位数
  - `ClearDay day`：取消某一天所有人的预约
  - `ClearFloor floor`：取消某一层楼的全部预约
  - `SetUnavailable day floor`：设置某一天或某一层楼不可被预约
  - `SetAvailable day floor`：设置某一天或某一层楼可被预约

## 编译和运行

### Windows系统
1. 确保已安装C++编译器（如MinGW或Visual Studio）
2. 打开命令提示符
3. 进入程序所在目录
4. 使用以下命令编译：
   ```
   g++ main.cpp -o library_system.exe
   ```
5. 运行程序：
   ```
   library_system.exe
   ```

### 使用示例

### 普通用户功能

1. 登录：
   ```
   请输入命令: Login
   请输入用户名
   A
   登录成功。
   ```

2. 查询座位：
   ```
   请输入命令: Monday Floor 1
   0000
   0000
   0000
   0000
   ```

3. 预约座位：
   ```
   请输入命令: Reserve Monday Floor 1 Seat 1 2
   OK
   ```
   *注：如果用户在同一天已经预约了座位，新的预约会自动取消之前的预约*

4. 查看预约：
   ```
   请输入命令: Reservation
   Monday Floor 1 Seat 1 2
   ```

5. 退出登录：
   ```
   请输入命令: Exit
   Logged out.
   ```

### 管理员功能

1. 管理员登录：
   ```
   请输入命令: Login
   请输入用户名
   Admin
   请输入密码
   666
   登录成功。
   ```

2. 管理员为用户预约座位：
   ```
   请输入命令: AdminReserve Monday 1 B 2 3
   OK
   ```
   *表示为用户B在周一1楼2排3列预约座位*

3. 管理员取消指定座位预约：
   ```
   请输入命令: AdminCancel Monday 1 1 2
   OK
   ```
   *表示取消周一1楼1排2列的预约*

4. 管理员调整图书馆座位配置：
   ```
   请输入命令: ManageSeats 6 5 5
   Seats updated successfully.
   ```
   *表示将图书馆改为6层，每层5×5个座位*

5. 管理员取消某一天所有人的预约：
   ```
   请输入命令: ClearDay Monday
   All reservations for Monday cleared.
   ```

6. 管理员取消某一层楼的全部预约：
   ```
   请输入命令: ClearFloor 2
   All reservations for Floor 2 cleared.
   ```

7. 管理员设置某一天某层楼不可预约：
   ```
   请输入命令: SetUnavailable Monday 3
   Monday Floor 3 is now unavailable.
   ```

8. 管理员恢复某一天某层楼可预约状态：
   ```
   请输入命令: SetAvailable Monday 3
   Monday Floor 3 is now available.
   ```

9. 管理员清空所有数据：
   ```
   请输入命令: Clear
   All data cleared.
   ```

10. 管理员清空特定用户数据：
    ```
    请输入命令: Clear A
    User A's data cleared.
    ```

## 注意事项
- 用户名只能包含字母，不能包含其他字符
- 管理员操作需要验证密码
- 程序退出时会自动保存所有数据
- 同时运行多个程序实例时，数据可能会被最后退出的实例覆盖