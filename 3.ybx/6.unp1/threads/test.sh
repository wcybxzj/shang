#!/bin/bash
echo "===============================web01==========================================="
echo "for i in {1..10}; do ./web00 192.168.91.12 80 /index.html; sleep 1; done"
for i in {1..10}; do ./web00 192.168.91.12 80 /index.html; sleep 1; done
sleep 5;
echo "================================web02=========================================="
echo "for i in {1..10}; do ./web02 1 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web02 1 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web02 2 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web02 2 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web02 3 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web02 3 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web02 5 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web02 5 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web02 10 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web02 10 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web02 20 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web02 20 192.168.91.12 80 /index.html; sleep 2; done
echo "===========================web03 pthread_cond_signal========================="
echo "for i in {1..10}; do ./web03 1 signal 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 1 signal 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 2 signal 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 2 signal 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 3 signal 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 3 signal 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 5 signal 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 5 signal 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 10 signal 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 10 signal 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 20 signal 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 20 signal 192.168.91.12 80 /index.html; sleep 2; done
echo "===========================web03 pthread_cond_broadcast========================="
echo "for i in {1..10}; do ./web03 1 broadcast 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 1 broadcast 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 2 broadcast 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 2 broadcast 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 3 broadcast 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 3 broadcast 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 5 broadcast 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 5 broadcast 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 10 broadcast 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 10 broadcast 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web03 20 broadcast 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web03 20 broadcast 192.168.91.12 80 /index.html; sleep 2; done
echo "================================web04=========================================="
echo "for i in {1..10}; do ./web04 func1 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web04 func1 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "for i in {1..10}; do ./web04 func2 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web04 func2 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
echo "================================web05=========================================="
echo "for i in {1..10}; do ./web05 func1 192.168.91.12 80 /index.html; sleep 2; done"
for i in {1..10}; do ./web05 func1 192.168.91.12 80 /index.html; sleep 2; done
sleep 5;
