__kernel void hello(__global char* string)
{
    __local myBuffer[100];

    int idx;
//    idx = get_global_id(0);
//    myBuffer[idx] = idx;
//
//    string[idx] = idx*2;


    for( idx = 0; idx < 100; idx++)
    {
        string[idx] = idx;
        myBuffer[idx] = idx*2;
    }

    for( idx = 0; idx < 100; idx++)
    {
        string[idx] += myBuffer[idx];
    }



//    string[0] = 1;
//    string[1] = 2;
//    string[2] = 3;
//    string[3] = 4;
//    string[4] = 5;
//    string[5] = 5;
//    string[6] = 6;
//    string[7] = 7;
//    string[8] = 8;
//    string[9] = 9;
//    string[10] = 10;
//    string[11] = 11;
//    string[12] = 12;
//    string[13] = 13;

    //string[idx] += myBuffer[idx];
}






