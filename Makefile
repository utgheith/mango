BUILD_TYPE ?= release  # Options: release, debug, plain, ...
CXX ?= g++
BUILD_DIR = build/${strip ${BUILD_TYPE}}/${strip ${CXX}}
BUILD_TAG = .build.${strip ${BUILD_TYPE}}.${strip ${CXX}}

all : compile

.PHONY: all compile test format clean

help:
	@echo "Usage: make [target]"
	@echo "Targets:"
	@echo "  all       - Compile the project"
	@echo "  compile   - Compile the project"
	@echo "  test      - Run tests"
	@echo "  format    - Format the code"
	@echo "  clean     - Clean build files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "BUILD_DIR = '${BUILD_DIR}'"

BUILD_FILES = meson.build Makefile

compile : ${BUILD_FILES} ${BUILD_TAG}
	(cd ${BUILD_DIR} && meson compile)

test: ${BUILD_FILES} ${BUILD_TAG}
	(cd ${BUILD_DIR} && meson test)

format: ${BUILD_FILES} ${BUILD_TAG}
	(cd ${BUILD_DIR} && ninja clang-format)

${BUILD_TAG}: ${BUILD_FILES} .setup
	meson setup --reconfigure --buildtype ${BUILD_TYPE} --werror ${BUILD_DIR} .
	touch ${BUILD_TAG}

.setup: ${BUILD_FILES}
	-mkdir -p subprojects
	-meson wrap install gtest
	-touch .setup

clean:
	rm -rf build .build.*


