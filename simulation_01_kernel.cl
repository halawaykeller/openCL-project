__kernel void simulation_01(
        __global int nAtom,
        __global float* xAtoms,
        __global int* nPartners,
        __global int** iPartner
//        __global Params params,
//        __global int nSteps,
//        __global float dt
)

{
    __local int iwg; //= get the work group address here
    __local int iwi; // = get the work item address here
    __local float x1 = xAtom[iwg];
    __local float x2 = xAtom[ iPartner[ iwi + nPartners[iwg]*iwg ];
//    __local float A = params[0];
//    __local float d = params[1];
//    __local float r12 = x1 - x2;
//    __local float f1 = A*r12*dt/(r12*r12 + d);
}
