def process_numbers(numbers):
    """
    处理整数列表，返回(平均值, 最大值, 最小值, 偶数和)
    """
    if not numbers:
        return None  # 如果列表为空，返回 None

    avg = sum(numbers) / len(numbers)  # 平均值
    maximum = max(numbers)             # 最大值
    minimum = min(numbers)             # 最小值
    even_sum = sum(n for n in numbers if n % 2 == 0)  # 偶数和

    return (avg, maximum, minimum, even_sum)


if __name__ == "__main__":
    # 提示用户输入
    user_input = input("请输入一系列整数，用逗号分隔：")
    try:
        # 将输入拆分并转换为整数列表
        numbers = [int(x.strip()) for x in user_input.split(",") if x.strip()]
        
        # 调用函数
        result = process_numbers(numbers)
        
        if result:
            print(f"平均值: {result[0]:.2f}, 最大值: {result[1]}, 最小值: {result[2]}, 偶数和: {result[3]}")
        else:
            print("输入的列表为空，无法计算。")
    except ValueError:
        print("输入错误，请确保输入的是整数并用逗号分隔。")
