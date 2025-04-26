BUILD_TYPE ?= release  # Options: release, debug, plain, ...
BUILD_DIR = build/${BUILD_TYPE}

all : compile

BUILD_FILES = meson.build Makefile

compile : ${BUILD_FILES} .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && meson compile)

test: ${BUILD_FILES} .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && meson test)

format: ${BUILD_FILES} .build.${BUILD_TYPE}
	(cd ${BUILD_DIR} && ninja clang-format)

.build.${BUILD_TYPE}: ${BUILD_FILES} .setup
	meson setup --reconfigure --buildtype ${BUILD_TYPE} --werror ${BUILD_DIR} .
	touch .build.${BUILD_TYPE}

.setup: ${BUILD_FILES}
	-mkdir -p subprojects
	-meson wrap install gtest
	-touch .setup

clean:
	rm -rf build .build.*


