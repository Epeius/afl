#!/bin/bash
# Prepare a clean environment for afl
mkdir /tmp/afltestcase
mkdir /tmp/afltracebits

rm -rf /tmp/afltestcase/*
rm -rf /tmp/afltracebits/*

rm -f /tmp/afl_qemu_queue

cp /home/epeius/work/afl-1.96b/tests/plot/aa.jpeg /home/epeius/work/afl-1.96b/tests/seed/aa.jpeg

rm -rf /home/epeius/work/afl-1.96b/tests/output/*
