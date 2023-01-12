rate: gpcr.c
	gcc $< -o $@

run: rate
	./rate input.txt

clean:
	rm rate