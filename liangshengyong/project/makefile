.PHONY:all server client clean

all: server client

server:
	@echo "server make.."
	$(MAKE) -C server

client:
	@echo "client make.."
	$(MAKE) -C client

clean:
	@echo "cleaning UP.."
	$(MAKE) -C server clean
	$(MAKE) -C client clean
