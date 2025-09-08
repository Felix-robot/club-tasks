#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 程序功能：
// - 交互式读取用户输入（逐行读取）
// - 当输入 "Dian" 输出 "2002" 并继续等待下一次输入
// - 当输入 "Quit" 时退出程序
// - 其他任意输入输出 "Error"
//
// 设计与鲁棒性：
// - 使用 fgets 读取，避免 gets 带来的缓冲区溢出风险
// - 统一移除行末的 \r\n 以兼容 Windows 和其他平台的换行格式
// - 对于超长输入，fgets 会截断；逻辑上将被视为不等于目标字符串，返回 "Error"
// - 区分大小写，按照题意严格匹配 "Dian" 与 "Quit"

// 安全移除行末换行与回车，原地修改缓冲区
static void strip_newline(char *buffer) {
	if (buffer == NULL) return;
	size_t len = strlen(buffer);
	while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
		buffer[len - 1] = '\0';
		len--;
	}
}

int main(void) {
	// 选择一个适中的缓冲区大小，足以容纳常见交互输入
	char input_buffer[256];


	// 提示用户输入
	printf("Enter commands (Dian to get 2002, Quit to exit):\n");

	// 交互主循环，直到用户输入 "Quit"
	for (;;) {
		if (fgets(input_buffer, (int)sizeof(input_buffer), stdin) == NULL) {
			// 读到 EOF 或输入错误：安全退出
			break;
		}

		strip_newline(input_buffer);

		if (strcmp(input_buffer, "Quit") == 0) {
			// 退出条件
			break;
		} else if (strcmp(input_buffer, "Dian") == 0) {
			printf("2002\n");
		} else {
			printf("Error\n");
		}
	}

	return 0;
}


