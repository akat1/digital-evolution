I always wanted to make a little experiment - build a small VM an write a
program which will make almost perfect copy itself, just to see if something
interesting can evolve. This repo contains a small VM and the program, but
I don't really know what are the results of the experiment. I just had a great
time playing with it, introducing food/fuel, changing parameters and
pretending I'm doing something scientific. ¯\_(ツ)_/¯

# BUILDING:

```
 $ cc -O3 -o vm vm.c -lncurses
 $ nasm -o copy.o copy.s
```

# RUN

```
 $ ./vm copy.o
```

Program stops when all threads are DEAD. The status is updated every few
thousands iterations.

```
threads: 1 level: 1390000
spawned: 5720458 died: 5720457
max fuel: 4

e[R14,Rw9vRW43i9n287jVjR3]norq.7
92fkqewvwfq8ow2nv491hqvR3,ev>h2]
k4wn4[[]2v83[]en27e;vikoj8vvp2i.
qpeq79f87vef9',8][R4vie.qriqke88
fe--1v[n,vve-n>;v2>e739wf2]V9v]4
127n,2oRjv9[29'pvfhvjffp,8vR]v42
fv9w7;4,87]2nVff79ohvho]qrR434p;
..o4312vir24;W>Vn-f,f7.44-o178v;
Rneqh8R4qqwi,9k;r4vk293vp8f2kee1
ro87vw[vwhpvVe>7hWiv.V;2[q4wwVqf
ovRqinee.4qk9]iq994e7h7k8w7w7e94
72q8r,hkv2.94-497W9[79v4';8jv8Rn
.9i'8[q'297ffk8[-erqqnq9R[e9eqv.
w9efweie.r4j9v3vrei9eqk3Wqqv4-hw
3fqn7Vw>9hwr2;]q2qfvh[,W[kv9287v
88qjvfnW-nn2vhnovVw4e8qqei'7',or
weve4ef-v47qfj.21v4;eWre1ore37.f
7vn9j>h2w,18q8iWvV7j.'72[ee-vov9
93]qi[88>Rfh8,hvRVw[[fo9nqjVe8wW
V2f2eqe2,wVqfj'R48VVv;>eRW97-vq-
W'81h79v>e7n-2vwif1v8i3wn4.8o78e
v8f4p81q-e;3weqjf>vp2e,W98837Wiv
7';hprj78R7ivf[98W14V.n,wW78R2h.
i22'p84-ov,v.eVvv1n-7o;o2772n.29
e7o298.k39in84vv>qq9v8ivn449er7e
qi;8jR7q18793,kv1V,1'rh9j8j3R[8v
R2rn7f]vkonW.27feefv779n33-1e8v-
2fv3w92e3ef]qk-'>9;f;q8q4-Vw,'11
;9WR[]v9.29[iv2q>oh4.R'h-.'42vq2
27eko,2-8f>[qeq3v]2j872e>q294qnn
7qRRp4W1449nR7pq;jwfW[8n2]oe3qwe
>-,q'3vo9.8'8[wivVVW3vn7[ne3v22p
```

What you see above is:

 + threads - the number of running threads
 + level - number of iterations
 + spawned - number of spawned threads since start
 + died - number of threads that are gone since start
 + max fuel - maximum amount of fuel that one of the threads has

 The picture below is just a memory dump, each characters represents a few
cells and character to print is chosen depending on the values. It just let
you to see what's happening.

The code is small, so I encourage you to change it here and there and see
the results. If you get some interesting results, let me know!

 Happy hacking!
 - shm
