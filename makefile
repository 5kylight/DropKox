
server:
	$(MAKE) -C ./server al
client:
	$(MAKE) -C ./client all

.PHONY: clean server client
clean:
	$(MAKE) -C ./client clean
	$(MAKE) -C ./server clean

.PHONY : clean