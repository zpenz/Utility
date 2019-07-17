import zlib;
import hashlib;
import md5;

file = open("xk1.apk");

i = 5
while i>0:
    a = file.read(4096);
    print(zlib.adler32(a))
    print(md5.md5(a).hexdigest())
    i=i-1;
