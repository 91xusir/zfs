import os
import re
import fnmatch

# 这是一个python脚本，
# 用于查找指定目录下所有系统头文件
# 和不含额外依赖的头文件
# 方便预编译加快构建速度
base_dir = "./game_client"  # 替换为你的目录

def find_system_headers(directory):
    """查找所有系统头文件"""
    system_headers = set()  # 使用集合去重
    include_pattern = re.compile(r"#include\s*<([^>]+)>")

    for root, _, files in os.walk(directory):
        for filename in files:
            if filename.endswith((".h", ".hpp", ".c", ".cpp")):
                file_path = os.path.join(root, filename)
                with open(file_path, "r", encoding="utf-8", errors="ignore") as file:
                    for line in file:
                        match = include_pattern.search(line)
                        if match:
                            system_headers.add(match.group(1))

    return system_headers  # 直接返回集合

# 找到所有的系统头文件
system_headers = find_system_headers(base_dir)

# 打印系统头文件列表
print("Found system headers:")
for header in sorted(system_headers):
    print(f'#include <{header}>')

def find_files(base_dir, pattern):
    """递归查找符合模式的文件"""
    for root, dirs, files in os.walk(base_dir):
        for filename in fnmatch.filter(files, pattern):
            yield os.path.join(root, filename)

def check_file_for_include(file_path):
    """检查文件是否包含 #include 语句"""
    with open(file_path, "r", encoding="utf-8", errors="ignore") as file:
        for line in file:
            if "#include" in line:
                return True
    return False

def main(base_dir):
    """主函数，查找不含 #include 语句的 .h 文件"""
    pattern = "*.h"
    unique_files = set()  # 使用集合去重
    for file_path in find_files(base_dir, pattern):
        if not check_file_for_include(file_path):
            unique_files.add(file_path)
    
    for file_path in sorted(unique_files):
        print(f'#include "{file_path}"')

if __name__ == "__main__":
    main(base_dir)
