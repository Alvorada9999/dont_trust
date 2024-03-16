.PHONY: build

make build:
	./pre_build.sh && gcc -g -pedantic-errors -Wall -Wextra -Wsign-conversion -Ibuild -std=gnu17 -o ./build/dont_trust ./build/*.c
make force:
	./pre_build.sh && gcc -o ./build/dont_trust ./build/*.c
run:
	./build/dont_trust
clean:
	rm -R build
