for VARIABLE in 10 100 1000 10000 100000
do
	./a.out $VARIABLE 2 > results.$VARIABLE.txt
done