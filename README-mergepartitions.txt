To merge a RISC OS SD card image with a Linux SD card.
Requires Linux, dd, sfdisk and losetup


Take a 4GB SD card, and find out its size in 512 byte sectors:
# sfdisk -uS /dev/sde
or
# dmesg
[84378.373086] scsi 6:0:0:0: Direct-Access     USB Mass Storage Device PQ: 0 ANSI: 0 CCS
[84378.375098] sd 6:0:0:0: Attached scsi generic sg5 type 0
[84378.377359] sd 6:0:0:0: [sde] 3948544 512-byte logical blocks: (2.02 GB/1.88 GiB)

here we find out that sectors_sdcard = 3948544
(keep a note of all these values, we'll use them in calculations later)

Find out the size of the RISC OS image:
$ ls -l RISCOS_Alpha_8Aug2012.img 
-rw-rw-rw- 1 atm26 atm26 1974067200 Aug  7 18:36 RISCOS_Alpha_8Aug2012.img

bytes_riscos_img = 1974067200
sectors_riscos_img = bytes_riscos_img/512

Likewise for Linux image:
$ ls -l 2012-07-15-wheezy-raspbian.img 
-rw-r--r-- 1 atm26 atm26 1939865600 Jul 15 20:45 2012-07-15-wheezy-raspbian.img

bytes_linux_img = 1939865600
sectors_linux_img = bytes_linux_img/512

Stick them together into a file:
# dd if=RISCOS_Alpha_8Aug2012.img of=merged.img bs=1M
# dd if=2012-07-15-wheezy-raspbian.img of=merged.img bs=512 skip=<sectors_riscos_img> conv=notrunc oflag=append

Look at the partition table of the Linux image:

# sfdisk -uS 2012-07-15-wheezy-raspbian.img -d
2012-07-15-wheezy-raspbian.img1 : start=     8192, size=   114688, Id= c
2012-07-15-wheezy-raspbian.img2 : start=   122880, size=  3665920, Id=83
2012-07-15-wheezy-raspbian.img3 : start=        0, size=        0, Id= 0
2012-07-15-wheezy-raspbian.img4 : start=        0, size=        0, Id= 0

So:
sectors_linux_fat_start = 8192
sectors_linux_fat_size=114688
sectors_linux_ext_start=122880
sectors_linux_ext_size = 3665920
megabytes_linux_ext_size = sectors_linux_ext_size/2048

sectors_merged_linux_fat_start = sectors_riscos_img + sectors_linux_fat_start
sectors_merged_linux_ext_start = sectors_riscos_img + sectors_linux_ext_start

bytes_merged_linux_ext_start = sectors_merged_linux_ext_start * 512

Check that works:
# losetup -f -o <bytes_merged_linux_ext_start> merged.img
# losetup -a
/dev/loop0: [fe05]:3408221 (merged.img), offset 1978261504  <- FAT
/dev/loop1: [fe05]:3408221 (merged.img), offset 2036981760  <- ext2
# mount /dev/loop1 /mnt
# ls /mnt
bin   boot  dev  etc
...
# umount /dev/loop1

Squeeze it to fit your SD card
# e2fsck /dev/loop1
# resize2fs /dev/loop1 1600M
(or whatever size fits)

redefine megabytes_linux_ext_size = 1600
sectors_linux_ext_size = megabytes_linux_ext_size*2048

Now to merge the partition tables
# sfdisk -uS merged.img -d > merged.part

Edit merged.part (img2 and img3 lines will contain zeros):
# partition table of merged.img
unit: sectors

merged.img1 : start=       10, size=   262144, Id= b, bootable  <- keep from RISC OS
merged.img2 : start=  3978480, size=  3665920, Id= 83
merged.img3 : start=  3863792, size=   114688, Id= c
merged.img4 : start=   262154, size=  3588086, Id=ad            <- keep from RISC OS

Fill in:
merged.img2 : start= <sectors_merged_linux_ext_start>
 size = <sectors_linux_ext_size>, Id=copy from img2 line of Linux partition table
merged.img3 : start= <sectors_merged_linux_fat_start>
 size = <sectors_linux_fat_size>, Id=copy from img1 line of Linux partition table

Write back to image:
# sfdisk -uS merged.img --force < merged.part

Now your image should be ready to write to an SD card!
