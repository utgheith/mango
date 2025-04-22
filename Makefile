BUILD_TYPE ?= release  # Options: release, debug, plain, ...
BUILD_DIR = build/${BUILD_TYPE}


compile : Makefile .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && meson compile)

test: Makefile .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && meson test)

format: Makefile .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && ninja clang-format)

.build.${BUILD_TYPE}: Makefile .setup
	meson setup --reconfigure --buildtype ${BUILD_TYPE} --werror ${BUILD_DIR} .
	touch .build.${BUILD_TYPE}

.setup: Makefile
	-mkdir -p subprojects
	-meson wrap install gtest
	-touch .setup

clean:
	rm -rf build .build.*


