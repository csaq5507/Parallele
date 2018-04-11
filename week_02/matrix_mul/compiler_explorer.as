
    .L12:                   // Every loop does this N*N*N
mov eax, DWORD PTR [rbp-4]
cdqe
imul rax, QWORD PTR [rbp-24]    // 1. mukl
mov rdx, rax
mov rax, QWORD PTR [rbp-48]
add rax, rdx                    // 1. add
lea rdx, [0+rax*4]
mov rax, QWORD PTR [rbp-56]
add rax, rdx                    // 2. add
vmovss xmm1, DWORD PTR [rax]
mov eax, DWORD PTR [rbp-4]
cdqe
imul rax, QWORD PTR [rbp-48]    // 2. mul
mov rdx, rax
mov rax, QWORD PTR [rbp-32]
add rax, rdx                    // 3. add
lea rdx, [0+rax*4]
mov rax, QWORD PTR [rbp-64]
add rax, rdx                    // 4. add
vmovss xmm0, DWORD PTR [rax]
vmulss xmm0, xmm1, xmm0         // vector[4] mul
vmovss xmm1, DWORD PTR [rbp-36]
vaddss xmm0, xmm1, xmm0         // vector[4] add
vmovss DWORD PTR [rbp-36], xmm0


    //every outer loop: N*N

// C[i*N+j] = sum; another add and 1 mul


    // (N * N * (N / 4) * 8) + (N * N * 2)
    //N=1000 = 2002 MFLOP
