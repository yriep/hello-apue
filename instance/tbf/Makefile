object = main.o mytbf_thread.o

mytbf_th: $(filter %.o, $(object))
	cc -o mytbf_th $(filter %.o, $(object))

$(filter %.o, $(object)): %.o : %.c
	$(CC) -c $< -o $@

.PHONY: clean
clean:
	-rm mytbf_th $(object)
