all:
# check if the MakeFile runned by the root
	@if [ `id -u` -ne 0 ]; then \
		echo "You need to be root to run this Makefile" >&2; \
		exit 1; \
	fi
# install compiler requirements
	sudo apt install build-essential
	sudo apt install g++
# install required libraries
	sudo apt install libx11-dev;
# compile the program
	./build/compile_linux_version.sh;
	cp -f ejib /usr/bin;
	sudo chmod u+s,g+s,o+s /usr/bin/ejib;
	mkdir -p /opt/ejib;
	cp -fr help /opt/ejib;
	cp -f EjibTextLogo.txt /opt/ejib;