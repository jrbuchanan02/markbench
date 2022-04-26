# build function

source_files = ./src/main.cc ./src/test.cc ./src/test-suite.cc ./src/messages.cc ./src/test-runner.cc
includes = ./src
standard = --std=c++20
win_libraries = -lbcrypt -lgdi32
lin_libraries = -pthread

for_windows:
	g++ $(source_files) -I $(includes) $(standard) -o markbench.exe -DWINDOWS $(win_libraries)

for_linux:
	g++-10 $(source_files) -I $(includes) $(standard) -o markbench.out -DLINUX $(lin_libraries)