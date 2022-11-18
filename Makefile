DATE = $(shell date -R)
CC = gcc
VFLAGS = -Wall -Werror -Wextra -lm
UNAME = $(shell uname)
ifeq ($(UNAME), Linux)
	CTEST = -lcheck -lpthread -lrt -lm -lsubunit --coverage
else
	CTEST = -lcheck --coverage
endif
FILE = decimal

all: $(FILE).a

work: clean
	$(CC) $(VFLAGS) work.c -o work
	./work
	rm -f work	

main: clean
	$(CC) $(VFLAGS) main.c -o main
	./main
	rm -f main

test: clean $(FILE).a
	# checkmk test.check > $(FILE)_test.c
	$(CC) $(VFLAGS) $(FILE)_test.c $(FILE).a $(CTEST) -L.
	./a.out

rebuild: clean all

$(FILE).a: 
	$(CC) -c $(FILE).c
	ar rc $(FILE).a $(FILE).o
	ranlib $(FILE).a
	rm *.o

gcov_report: test
	$(CC) -fprofile-arcs -ftest-coverage $(FILE).c $(FILE)_test.c $(CTEST) -L.
	./a.out
	gcovr --html --html-details -o reports.html
	lcov -t a.out -o rep.info -c -d . --no-external
	genhtml -o report rep.info
	open report/index.html

run:
	./$(FILE).out

valg: $(FILE).a
	$(CC) -g $(VFLAGS) $(FILE)_test.c $(FILE).a $(CTEST) -L.
	valgrind --track-origins=yes -q ./a.out

clean:
	rm -f *.html *.o *.a *.gcno *.gcda *.info *.out *.css
	rm -rf ./report
	rm -f $(FILE)
	rm -rf *.dSYM
	clear

leaks: $(FILE).a
	$(CC) -g $(FILE)_test.c $(FILE).a $(CTEST) -L.
#	leaks --atExit -- ./a.out
	CK_FORK=no leaks --atExit -- ./a.out

git:
	git add .
	git commit -m "$(DATE)"
	git push origin master

