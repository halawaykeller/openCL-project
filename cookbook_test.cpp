// cookbook_test from OpenCl Parallel Programming Development Cookbook

#define APPLE

#include <stdio.h>
#include <stdlib.h>

#ifdef APPLE
#include <OpenCl/cl.h>
#else
#include <CL/cl.h>
#endif

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

    return 0;


}