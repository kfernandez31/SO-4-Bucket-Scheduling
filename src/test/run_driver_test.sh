cd /
patch -t -p1 < /tests/driver.patch

cd /usr/src/minix/drivers/examples/hello
make clean
make
make install

mknod /dev/test111 c 20 0
service up /service/hello -dev /dev/test111
cat /dev/test111

service update /service/hello
service down hello
rm /dev/test111
