// cookbook_test from OpenCl Parallel Programming Development Cookbook

#define APPLE

#include <stdio.h>
#include <stdlib.h>

#ifdef APPLE
#include <OpenCl/cl.h>
#else
#include <CL/cl.h>
#endif


#define NUMBER_OF_ATOMS (16)
#define MAX_SOURCE_SIZE (0x100000)


FILE* debug;

void displayPlatformInfo( cl_platform_id id,
                          cl_platform_info param_name,
                          const char* paramNameAsStr )
{
    cl_int error = 0;
    size_t paramSize = 0;

    error = clGetPlatformInfo( id, param_name, 0, NULL, &paramSize );
//
//    fprintf( debug, "clGetPlatformInfo(): %\n", paramSize );
//    if( error != CL_SUCCESS )
//    {
//        //perror( "Error clGetPlatformInfo():" );
//        fprintf( debug, "Error clGetPlatformInfo(): %d\n", paramSize );
//    }

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
    int i,j;

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

    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    //cl_mem memobj = NULL;
    cl_mem gpu_nAtoms = NULL;
    cl_mem gpu_xAtom = NULL;
    cl_mem gpu_nPartners = NULL;
    cl_mem gpu_iPartner = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    size_t nAtoms = NUMBER_OF_ATOMS;
    float* xAtom = (float*)calloc(nAtoms, sizeof(float));
    for( i=0; i<nAtoms; i++ ){ xAtom[i] = i; }

    int* nPartners = (int*)calloc(nAtoms, sizeof(int));
    int NN = 0;
    for(i=0; i<nAtoms; i++){ nPartners[i] = 4; NN += nPartners[i]; }

    int* iPartner = (int*)calloc( NN, sizeof(int));
    for( i=0; i<nAtoms; i++ )
    {
        for( j=0; j<nPartners[i]; j++ )
        {
            iPartner[i*nPartners[i]+j] = 1;//i + (i - nPartners[i]/2 + j)%nAtoms;
        }
    }

    //cl_platform_id platform_id = NULL;
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret;

    FILE *fp;
    char fileName[] = "/Users/khaleesi/Desktop/c-project/simulation_01_kernel.cl";
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

/* Get Platform and Device Info */
    //ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

/* Create OpenCL context */
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

/* Create Command Queue */
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

/* Create Memory Buffer */
    //memobj = clCreateBuffer(context, CL_MEM_READ_WRITE,MEM_SIZE * sizeof(char), NULL, &ret);

    gpu_xAtom = clCreateBuffer(context, CL_MEM_READ_WRITE, nAtoms * sizeof(float), NULL, &ret);
    gpu_nPartners = clCreateBuffer(context, CL_MEM_READ_WRITE, nAtoms * sizeof(int), NULL, &ret);
    gpu_iPartner = clCreateBuffer( context, CL_MEM_READ_WRITE, NN * sizeof(int), NULL, &ret);

/* Create Kernel Program from the source */
    program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                        (const size_t *)&source_size, &ret);

/* Build Kernel Program */
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

/* Create OpenCL Kernel */
    kernel = clCreateKernel(program, "simulation_01", &ret);

/* Set OpenCL Kernel Parameters */
    //ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobj);
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&gpu_nAtoms);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&gpu_xAtom);
    ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&gpu_nPartners);
    ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&gpu_iPartner);

    ret = clEnqueueWriteBuffer(command_queue, gpu_nAtoms,    CL_TRUE, 0, sizeof(int),   &nAtoms,   0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, gpu_xAtom,     CL_TRUE, 0, nAtoms*sizeof(float), xAtom,     0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, gpu_nPartners, CL_TRUE, 0, nAtoms*sizeof(int),   nPartners, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, gpu_iPartner,  CL_TRUE, 0, NN*sizeof(int),   iPartner,  0, NULL, NULL);

/* Execute OpenCL Kernel */
//    ret = clEnqueueTask(command_queue, kernel, 0, NULL,NULL);
    ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, 0, &nAtoms, &nAtoms, 0, NULL, NULL);

/* Copy results from the memory buffer */
    //ret = clEnqueueReadBuffer(command_queue, memobj, CL_TRUE, 0, NUMBER_OF_ATOMS * sizeof(float),string, 0, NULL, NULL);
    ret = clEnqueueReadBuffer(command_queue, gpu_xAtom, CL_TRUE, 0, nAtoms * sizeof(float), xAtom, 0, NULL, NULL);

/* Display Result */
    for( i = 0; i<nAtoms; i++)
    {
        fprintf(debug, "%f\n", xAtom[i]);
    }
    fclose(debug);

/* Finalization */
    ret = clFlush(command_queue);
    ret = clFinish(command_queue);
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    //ret = clReleaseMemObject(memobj);
    ret = clReleaseMemObject(gpu_nAtoms);
    ret = clReleaseMemObject(gpu_xAtom);
    ret = clReleaseMemObject(gpu_nPartners);
    ret = clReleaseMemObject(gpu_iPartner);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    free(source_str);

    return 0;


}