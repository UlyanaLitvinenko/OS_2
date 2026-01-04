def fibonacci(n):
    result = []
    a = 0
    b = 1
    for _ in range(n):
        result.append(b)
        temp = b
        b = a + b
        a = temp
    return result

def is_palindrome(num):
    s = str(num)
    left = 0
    right = len(s) - 1
    while left < right:
        if s[left] != s[right]:
            return False
        left += 1
        right -= 1
    return True

def reverse_list(values):
    result = []
    i = len(values) - 1
    while i >= 0:
        result.append(values[i])
        i -= 1
    return result

print("1 - Фибоначчи")
print("2 - Палиндром")
print("3 - Разворот списка")

task = input("Номер задачи: ")

if task == "1":
    n = int(input("n: "))
    for num in fibonacci(n):
        print(num)

elif task == "2":
    num = int(input("Число: "))
    if is_palindrome(num):
        print("Да")
    else:
        print("Нет")

elif task == "3":
    raw = input("Числа через пробел: ")
    nums = [int(x) for x in raw.split()]
    for num in reverse_list(nums):
        print(num)

else:
    print("Нет такой задачи")
