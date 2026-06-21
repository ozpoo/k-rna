```bash
git init
git add .
git commit -m "mykernel v0.6 — multitasking, paging, keyboard, config"
```

# verify toolchain still works
x86_64-elf-gcc --version
nasm --version
qemu-system-x86_64 --version

# make sure it still boots
cd mykernel
make clean && make run