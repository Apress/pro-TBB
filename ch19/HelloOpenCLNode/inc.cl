
__kernel void cl_inc(global int* A) {
     int gId = get_global_id(0);
     A[gId]++;
     if(gId == 0) printf("A[0]=%d\n", A[0]);
}
