
 // sgsize must be >= 5 && <= 10
 #include <stdio.h>


int CliqueCount(int *g,int gsize)
{
    int i;
    int j;
    int k;
    int l;
    int m;
    int n;
    int o;
    int p;
    int q;
    int r;
    int count=0;
    int sgsize = 10;
    
    
    for(i=0;i < gsize-sgsize+1; i++)
    {
	for(j=i+1;j < gsize-sgsize+2; j++)
        {
	    for(k=j+1;k < gsize-sgsize+3; k++) 
            { 
		if((g[i*gsize+j] == g[i*gsize+k]) && 
		   (g[i*gsize+j] == g[j*gsize+k])) 
		{
		    for(l=k+1;l < gsize-sgsize+4; l++) 
		    { 
			if((g[i*gsize+j] == g[i*gsize+l]) && 
			   (g[i*gsize+j] == g[j*gsize+l]) && 
			   (g[i*gsize+j] == g[k*gsize+l])) 
			{
			    for(m=l+1;m < gsize-sgsize+5; m++) 
			    {
				if((g[i*gsize+j] == g[i*gsize+m]) && 
				   (g[i*gsize+j] == g[j*gsize+m]) &&
				   (g[i*gsize+j] == g[k*gsize+m]) && 
				   (g[i*gsize+j] == g[l*gsize+m])) 
				{
					if(sgsize <= 5) {
						count++;
					} else {
				  		for(n=m+1;n<gsize-sgsize+6;n++)
						{
						  if
					  ((g[i*gsize+j] == g[i*gsize+n]) && 
					   (g[i*gsize+j] == g[j*gsize+n]) &&
					   (g[i*gsize+j] == g[k*gsize+n]) && 
					   (g[i*gsize+j] == g[l*gsize+n]) &&
					   (g[i*gsize+j] == g[m*gsize+n]))
						    {
							if(sgsize <= 6){
						      count++;
							} else {
						for(o=n+1;o<gsize-sgsize+7;o++){
						  if
					  ((g[i*gsize+j] == g[i*gsize+o]) && 
					   (g[i*gsize+j] == g[j*gsize+o]) &&
					   (g[i*gsize+j] == g[k*gsize+o]) && 
					   (g[i*gsize+j] == g[l*gsize+o]) &&
					   (g[i*gsize+j] == g[m*gsize+o]) &&
					   (g[i*gsize+j] == g[n*gsize+o])) {
						if(sgsize <= 7) {
							count++;
						} else {
						for(p=o+1;p<gsize-sgsize+8;p++){
						  if
					  ((g[i*gsize+j] == g[i*gsize+p]) && 
					   (g[i*gsize+j] == g[j*gsize+p]) &&
					   (g[i*gsize+j] == g[k*gsize+p]) && 
					   (g[i*gsize+j] == g[l*gsize+p]) &&
					   (g[i*gsize+j] == g[m*gsize+p]) &&
					   (g[i*gsize+j] == g[n*gsize+p]) &&
					   (g[i*gsize+j] == g[o*gsize+p])) {
						if(sgsize <= 8) {
							count++;
						} else {
						for(q=p+1;q<gsize-sgsize+9;q++){
						  if
					  ((g[i*gsize+j] == g[i*gsize+q]) && 
					   (g[i*gsize+j] == g[j*gsize+q]) &&
					   (g[i*gsize+j] == g[k*gsize+q]) && 
					   (g[i*gsize+j] == g[l*gsize+q]) &&
					   (g[i*gsize+j] == g[m*gsize+q]) &&
					   (g[i*gsize+j] == g[n*gsize+q]) &&
					   (g[i*gsize+j] == g[o*gsize+q]) &&
					   (g[i*gsize+j] == g[p*gsize+q])) { 
						if(sgsize <= 9) {
							count++;
						} else {
					for(r=q+1;r<gsize-sgsize+10;r++){
						  if
					  ((g[i*gsize+j] == g[i*gsize+r]) && 
					   (g[i*gsize+j] == g[j*gsize+r]) &&
					   (g[i*gsize+j] == g[k*gsize+r]) && 
					   (g[i*gsize+j] == g[l*gsize+r]) &&
					   (g[i*gsize+j] == g[m*gsize+r]) &&
					   (g[i*gsize+j] == g[n*gsize+r]) &&
					   (g[i*gsize+j] == g[o*gsize+r]) &&
					   (g[i*gsize+j] == g[p*gsize+r]) &&
					   (g[i*gsize+j] == g[q*gsize+r])) { 
							count++;
						}
					   }
						}
					}
					}
					}
					}
					}
					}
					}
					}
					}
					}
					}
					}
				}
			    }
			}
		    }
		}
	    }
         }
     }
    return(count);

}




int main(void){
	printf("Ramsey test\n");
	int g[10][10] = {
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}, 
		{0,0,0,0,0,0,0,0,0,0}
	};
	


	int ans = CliqueCount(g, 10);
	printf("Answer: %d\n", ans);
	
}
