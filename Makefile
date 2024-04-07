.PHONY: build

make build:
	./pre_build.sh && gcc -pedantic-errors -Wall -Wextra -Wsign-conversion -Ibuild -std=gnu17 -lcrypto -o ./build/dont_trust ./build/*.c
make debug:
	./pre_build.sh && gcc -g3 -pedantic-errors -Wall -Wextra -Wsign-conversion -Ibuild -std=gnu17 -lcrypto -o ./build/dont_trust ./src/*.c ./src/net/*.c ./src/net/tor/*.c ./src/net/tor/control/*.c ./src/crypto/*.c ./src/common/*.c ./src/utils/terminal/*.c ./src/utils/error/*.c ./src/utils/signal/*.c ./src/utils/string/*.c ./src/init/*.c
clean:
	rm -R build
