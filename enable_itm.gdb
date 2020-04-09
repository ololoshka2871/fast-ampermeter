shell if [[ ! -p /tmp/itm.fifo ]]; then mkfifo /tmp/itm.fifo; fi
target remote :2331
monitor reset halt
monitor tpiu config internal /tmp/itm.fifo uart off 60000000 500000
monitor itm port 0 1
quit
