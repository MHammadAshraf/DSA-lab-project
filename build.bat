@echo off
REM Quick build with g++ / MinGW. Run from the project root (D:\DSAProject).
REM The data\ folder must sit next to the produced exe at runtime.
echo Building FoodExpress...
g++ -std=c++17 -O2 -Wall -o foodexpress.exe src\*.cpp
if %errorlevel%==0 (
    echo Build OK -> foodexpress.exe
) else (
    echo Build FAILED
)
