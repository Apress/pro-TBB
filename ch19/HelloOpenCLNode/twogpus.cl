
__kernel void cl_add(global float* A,
                       global float* B,
                       global float* C) {
  int2 gId = (int2)(get_global_id(0), get_global_id(1));
  int2 gSize = (int2)(get_global_size(0), get_global_size(1));

  if(gId.x == 0 && gId.y==0)
   printf("gSize.x=%d, gSize.y=%d\n",gSize.x,gSize.y);
  C[gId.x*gSize.x+gId.y]=A[gId.x*gSize.x+gId.y]+B[gId.x*gSize.x+gId.y];
}

__kernel void cl_sub(global float* A,
                       global float* B,
                       global float* C) {
  int2 gId = (int2)(get_global_id(0), get_global_id(1));
  int2 gSize = (int2)(get_global_size(0), get_global_size(1));

  if(gId.x == 0 && gId.y==0)
     printf("gSize.x=%d, gSize.y=%d\n",gSize.x,gSize.y);
  C[gId.x*gSize.x+gId.y]=C[gId.x*gSize.x+gId.y]-B[gId.x*gSize.x+gId.y];
}
