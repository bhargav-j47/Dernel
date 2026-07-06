#!/bin/bash

cp kernel.bin root_dir/boot/dernel

grub-mkrescue -o dernel.iso root_dir/
