all: SDL

SDL-1.2.13:
	tar xzf SDL-1.2.13.tar.gz

SDL: SDL-1.2.13
	@if [ ! -d /usr/include/X11 ]; then echo "you don't appear to have X. On Ubuntu, this is fixed with 'sudo apt-get install xorg-dev'"; false; fi
	@if [ ! -f /usr/include/GL/gl.h -a ! -f /usr/X11/include/GL/gl.h ]; then echo "you don't appear to have OpenGL. On Ubuntu, this is fixed with 'sudo apt-get install libgl1-mesa-dev'"; false; fi
	@if [ ! -f /usr/include/GL/glu.h -a ! -f /usr/X11/include/GL/glu.h ]; then echo "you don't appear to have the OpenGL GLU library. On Ubuntu, this can be fixed with 'sudo apt-get install libglu1-mesa-dev'"; false; fi
	cd SDL-1.2.13 && ./configure --prefix=$(PWD)/SDL
	cd SDL-1.2.13 && make && make install

clean:
	-rm -rf SDL-1.2.13 SDL

.PHONY : clean
