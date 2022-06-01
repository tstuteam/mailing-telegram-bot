
compile:
	g++ main.cpp -o main \
		--std=c++14 \
		-I/usr/local/include \
		-lTgBot -lboost_system -lssl -lcrypto -lpthread

clean:
	git clean -fdX
