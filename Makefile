.PHONY: build
build:
	-rm -R build
	mkdir build
	find . -type f -name \*.h -exec cp \{\} ./build/ \;
	find . -type f -name \*.c -exec cp \{\} ./build/ \;
	gcc -pedantic-errors -Wall -Wextra -Wsign-conversion -Ibuild -std=gnu17 -o ./build/donttrust ./build/*.c -lcrypto 
debug:
	-rm -R build
	mkdir build
	find . -type f -name \*.h -exec cp \{\} ./build/ \;
	find . -type f -name \*.c -exec cp \{\} ./build/ \;
	gcc -g3 -pedantic-errors -Wall -Wextra -Wsign-conversion -Ibuild -std=gnu17 -o ./build/donttrust \
		./src/*.c \
		./src/net/*.c \
		./src/net/tor/*.c \
		./src/net/tor/control/*.c \
		./src/crypto/*.c \
		./src/common/*.c \
		./src/utils/terminal/*.c \
		./src/utils/error/*.c \
		./src/utils/signal/*.c \
		./src/utils/string/*.c \
		./src/init/*.c -lcrypto 
clean:
	rm -R build
