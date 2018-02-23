# App for NanoPi-NEO

# mpd_gui.cpp

Display mpd playing info on LCD that connected to NanoPi-NEO.
![image.png](https://qiita-image-store.s3.amazonaws.com/0/187531/0a18da0a-7180-295d-a436-79224c031271.png)

https://qiita.com/blue-7/items/07ad5661607799053d44

### Compile

```bash:console
apt install libcv-dev libopencv-dev opencv-doc
apt install fonts-takao-pgothic
apt install libtag1-dev

g++ -O3 -std=c++11 mpd_gui.cpp -o mpd_gui.cpp.o -pthread `pkg-config --cflags opencv` `pkg-config --libs opencv` `freetype-config --cflags` `freetype-config --libs` `taglib-config --cflags` `taglib-config --libs`
```

# reg.c

Utility for register operation.
Read or write data to physical address space.

https://qiita.com/blue-7/items/5b1071a1176482d1f3a0

### Compile

gcc reg.c -o reg

# PerfTest_Matrix.cpp

4 by 4 matrix multiply performance test.

- NEON support.
- SSE2,AVX2,FMA support.

https://qiita.com/blue-7/items/7a4ea5a4c3aa63c61be9

https://qiita.com/blue-7/items/6b607e1af48bc25ecb35

