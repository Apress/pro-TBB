__kernel void triad(global float* A, global float* B,
                    global float* C, float alpha)
  {
    //printf("%f", alpha);
    int i=get_global_id(0);
    C[i]=A[i]+alpha*B[i];
  }
