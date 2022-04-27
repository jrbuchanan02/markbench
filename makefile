# build function

source_files = ./src/main.cc ./src/test.cc ./src/test-suite.cc ./src/messages.cc ./src/test-runner.cc
includes = ./src
standard = --std=c++20
win_libraries = -lbcrypt -lgdi32
lin_libraries = -pthread

# optimize for O2, although there are some tests which are forcibly optimized for
# O0 and O1 since O2 optimizations either eliminated the test itself or changed
# some external behavior.
optimize = -O2

for_windows:
	g++ $(source_files) -I $(includes) $(standard) -o markbench.exe -DWINDOWS $(win_libraries) $(optimize)

for_linux:
	g++-10 $(source_files) -I $(includes) $(standard) -o markbench.out -DLINUX $(lin_libraries) $(optimize)