CXXFLAGS = -std=c++23 -g -I"../raylib/include" -L"../raylib/lib" -Iinclude

final : main.cpp
	"\\8408-SRV-FILES\perso\F_ELE\TG6\LAPLACE-TOULOUSE\MinGW\bin\g++.exe" $(CXXFLAGS) main.cpp -o drone.exe -lraylibdll
