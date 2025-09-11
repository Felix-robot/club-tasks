#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 程序功能：
// - 自动查找同目录下的 data.txt 文件（UTF-8 编码）
// - 解析键值对，每行格式为 key:value
// - 交互式查询：输入键返回值，不存在返回 "Error"，输入 "Quit" 退出
//
// 文件解析规则：
// - 键：长度 1-10 字符，可含字母、数字、下划线，不含空格
// - 跳过空行和错误行（无冒号、含空格键等）
// - 重复键：保留首次出现，忽略后续重复
//
// 使用方法：
// Windows 示例：test2.exe（自动查找 data.txt）

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

// 校验键：长度 1-10，仅含字母、数字、下划线，不含空格
static int is_valid_key(const char *key) {
	if (!key) return 0;
	size_t len = strlen(key);
	if (len == 0 || len > 10) return 0;
	for (size_t i = 0; i < len; ++i) {
		unsigned char c = (unsigned char)key[i];
		// 允许：字母、数字、下划线
		if (!(isalnum(c) || c == '_')) return 0;
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
		if (!colon) continue; // 无冒号，跳过错误行
		
		// 分割键值
		*colon = '\0';
		char *key = line;
		char *value = colon + 1;
		trim(key);
		trim(value);
		
		// 检查键值非空
		if (*key == '\0' || *value == '\0') continue;
		
		// 校验键格式：长度1-10，仅含字母数字下划线
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

	printf("Please enter keys to look up values (Quit to exit):\n");

	// 自动查找同目录下的 data.txt
	const char *data_file = "data.txt";
	if (!load_file(data_file, &dict)) {
		fprintf(stderr, "Warning: failed to open or parse file: %s\n", data_file);
	}

	char input[512];
	for (;;) {
		if (fgets(input, sizeof(input), stdin) == NULL) break; // EOF 退出
		strip_newline(input);
		if (strcmp(input, "Quit") == 0) break; // 用户请求退出
		trim(input);
		// 空行直接跳过
		if (input[0] == '\0') { continue; }
		// 查询键值
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


