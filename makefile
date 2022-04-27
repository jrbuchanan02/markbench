# build function

source_files = ./src/main.cc ./src/test.cc ./src/test-suite.cc ./src/messages.cc ./src/test-runner.cc
includes = -I ./src
standard = --std=c++20
win_libraries = -lbcrypt -lgdi32
lin_libraries = -pthread -lgtk-3

win_includes = 
lin_includes = -I /usr/include/gtk-3.0 -I /usr/include/glib-2.0 -I /usr/lib/x86_64-linux-gnu/glib-2.0/include/ -I /usr/include/pango-1.0/ -I /usr/include/harfbuzz -I /usr/include/cairo -I /usr/include/gdk-pixbuf-2.0 -I /usr/include/atk-1.0

# optimize for O2, although there are some tests which are forcibly optimized for
# O0 and O1 since O2 optimizations either eliminated the test itself or changed
# some external behavior.
optimize = -O2

for_windows:
	g++ $(source_files) $(includes) $(win_includes) $(standard) -o markbench.exe -DWINDOWS $(win_libraries) $(optimize)

for_linux:
	g++-10 $(source_files) $(includes) $(lin_includes) $(standard) -o markbench.out -DLINUX $(lin_libraries) $(optimize)