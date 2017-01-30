/*
    Attackon5-rASCON.cpp
    Implementation of our conditional cube attack on 5-round ASCON
    One experiment takes a few seconds on a notebook.
    Times of experiment is set as ExpTimes. The program is run in CodeBlocks platform.
*/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long long int UINT64;
#define nrRounds 5
#define cubedim 16
#define RKN 64
#define random(x) (rand())%x
#define ROL64(a, offset) ((offset != 0) ? ((((UINT64)a) << offset) ^ (((UINT64)a) >> (64-offset))) : a)
#define ExpTimes 10//times of experiment



void AsconPermutation(UINT64 *state);
void AdditionofConstants(UINT64 *A,unsigned int indexround);
void SubstitutionLayer(UINT64 *A);
void LinearDiffusionLayer(UINT64 *A);

UINT64 AsconRoundConstants[12]={
0x0f00000000000000,0x8700000000000000,
0x4b00000000000000,0xc300000000000000,
0x2d00000000000000,0xa500000000000000,
0x6900000000000000,0xe100000000000000,
0x1e00000000000000,0x9600000000000000,
0x5a00000000000000,0xd200000000000000
};
void AsconPermutation(UINT64 *state){
    unsigned int i;
    for(i=0;i<nrRounds;i++){
        AdditionofConstants(state,i);
        SubstitutionLayer(state);
        LinearDiffusionLayer(state);
    }
}

void AdditionofConstants(UINT64 *A,unsigned int indexround){
    A[2]^=AsconRoundConstants[indexround];
}

void SubstitutionLayer(UINT64 *A){
    unsigned int i;
    UINT64 C[5];
    C[0] = A[4]&A[1] ^ A[3] ^ A[2]&A[1] ^ A[2] ^ A[1]&A[0] ^ A[1] ^ A[0];
    C[1] = A[4] ^ A[3]&A[2] ^ A[3]&A[1] ^ A[3] ^ A[2]&A[1] ^ A[2] ^ A[1] ^ A[0];
    C[2] = A[4]&A[3] ^ A[4] ^ A[2] ^ A[1] ^ 1;
    C[3] = A[4]&A[0] ^ A[4] ^ A[3]&A[0] ^ A[3] ^ A[2] ^ A[1] ^ A[0];
    C[4] = A[4]&A[1] ^ A[4] ^ A[3] ^ A[1]&A[0] ^ A[1];
    for(i=0;i<5;i++){
        A[i]=C[i];
    }
}

void LinearDiffusionLayer(UINT64 *A){
    unsigned int i;
    unsigned int offsets[5][2]={19,28,61,39,1,6,10,17,7,41};
    for(i=0;i<5;i++){
        A[i]=A[i]^ROL64(A[i],offsets[i][0])^ROL64(A[i],offsets[i][1]);
    }
}

int main(){

    srand(time(NULL));
    UINT64 Key[2]={0};

    UINT64 InitialState[5]={0,0,0,0,0};
    UINT64 TempState[5]={0};
    UINT64 FinalState;
    UINT64 RightState;

    unsigned int guesskey[2][RKN]={0};
    UINT64 GUESSKEY64[2]={0};

    unsigned int temp,counter;
    unsigned char flag[2][RKN];
    unsigned int index[cubedim];

    unsigned int i,j,k,l,aa,suc=0,remain=0,maxremain=0;


    for(aa=0;aa<ExpTimes;aa++){

    //this part corresponds to Algorithm 1 in the paper
    remain=0;
    for(i=0;i<2;i++){
        for(j=0;j<RKN;j++){
             flag[i][j]=0;
        }
    }
    Key[0]=0;Key[1]=0;
    for(i=0;i<2;i++){
        for(j=0;j<64;j++){
           temp=random(2);
           if(temp){
            Key[i]|=((UINT64)0x1<<j);
           }
        }
    }
	printf("right key:0x%I64x,0x%I64x\n",Key[0],Key[1]);


    InitialState[0]=0x0000000060300201;
    InitialState[1]=Key[0];
    InitialState[2]=Key[1];
    InitialState[3]=0;
    InitialState[4]=0;
    for(i=0;i<2;i++){
        for(j=0;j<64;j++){
           temp=random(2);
           if(temp){
            InitialState[3+i]|=((UINT64)0x1<<j);
           }
        }
    }
    for(i=0;i<RKN;i++){
        index[0]=i;
        index[1]=(5+i)%64;
        index[2]=(8+i)%64;
        index[3]=(12+i)%64;
        index[4]=(14+i)%64;
        index[5]=(15+i)%64;
        index[6]=(18+i)%64;
        index[7]=(19+i)%64;
        index[8]=(21+i)%64;
        index[9]=(27+i)%64;
        index[10]=(28+i)%64;
        index[11]=(30+i)%64;
        index[12]=(34+i)%64;
        index[13]=(37+i)%64;
        index[14]=(49+i)%64;
        index[15]=(50+i)%64;
        FinalState=0;
        for(j=0;j<(UINT64(1)<<cubedim);j++){
            for(k=0;k<5;k++){
                TempState[k]=InitialState[k];
            }
            for(k=0;k<cubedim;k++){
                TempState[3]&=(~(UINT64(1)<<index[k]));
                if((j>>k)&1){
                    TempState[4]|=(UINT64(1)<<index[k]);
                }
                else{
                    TempState[4]&=(~(UINT64(1)<<index[k]));
                }
            }
            AsconPermutation(TempState);
            FinalState^=TempState[0];
        }
        if(FinalState){
            guesskey[0][i]=1;
            flag[0][i]=1;
        }
    }


    for(i=0;i<RKN;i++){
        if(flag[0][i]==1){
            continue;
        }
        index[0]=i;
        index[1]=(5+i)%64;
        index[2]=(7+i)%64;
        index[3]=(8+i)%64;
        index[4]=(14+i)%64;
        index[5]=(15+i)%64;
        index[6]=(24+i)%64;
        index[7]=(27+i)%64;
        index[8]=(30+i)%64;
        index[9]=(34+i)%64;
        index[10]=(37+i)%64;
        index[11]=(41+i)%64;
        index[12]=(43+i)%64;
        index[13]=(49+i)%64;
        index[14]=(50+i)%64;
        index[15]=(52+i)%64;
        FinalState=0;
        for(j=0;j<(UINT64(1)<<cubedim);j++){
            for(k=0;k<5;k++){
                TempState[k]=InitialState[k];
            }
            for(k=0;k<cubedim;k++){
                TempState[3]&=(~(UINT64(1)<<index[k]));
                if((j>>k)&1){
                    TempState[4]|=(UINT64(1)<<index[k]);
                }
                else{
                    TempState[4]&=(~(UINT64(1)<<index[k]));
                }
            }
            AsconPermutation(TempState);
            FinalState^=TempState[0];
        }
        if(FinalState){
            guesskey[0][i]=0;
            flag[0][i]=1;
        }
    }


    for(i=0;i<RKN;i++){
        index[0]=i;
        index[1]=(1+i)%64;
        index[2]=(4+i)%64;
        index[3]=(5+i)%64;
        index[4]=(6+i)%64;
        index[5]=(8+i)%64;
        index[6]=(14+i)%64;
        index[7]=(15+i)%64;
        index[8]=(16+i)%64;
        index[9]=(17+i)%64;
        index[10]=(20+i)%64;
        index[11]=(26+i)%64;
        index[12]=(27+i)%64;
        index[13]=(29+i)%64;
        index[14]=(30+i)%64;
        index[15]=(33+i)%64;
        FinalState=0;
        for(j=0;j<(UINT64(1)<<cubedim);j++){
            for(k=0;k<5;k++){
                TempState[k]=InitialState[k];
            }
            for(k=0;k<cubedim;k++){
                TempState[4]&=(~(UINT64(1)<<index[k]));
                if((j>>k)&1){
                    TempState[3]|=(UINT64(1)<<index[k]);
                }
                else{
                    TempState[3]&=(~(UINT64(1)<<index[k]));
                }
            }
            if((TempState[3]>>i)&1){
                TempState[4]|=(UINT64(1)<<i);
            }
            if((TempState[3]>>i)&1==0){
                TempState[4]&=(~(UINT64(1)<<i));
            }
            AsconPermutation(TempState);
            FinalState^=TempState[0];
        }
        if(FinalState){
            if(i>55&&i<60){
                guesskey[1][i]=0;
                flag[1][i]=1;
            }
            else{
                guesskey[1][i]=1;
                flag[1][i]=1;
            }
        }
    }


    for(i=0;i<RKN;i++){
        if(flag[1][i]==1){
            continue;
        }
        if((i==0)||(i==9)||(i==20)||(i==21)||(i==29)||(i==30)){
            index[0]=i;
            index[1]=(5+i)%64;
            index[2]=(8+i)%64;
            index[3]=(14+i)%64;
            index[4]=(15+i)%64;
            index[5]=(16+i)%64;
            index[6]=(17+i)%64;
            index[7]=(20+i)%64;
            index[8]=(27+i)%64;
            index[9]=(29+i)%64;
            index[10]=(30+i)%64;
            index[11]=(33+i)%64;
            index[12]=(34+i)%64;
            index[13]=(35+i)%64;
            index[14]=(37+i)%64;
            index[15]=(38+i)%64;
            FinalState=0;
            for(j=0;j<(UINT64(1)<<cubedim);j++){
                for(k=0;k<5;k++){
                    TempState[k]=InitialState[k];
                }
                for(k=0;k<cubedim;k++){
                    if((j>>k)&1){
                        TempState[3]|=(UINT64(1)<<index[k]);
                    }
                    else{
                        TempState[3]&=(~(UINT64(1)<<index[k]));
                    }
                }

                AsconPermutation(TempState);
                FinalState^=TempState[0];
            }
            if(FinalState){
                if(i>55&&i<60){
                    guesskey[1][i]=1;
                    flag[1][i]=1;
                }
                else{
                    guesskey[1][i]=0;
                    flag[1][i]=1;
                }
            }
        }
        else{
            index[0]=i;
            index[1]=(5+i)%64;
            index[2]=(8+i)%64;
            index[3]=(14+i)%64;
            index[4]=(15+i)%64;
            index[5]=(27+i)%64;
            index[6]=(29+i)%64;
            index[7]=(30+i)%64;
            index[8]=(34+i)%64;
            index[9]=(36+i)%64;
            index[10]=(37+i)%64;
            index[11]=(38+i)%64;
            index[12]=(39+i)%64;
            index[13]=(45+i)%64;
            index[14]=(49+i)%64;
            index[15]=(50+i)%64;
            FinalState=0;
            for(j=0;j<(UINT64(1)<<cubedim);j++){
                for(k=0;k<5;k++){
                    TempState[k]=InitialState[k];
                }
                for(k=0;k<cubedim;k++){
                    if((j>>k)&1){
                        TempState[3]|=(UINT64(1)<<index[k]);
                    }
                    else{
                        TempState[3]&=(~(UINT64(1)<<index[k]));
                    }
                }

                AsconPermutation(TempState);
                FinalState^=TempState[0];
            }
            if(FinalState){
                if(i>55&&i<60){
                    guesskey[1][i]=1;
                    flag[1][i]=1;
                }
                else{
                    guesskey[1][i]=0;
                    flag[1][i]=1;
                }
            }
        }
    }

    for(i=0;i<2;i++){
        for(j=0;j<64;j++){
            if(flag[i][j]==0){
                remain++;
            }
        }
    }
	if(remain>maxremain){
        maxremain=remain;
    }
    //exhaustive search among the remaining key bits
    for(i=0;i<(1<<remain);i++){
        for(j=0;j<2;j++){
            for(k=0;k<64;k++){
                if(flag[j][k]==0){
                    guesskey[j][k]=0;
                }
            }
        }
        counter=0;
        for(j=0;j<2;j++){
            for(k=0;k<64;k++){
                if(flag[j][k]==0){
                    guesskey[j][k]=(i>>counter)&1;
                    counter++;
                }
            }
        }

        TempState[0]=0x0000000060300201;
        TempState[1]=Key[0];
        TempState[2]=Key[1];
        TempState[3]=0;
        TempState[4]=0;
        AsconPermutation(TempState);
        RightState=TempState[0];

        GUESSKEY64[0]=0;
        GUESSKEY64[1]=0;
        for(j=0;j<64;j++){
            if(guesskey[0][j]){
                GUESSKEY64[0]|=(UINT64(1)<<j);
            }
        }
        for(j=0;j<64;j++){
            if(guesskey[0][j]^guesskey[1][j]){
                GUESSKEY64[1]|=(UINT64(1)<<j);
            }
        }
        TempState[0]=0x0000000060300201;
        TempState[1]=GUESSKEY64[0];
        TempState[2]=GUESSKEY64[1];
        TempState[3]=0;
        TempState[4]=0;
        AsconPermutation(TempState);
        if(TempState[0]==RightState){
           break;
        }
    }
	printf("guess key:0x%I64x,0x%I64x\n",GUESSKEY64[0],GUESSKEY64[1]);

    //check whether the recovery is correct or not
    if(memcmp(Key,GUESSKEY64,2*sizeof(UINT64))==0){
        printf("right\n\n");
        suc++;
    }
    else{
        printf("wrong\n\n");

    }

    }
	printf("number of success:%d	number of experiments:%d\n",suc,ExpTimes);
    printf("maxremain(algorithm 1):%d\n",maxremain);
	printf("over");
	getchar();
    return 0;
}