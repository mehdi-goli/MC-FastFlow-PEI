// Simple kernel which computes the inc of each element of the input vector
//
										
__kernel void inc(                                                
   __global float* input,
   __global float* output,
   const  uint max_items)
{        
    int i = get_global_id(0);
    if (i<max_items)
        output[i] = input[i] + 1;
}                                                                      ;
 
////////////////////////////////////////////////////////////////////////////////