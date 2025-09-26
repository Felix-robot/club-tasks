# 项目说明

本项目(除了level1、level2文件夹之外，原谅我的史山)包含两个 Windows 可执行程序：

- test1.exe：交互式读取输入；输入 `Dian` 输出 `2002`，输入 `Quit` 退出，其余输入输出 `Error`。
- test2.exe：从 UTF-8 `.txt` 文件中读取 `key:value` 键值对，交互查询；输入 `Quit` 退出，找不到键输出 `Error`。



## 运行示例

```bash
# test1 交互示例（逐行输入）
./test1/test1.exe
Dian
2002
abc
Error
Quit

# test2 需提供一个 UTF-8 文本文件
# 假设 data.txt 内容：
# Robot:RL
# sdf:vdv
# sad:dfdsv
# eff:vsad
./test2/test2.exe data.txt
Robot
RL
unknown
Error
Quit
```

## 注意事项与边界
- 输入严格区分大小写。
- `test2` 会修剪键和值两端空白；非法行（无冒号、空键或空值）将被跳过。
- 对于超长行（> 1023 字符），会被截断并可能解析失败（返回 Error）。
