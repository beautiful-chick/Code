all:
	@echo "Start compile library..."
	make -C src
	@echo "Start compile test program..."
	make -C project


run:
	@echo "Start run test program..."
	make run -C project


clean:
	make clean -C src
	make clean -C project


distclean: clean
	rm -f lib/*.a
	rm -f lib/*.so
