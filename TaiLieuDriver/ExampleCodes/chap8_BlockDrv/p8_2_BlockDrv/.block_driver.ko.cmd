cmd_/home/pvh/DriverProgramming/ExampleCodes/chap8_BlockDrv/p8_2_BlockDrv/block_driver.ko := ld -r -m elf_x86_64  -z max-page-size=0x200000 -T ./scripts/module-common.lds --build-id  -o /home/pvh/DriverProgramming/ExampleCodes/chap8_BlockDrv/p8_2_BlockDrv/block_driver.ko /home/pvh/DriverProgramming/ExampleCodes/chap8_BlockDrv/p8_2_BlockDrv/block_driver.o /home/pvh/DriverProgramming/ExampleCodes/chap8_BlockDrv/p8_2_BlockDrv/block_driver.mod.o ;  true
