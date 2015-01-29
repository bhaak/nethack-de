TESTS = test_gott_weiblich test_str2xxx

all: $(TESTS)

test_gott_weiblich: test_gott_weiblich.c ../src/german.c
	$(CC) -Wall -DGERMAN -I. -I../include $< -lcheck $(CFLAGS) -o $@

test_str2xxx: test_str2xxx.c ../src/role.c
	$(CC) -Wall -DGERMAN -I. -I../include $< -lcheck $(CFLAGS) -o $@

clean:
	rm -f *.xml test_*.o $(TESTS)

check: $(TESTS)
	./test_gott_weiblich
	./test_str2xxx
