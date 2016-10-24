CC = gcc
task1: external8.c central8.c
	gcc external8.c -o external8
	gcc central8.c -o central8

task2: external44.c central44.c
	gcc external44.c -o external44
	gcc central44.c -o central44

task3: external442T.c central442T.c
	gcc -pthread external442T.c -o external442T
	gcc -pthread central442T.c -o central442T

runTask1: 
	./external8 10 1 &
	./external8 100 2 &
	./external8 1000 3 &
	./external8 10000 4 &
	./external8 50 5 &
	./external8 500 6 &
	./external8 5000 7 &
	./external8 50000 8 &
	./central8 10 &


runTask2: 
	./external44 10 1 1200 &
	./external44 100 2 1200 &
	./external44 1000 3 1200 &
	./external44 10000 4 1200 &
	./external44 50 5 1300 &
	./external44 500 6 1300 &
	./external44 5000 7 1300 &
	./external44 50000 8 1300 &
	./central44 10 100 1200 1300 &


runTask3: 
	./external442T 10 1 1200 &
	./external442T 100 2 1200 &
	./external442T 1000 3 1200 &
	./external442T 10000 4 1200 &
	./external442T 50 5 1300 &
	./external442T 500 6 1300 &
	./external442T 5000 7 1300 &
	./external442T 50000 8 1300 &
	./central442T 10 100 1200 1300 &
clean:
	rm -f external8
	rm -f central8
	rm -f external44
	rm -f central44
	rm -f external442T
	rm -f central442T
