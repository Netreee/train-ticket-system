@echo off
chcp 65001 >nul
echo ========================================
echo B+树数据文件清理工具 (批处理版本)
echo ========================================
echo.
echo 当前目录: %cd%
echo.

setlocal enabledelayedexpansion
set count=0
set found=0

echo 正在查找以 "File" 结尾的文件...
echo.

for /r %%f in (*File) do (
    if exist "%%f" (
        if !found!==0 (
            echo 找到以下文件：
            echo.
        )
        echo   - %%f
        set /a found+=1
    )
)

if !found!==0 (
    echo 没有找到需要清理的文件。
    echo.
    pause
    exit /b
)

echo.
echo 共找到 !found! 个文件
echo.

:confirm
set /p choice=确定要删除这些文件吗? (Y/N): 

if /i "%choice%"=="Y" goto delete
if /i "%choice%"=="N" goto cancel
echo 请输入 Y 或 N
goto confirm

:delete
echo.
echo 正在删除文件...
echo.

for /r %%f in (*File) do (
    if exist "%%f" (
        del /f /q "%%f" 2>nul
        if !errorlevel!==0 (
            echo 已删除: %%~nxf
            set /a count+=1
        ) else (
            echo 删除失败: %%~nxf
        )
    )
)

echo.
echo 清理完成！共删除 !count! 个文件。
echo.
pause
exit /b

:cancel
echo.
echo 操作已取消。
echo.
pause
exit /b 