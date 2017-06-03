/*
* =====================================================================================
*
*       Filename:  main.c
*
*    Description:  
*
*        Version:  1.0
*        Created:  02/06/2017 01:58:44
*       Revision:  none
*       Compiler:  gcc
*
*         Author:  Zakaria ElQotbi (zakariae), zakaria.elqotbi@harman.com
*        Company:  Harman
*
* =====================================================================================
*/


int main(int argc,char** argv)
{
    FILE* in;
    uint8_t buf[512]; 

    if(argc<2){
	printf("please provide file\n");
	exit(1);
    }
    in = fopen(argv[1],"ro");

    fread(buf,512,0,in);

    fclose(in);
}
