
# 用于统计指定目录下的代码行数 会忽略注释和空行

import os
def is_comment_or_empty(line):
    # 判断是否是空行或注释行
    if line.strip() == '':
        return True
    if line.strip().startswith('//') or line.strip().startswith('#'):
        return True
    if '/*' in line or '*/' in line:
        return True
    return False

def count_lines(file_path):
    total_lines = 0
    with open(file_path, 'r', encoding='utf-8', errors='ignore') as file:
        in_comment_block = False
        for line in file:
            if '/*' in line:
                in_comment_block = True
            if not in_comment_block and not is_comment_or_empty(line):
                total_lines += 1
            if '*/' in line:
                in_comment_block = False
    return total_lines

def recursive_count(directories, extensions):
    line_counts = {ext: 0 for ext in extensions}
    for directory in directories:
        print(f"Scanning directory: {directory}")
        for root, dirs, files in os.walk(directory):
            for file in files:
                if file.endswith(extensions):
                    file_path = os.path.join(root, file)
                    ext = os.path.splitext(file)[1]
                    if ext in line_counts:
                        line_counts[ext] += count_lines(file_path)
    return line_counts

def main():
    # 直接在代码中指定目录列表
    directories = [
        'game_client',
        'game_engine',
        'gameworld_server',
        'login_server',
        'region_server',
        'shared'
        # 添加更多的目录路径
    ]
    
    # 需要统计的文件扩展名
    extensions = ('.h', '.cpp', '.c')
    
    # 进行递归统计
    line_counts = recursive_count(directories, extensions)

    # 打印结果
    for ext, count in line_counts.items():
        print(f"Total lines of code for {ext} files: {count}")

if __name__ == "__main__":
    main()
