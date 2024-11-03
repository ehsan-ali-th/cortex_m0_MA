Github does not allow files more than 100MB. That is why I had to split the gcc sysroot file. 

Sysroot zipped file: gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz

The split command used: 
```console
split -b 50MB gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz.
```

To merge:
```console
cat gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz.* > gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz
```

To unzip:
```console
tar xvf gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu.tar.xz
```
