// cookbook_test from OpenCl Parallel Programming Development Cookbook

#define APPLE

#include <stdio.h>
#include <stdlib.h>

#ifdef APPLE
#include <OpenCl/cl.h>
#else
#include <CL/cl.h>
#endif


#define MEM_SIZE (128)
#define MAX_SOURCE_SIZE (0x100000)


FILE* debug;

void displayPlatformInfo( cl_platform_id id,
                          cl_platform_info param_name,
                          const char* paramNameAsStr )
{
    cl_int error = 0;
    size_t paramSize = 0;

    error = clGetPlatformInfo( id, param_name, 0, NULL, &paramSize );

    fprintf( debug, "clGetPlatformInfo(): %d\n", paramSize );
    if( error != CL_SUCCESS )
    {
        //perror( "Error clGetPlatformInfo():" );
        fprintf( debug, "Error clGetPlatformInfo(): %d\n", paramSize );
    }

    char* moreInfo = (char*)alloca( sizeof(char) * paramSize );
    error = clGetPlatformInfo( id, param_name, paramSize, moreInfo, NULL );

    if( error != CL_SUCCESS )
    {
        perror( "Unable to find any OpenCL platform information" );
        return;
    }

    fprintf( debug, "%s: %s\n", paramNameAsStr, moreInfo );

}



int main()
{
    debug = fopen( "debug.out", "w" );

    cl_platform_id* platforms;
    cl_uint numOfPlatforms;
    cl_int error;

    error = clGetPlatformIDs( 0, NULL, &numOfPlatforms );
    if( error < 0 )
    {
        perror( "Unable to find any OpenCL platforms" );
        exit(1);
    }

    platforms = (cl_platform_id*)alloca( sizeof(cl_platform_id) * numOfPlatforms );
    fprintf( debug, "Number of OpenCL platforms found: %d\n", numOfPlatforms );

    error = clGetPlatformIDs( numOfPlatforms, platforms, NULL );
    if( error < 0 )
    {
        perror( "clGetPlatformIDs failed:" );
        exit(1);
    }

    for( cl_uint i = 0; i< numOfPlatforms; ++i )
    {
        displayPlatformInfo( platforms[i], CL_PLATFORM_PROFILE, "CL_PLATFORM_PROFILE" );
        displayPlatformInfo( platforms[i], CL_PLATFORM_VERSION, "CL_PLATFORM_VERSION" );
        displayPlatformInfo( platforms[i], CL_PLATFORM_NAME, "CL_PLATFORM_NAME" );
        displayPlatformInfo( platforms[i], CL_PLATFORM_VENDOR, "CL_PLATFORM_VENDOR" );
        displayPlatformInfo( platforms[i], CL_PLATFORM_EXTENSIONS, "CL_PLATFORM_EXTENSIONS" );
    }

    /* NEW STUFF */
    fprintf( debug, "got to 1\n");

    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    cl_mem memobj = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    //cl_platform_id platform_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;
    size_t mem_size = 50;

    char string[MEM_SIZE];

    fprintf( debug, "got to 2\n");

    FILE *fp;
    char fileName[] = "/Users/khaleesi/Desktop/c-project/kernel_test.cl";
    char *source_str;
    size_t source_size;

/* Load the source code containing the kernel*/
    fp = fopen(fileName, "r");
    if (!fp) {
        fprintf(stderr, "Failed to load kernel.\n");
        exit(1);
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    fprintf( debug, "got to 3\n");
/* Get Platform and Device Info */
    //ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

/* Create OpenCL context */
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

/* Create Command Queue */
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    fprintf( debug, "got to 4\n");
/* Create Memory Buffer */
    memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), NULL, &ret);

/* Create Kernel Program from the source */
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                        (const size_t *)&source_size, &ret);

/* Build Kernel Program */
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

/* Create OpenCL Kernel */
    kernel = clCreateKernel(program, "hello", &ret);

/* Set OpenCL Kernel Parameters */
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);

    fprintf( debug, "got to 5\n");
/* Execute OpenCL Kernel */
//    ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, 0, &mem_size, &mem_size, 0, NULL, NULL);

/* Copy results from the memory buffer */
    ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0,
                              MEM_SIZE * sizeof(char),string, 0, NULL, NULL);


    fprintf( debug, "got to 6\n");

/* Display Result */
    int i;
    for( i = 0; i<100; i++)
    {
        fprintf(debug, "%d\n", string[i]);
    }
    fclose(debug);

/* Finalization */
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(memobj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(source_str);

    return 0;


}