make:
	clang main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o caudio
	./caudio
