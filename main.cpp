#include <iostream>
#include <stdio.h>
#include <opencl.h>
#include <com_err.h>
#include <memory.h>
#include <cmath>

using namespace std;

int main()
{

    std::cout << "Hello, Kimberly" << std::endl;
    FILE* out = fopen( "test.out", "w" );
    fprintf( out, "Hello. Papa" );
    fclose( out );

/*
//    cl_int oclGetPlatformID (cl_platform_id *platforms);

    cl_int clGetDeviceIDs (cl_platform_id platform,
                           cl_device_type device_type,
                           cl_uint num_entries,
                           cl_device_id *devices,
                           cl_uint *num_devices
    );

    cl_context clCreateContext ( const cl_context_properties *properties,
                                 cl_uint num_devices,
                                 const cl_device_id *devices,
                                 void (*pfn_notify) (const char *errinfo, const void *private_info, size_t cb, void *user_data ),
                                 void *user_data,
                                 cl_int *errcode_ret
    );

    cl_command_queue clCreateQueue ( cl_context context,
                                     cl_device_id device,
                                     cl_command_queue_properties properties,
                                     cl_int *errcode_ret

    );
*/


    cl_int error = 0;
    cl_platform_id platform;
    cl_context context;
    cl_command_queue queue;
    cl_device_id device;
    cl_uint num_entries = 1;
    cl_uint num_platforms[1];


   /* cl_int clGetPlatformIDs(	cl_uint num_entries,
                                cl_platform_id *platforms,
                                cl_uint *num_platforms)
*/

    // setup platform, device, queue

    error = clGetPlatformIDs( num_entries, &platform, num_platforms );

    int i;
    printf( "num_entries %d\n", num_entries );
    //printf( "platform %d\n", platform );
    for( i=0; i<num_entries; i++ )
    {
        printf( "%d, %d\n", i, num_platforms[i] );
    }

    if( error != CL_SUCCESS ){ cout << "Error getting platform id: " << /*error_message(error) << */ endl; exit( error ); }

    error = clGetDeviceIDs ( platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL );

    if( error != CL_SUCCESS ){ cout << "Error getting device ids: " << endl; exit( error ); }

    context = clCreateContext( 0, 1, &device, NULL, NULL, &error );

    if( error != CL_SUCCESS ){ cout << "Error creating context: "  << endl; exit( error ); }

    queue = clCreateCommandQueue( context, device, 0, &error );

    if( error != CL_SUCCESS ){ cout << "Error creating command queue: "  << endl; exit( error ); }

    // memory allocation

    int size = 12;
    float *src_a_h = new float[size];
    float *src_b_h = new float[size];
    float *res_h   = new float[size];

    // initialize

    for( i=0; i<size; i++ )
    {
        src_a_h[i] = (float)i;
        src_b_h[i] = (float)i;
    }

    // Allocates a buffer of size mem_size and copies mem_size bytes from src_a_h

    int mem_size = sizeof(float)*size;

    cl_mem src_a_d = clCreateBuffer( context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_a_h, &error );
    cl_mem src_b_d = clCreateBuffer( context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, src_b_h, &error );
    cl_mem res_d = clCreateBuffer( context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mem_size, res_h, &error );

    // Creates the program
    // Uses NVIDIA helper functions to get the code string and it's size (in bytes)

    size_t  src_size = 0;

    // int filesize = 0;
    // const char *path = shrFindFilePath( "vector_add_gpu.cl", NULL);
    // const char *source = oclLoadProgSource(path, "", &src_size, &error);

    FILE* kernelfile = fopen("/Users/khaleesi/Desktop/c-project/test_kernel.cl", "r");
    while( fgetc(kernelfile) != EOF ) src_size++;
    //char *fakekernel = (char*)calloc( src_size+1, sizeof( char ) );
    char *fakekernel = (char*)calloc( src_size, sizeof( char ) );

    rewind(kernelfile);
    fread( fakekernel, src_size, sizeof(char), kernelfile );
//    i = 0;
//    while(  ( fakekernel[i] = fgetc(kernelfile) ) != EOF ) i++;
    //fakekernel[src_size] = '\0';

    fclose( kernelfile );

    const char* kernel = fakekernel;

    out = fopen( "debug.out", "w" );
    //fprintf( out, "%s", kernel );
    //fclose( out );

//    clCreateProgramWithSource(  cl_context        /* context */,
//                                cl_uint           /* count */,
//                                const char **     /* strings */,
//                                const size_t *    /* lengths */,
//                                cl_int *          /* errcode_ret */) CL_API_SUFFIX__VERSION_1_0;

    cl_program  program = clCreateProgramWithSource(context, 1, &kernel, &src_size, &error);
    //assert(error == CL_SUCCESS);

    // Shows the log
    char* build_log;
    size_t log_size;

    // First call to know the proper size
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
    build_log = new char[log_size + 1];

    // Second call to get the log
    clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, build_log, NULL);
    build_log[log_size] = '\0';

    fprintf( out, "build_log:  %s\n\n", build_log );
    cout << build_log << endl;
    delete[] build_log;

    fprintf( out, "src_a_d  %p    src_b_d  %p   res_d  %p\n\n", src_a_d, src_b_d, res_d );


    // Extracting the kernel
    cl_kernel  vector_add_kernel = clCreateKernel(program, "test_kernel.cl", &error);
    // assert(error == CL_SUCCESS);

    // Enqueueing parameters
    // Note that we inform the size of the cl_mem object, not the size of the memory pointed by it

    cl_kernel vector_add_k;
    error = clSetKernelArg( vector_add_k, 0, sizeof( cl_mem ), &src_a_d );
    error = clSetKernelArg( vector_add_k, 1, sizeof( cl_mem ), &src_b_d );
    error = clSetKernelArg( vector_add_k, 2, sizeof( cl_mem ), &res_d );
    error = clSetKernelArg( vector_add_k, 3, sizeof( size_t ), &size );

    // Launching kernel
    const size_t local_ws = 512;

    // shrRoundUp returns the smallest multiple of local_ws bigger than size
    const size_t global_ws = 512*(floor( size/512 )+1);

    error = clEnqueueNDRangeKernel( queue, vector_add_k, 1, NULL, &global_ws, &local_ws, 0, NULL, NULL );
    // assert( error == CL_SUCCESS );

    // Reading back
    float* check = new float[size];


    for( i=0; i<size; i++ )
        check[i] = (float)i;

    clEnqueueReadBuffer( queue, res_d, CL_TRUE, 0, mem_size, check, 0, NULL, NULL );

    for( i=0; i<size; i++ )
        fprintf( out, "%8.4lf  %8.4lf  %8.4lf\n", check[i], src_a_h[i], src_b_h[i] );
    fclose( out );

    // Cleaning Up
    delete[] src_a_h;
    delete[] src_b_h;
    delete[] res_h;
    delete[] check;

    clReleaseKernel( vector_add_k );
    clReleaseCommandQueue( queue );
    clReleaseContext( context );
    clReleaseMemObject( src_a_d );
    clReleaseMemObject( src_b_d );
    clReleaseMemObject( res_d );






    return 0;
}