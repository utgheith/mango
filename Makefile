BUILD_TYPE ?= release  # Options: release, debug, plain, ...
BUILD_DIR = build/${BUILD_TYPE}

all : compile

build_files: meson.build Makefile;

compile : build_files .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && meson compile)

test: build_files .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && meson test)

format: build_files .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && ninja clang-format)

.build.${BUILD_TYPE}: build_files .setup
	meson setup --reconfigure --buildtype ${BUILD_TYPE} --werror ${BUILD_DIR} .
	touch .build.${BUILD_TYPE}

.setup: build_files
	-mkdir -p subprojects
	-meson wrap install gtest
	-touch .setup

clean:
	rm -rf build .build.*


