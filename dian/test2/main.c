#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 程序功能：
// - 从 UTF-8 编码的 .txt 文件读取键值对，每行格式为 key:value
// - 解析并存储到内存（简单数组动态扩容）
// - 交互式读取用户输入 key，打印对应 value；不存在则输出 "Error"
// - 输入 "Quit" 时退出
//
// 设计与鲁棒性：
// - 使用 fgets 逐行读取，避免缓冲区溢出
// - 自动跳过空行与无冒号或冒号在首/尾的非法行
// - 修剪键和值两端的空白字符（空格、\t、\r、\n 等）
// - 大小写敏感匹配键
// - 若文件较大，简单动态数组可能多次扩容；对于超大文件建议换为哈希表
//
// 使用方法：
// Windows 示例：test2.exe data.txt

typedef struct {
	char *key;
	char *value;
} Pair;

typedef struct {
	Pair *items;
	size_t size;
	size_t capacity;
} PairArray;

static void strip_newline(char *buffer) {
	if (!buffer) return;
	size_t len = strlen(buffer);
	while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) {
		buffer[len - 1] = '\0';
		len--;
	}
}

// 去除 UTF-8 BOM（若存在）
static void strip_utf8_bom(char *s) {
	if (!s) return;
	unsigned char *u = (unsigned char*)s;
	if (u[0] == 0xEF && u[1] == 0xBB && u[2] == 0xBF) {
		memmove(s, s + 3, strlen(s + 3) + 1);
	}
}

static void trim(char *s) {
	// 原地去除首尾空白字符（isspace 处理 \t\r\n 空格等）
	if (!s) return;
	char *start = s;
	while (*start && isspace((unsigned char)*start)) start++;
	char *end = s + strlen(s);
	while (end > start && isspace((unsigned char)end[-1])) end--;
	size_t new_len = (size_t)(end - start);
	if (start != s) memmove(s, start, new_len);
	s[s == NULL ? 0 : new_len] = '\0';
}

static int pair_array_init(PairArray *arr, size_t initial_capacity) {
	if (!arr) return 0;
	arr->size = 0;
	arr->capacity = initial_capacity ? initial_capacity : 8;
	arr->items = (Pair*)calloc(arr->capacity, sizeof(Pair));
	return arr->items != NULL;
}

static void pair_array_free(PairArray *arr) {
	if (!arr) return;
	for (size_t i = 0; i < arr->size; ++i) {
		free(arr->items[i].key);
		free(arr->items[i].value);
	}
	free(arr->items);
	arr->items = NULL;
	arr->size = 0;
	arr->capacity = 0;
}

static int pair_array_push(PairArray *arr, const char *key, const char *value) {
	if (!arr || !key || !value) return 0;
	if (arr->size == arr->capacity) {
		size_t new_cap = arr->capacity * 2;
		Pair *new_items = (Pair*)realloc(arr->items, new_cap * sizeof(Pair));
		if (!new_items) return 0;
		// 将新增的内存区域清零（不是必须，但更安全）
		memset(new_items + arr->capacity, 0, (new_cap - arr->capacity) * sizeof(Pair));
		arr->items = new_items;
		arr->capacity = new_cap;
	}
	arr->items[arr->size].key = _strdup(key);
	arr->items[arr->size].value = _strdup(value);
	if (!arr->items[arr->size].key || !arr->items[arr->size].value) return 0;
	arr->size++;
	return 1;
}

static const char* pair_array_find(const PairArray *arr, const char *key) {
	if (!arr || !key) return NULL;
	for (size_t i = 0; i < arr->size; ++i) {
		if (strcmp(arr->items[i].key, key) == 0) return arr->items[i].value;
	}
	return NULL;
}

// 校验键：长度 1..10，且不包含任意空白字符（仅用于文件解析）
static int is_valid_key(const char *key) {
	if (!key) return 0;
	size_t len = strlen(key);
	if (len == 0 || len > 10) return 0;
	for (size_t i = 0; i < len; ++i) {
		if (isspace((unsigned char)key[i])) return 0;
	}
	return 1;
}

static int load_file(const char *path, PairArray *out) {
	FILE *fp = fopen(path, "r");
	if (!fp) return 0;
	char line[1024];
	while (fgets(line, sizeof(line), fp)) {
		strip_newline(line);
		strip_utf8_bom(line);
		// 跳过空行
		char *cursor = line;
		while (*cursor && isspace((unsigned char)*cursor)) cursor++;
		if (*cursor == '\0') continue;

		char *colon = strchr(line, ':');
		if (!colon) continue; // 无冒号，跳过
		// 将冒号左侧作为 key，右侧作为 value
		*colon = '\0';
		char *key = line;
		char *value = colon + 1;
		trim(key);
		trim(value);
		if (*key == '\0' || *value == '\0') continue; // 非法行
		// 校验键：长度<=10 且不含空白
		if (!is_valid_key(key)) continue;
		// 忽略重复键（保留首次出现）
		if (pair_array_find(out, key) != NULL) continue;
		if (!pair_array_push(out, key, value)) {
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	return 1;
}

int main(int argc, char **argv) {
	PairArray dict;
	if (!pair_array_init(&dict, 16)) {
		fprintf(stderr, "Memory allocation failed.\n");
		return 1;
	}

	// 可选：提供文件则加载；失败仅告警，继续交互
	if (argc >= 2) {
		if (!load_file(argv[1], &dict)) {
			fprintf(stderr, "Warning: failed to open or parse file: %s\n", argv[1]);
		}
	}

	char input[512];
	for (;;) {
		if (fgets(input, sizeof(input), stdin) == NULL) break; // EOF 退出
		strip_newline(input);
		if (strcmp(input, "Quit") == 0) break; // 用户请求退出
		trim(input);
		// 空行直接跳过
		if (input[0] == '\0') { continue; }
		// 交互阶段不再限制键格式，仅根据是否存在返回
		const char *val = pair_array_find(&dict, input);
		if (val) {
			printf("%s\n", val);
		} else {
			printf("Error\n");
		}
	}

	pair_array_free(&dict);
	return 0;
}


