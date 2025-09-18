CXXFLAGS = -std=c++23 -g -I"../../../raylib/include" -L"../../../raylib/lib" -Iinclude

final : main.cpp
	g++.exe $(CXXFLAGS) main.cpp -o drone.exe -lraylibdll
