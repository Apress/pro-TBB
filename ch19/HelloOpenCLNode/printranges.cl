
__kernel void cl_print(global float* A,
                       global float* B,
                       global float* C,
                       int rows) {
     int2 gId = (int2)(get_global_id(0), get_global_id(1));
     int2 lId = (int2)(get_local_id(0), get_local_id(1));
     int2 grId = (int2)(get_group_id (0), get_group_id (1));
     int2 gSize = (int2)(get_global_size(0), get_global_size(1));
     int2 lSize = (int2)(get_local_size(0), get_local_size(1));
     int2 numGrp = (int2)(get_num_groups (0), get_num_groups (1));
     if(gId.x == 0 && gId.y==0)
       printf("gSize.x=%d, gSize.y=%d, lSize.x=%d, lSize.y=%d,numGrp.x=%d, numGrp.y=%d\n",
               gSize.x,gSize.y,lSize.x,lSize.y,numGrp.x,numGrp);
     printf("gId.x=%d, gId.y=%d, lId.x=%d, lId.y=%d, grId.x=%d, grId.y=%d \n\n",
             gId.x,gId.y,lId.x,lId.y,grId.x,grId.y);
     C[gId.x*rows+gId.y]=A[gId.x*rows+gId.y]+B[gId.x*rows+gId.y];
}
