
compile : Makefile .build
	(cd build && meson compile)

test: Makefile .build
	(cd build && meson test)

format: Makefile .build
	(cd build && ninja clang-format)

.build: Makefile .setup
	meson build --reconfigure
	touch .build

.setup: Makefile
	-mkdir -p subprojects
	-meson wrap install gtest
	-touch .setup

clean:
	rm -rf .build build


