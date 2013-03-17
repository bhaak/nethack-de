TESTS = test_gott_weiblich

all: $(TESTS)

test_gott_weiblich: test_gott_weiblich.c ../src/german.c
	$(CC) -Wall -I. -I../include -lcheck $(CFLAGS) $< -o $@

clean:
	rm -f *.xml test_*.o $(TESTS)

check: $(TESTS)
	test_gott_weiblich
