# 获取当前工作目录
$currentDirectory = Get-Location
Write-Host "Current Directory: $currentDirectory"

# 定义相对路径下的 SQL 文件目录
$sqlDirectory = ".\"
$outputFile = ".\combined.sql"

# 如果输出文件已存在，则删除它
if (Test-Path $outputFile) {
    Remove-Item $outputFile
}

# 获取目录中的所有 .sql 文件
$sqlFiles = Get-ChildItem -Path $sqlDirectory -Filter *.sql

# 创建一个新文件
New-Item -Path $outputFile -ItemType File

# 遍历每个 .sql 文件并将其内容追加到输出文件中
foreach ($file in $sqlFiles) {
    $content = Get-Content $file.FullName -Raw
    Add-Content -Path $outputFile -Value $content
    Add-Content -Path $outputFile -Value "`n"
}

Write-Host "All .sql files have been merged into $outputFile"
