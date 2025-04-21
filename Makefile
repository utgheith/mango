
all: .build
	(cd build ; ninja)

.build:
	rm -rf build
	mkdir -p build
	(cd build; cmake -G Ninja ..)
	touch .build

format:
	clang-format -style=Google -Werror -i *.cc mango/*.h

clean:
	rm -rf build .build


