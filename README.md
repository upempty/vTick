# vTickSphere
os over x86 architecture, drive from one to one, lifting little by little.

# guide
## build
```
make
```

## run
```
qemu-system-i386 -drive format=raw,file=disk.img -vnc 172.23.245.135:5901

```

## ui
```
To check exact port used by qemu process via netstat -lntup in server,
e.g
netstat -lntup
tcp        0      0 172.23.245.135:11801    0.0.0.0:*               LISTEN      1289/qemu-system-i3

Then in client as RealVNC Viewer, login via:
121.43.133.208:11801

```

![image](https://github.com/upempty/stepback-os/assets/52414719/4eacd75b-ade7-425d-b572-bb357733e71c)


