#!/bin/bash
# use testnet settings,  if you need mainnet,  use ~/.reesistcore/reesistd.pid file instead
reesist_pid=$(<~/.reesistcore/testnet3/reesistd.pid)
sudo gdb -batch -ex "source debug.gdb" reesistd ${reesist_pid}
