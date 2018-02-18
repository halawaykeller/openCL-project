__kernel void hello(__global char* string)
{
    __local char buffer[100];

    int idx;
    idx = get_local_id(0);

    buffer[idx] = idx;

    string = buffer;
}






