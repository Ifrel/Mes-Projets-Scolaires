Pour linker un programme avec ld:

```
arm-none-eabi-ld --section-start .text=0x58 --section-start .data=0x1000 -o prog ./Examples_loader/example4.o -EB
```
