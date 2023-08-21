.PHONY: build

make build:
	./pre_build.sh && gcc -pedantic-errors -Wall -Wextra -Wsign-conversion -Ibuild -lpthread -o ./build/dont_trust ./build/*.c
make force:
	./pre_build.sh && gcc -lpthread -o ./build/dont_trust ./build/*.c
run:
	./build/dont_trust
clean:
	rm -R build
