# PowerShell脚本 - 清理B+树生成的数据文件
# 该脚本会删除所有以 "File" 结尾的文件

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "B+树数据文件清理工具" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan

# 获取当前目录
$currentPath = Get-Location
Write-Host "`n当前目录: $currentPath" -ForegroundColor Green

# 查找所有以File结尾的文件
$filesToDelete = Get-ChildItem -Path . -Recurse -Filter "*File" -File

if ($filesToDelete.Count -eq 0) {
    Write-Host "`n没有找到需要清理的文件。" -ForegroundColor Yellow
} else {
    Write-Host "`n找到以下文件将被删除:" -ForegroundColor Yellow
    $filesToDelete | ForEach-Object {
        Write-Host "  - $($_.FullName)" -ForegroundColor Red
    }
    
    # 询问用户确认
    $confirmation = Read-Host "`n确定要删除这些文件吗? (Y/N)"
    
    if ($confirmation -eq 'Y' -or $confirmation -eq 'y') {
        $deletedCount = 0
        $filesToDelete | ForEach-Object {
            try {
                Remove-Item $_.FullName -Force
                Write-Host "已删除: $($_.Name)" -ForegroundColor Green
                $deletedCount++
            } catch {
                Write-Host "删除失败: $($_.Name) - $($_.Exception.Message)" -ForegroundColor Red
            }
        }
        Write-Host "`n清理完成！共删除 $deletedCount 个文件。" -ForegroundColor Green
    } else {
        Write-Host "`n操作已取消。" -ForegroundColor Yellow
    }
}

Write-Host "`n========================================" -ForegroundColor Cyan 