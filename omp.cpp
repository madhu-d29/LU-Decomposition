#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <chrono>
#include <vector>
#include <omp.h>
#define d double
#define z for(int i = 0; i < n; i++)
#define y for(int j = 0; j < n; j++)
using namespace std ;

void print_result(d** m, int n){
	z{
	   y{
		   cout << m[i][j] << "  ";
	   }
	   cout<< endl;
   }
   cout<<endl;
	
}

d check_result(int n,d **a, d** l, d** u, vector<int> pvector){
	
	d **p; d **result;
	p = new d* [n]; result = new d* [n];
	z{
		p[i] = new d[n];
		result[i] = new d[n];
	}

	z{
		y{
			p[i][j] = 0.0;
			result[i][j] = 0.0;
		}
	}
	z{
		p[i][pvector[i]] = 1.0;
	}
	
	d tmp = 0.0;
	z{
		y{
			tmp = 0.0;
			for(int k = 0; k < n; k++){
				tmp = tmp + p[i][k]*a[k][j] - l[i][k]*u[k][j]; 
			}
			result[i][j] = tmp;
		}
	}
	
	d norm=0.0,colnorm = 0.0;
	z{
		colnorm = 0.0;
		for(int j = 0; j < n; j++){
			colnorm += result[i][j]*result[i][j];
		}
		norm += sqrt(colnorm);
	}
	
	return norm;
	
}

int main(int argc, char *argv[]) {
ios::sync_with_stdio(0);
	cin.tie(nullptr); 
     cout.tie(nullptr);

	string arg1= argv[1] ;
    string arg2= argv[2] ;
	
	int cores= stoi(arg1);
	int n = stoi(arg2);
        omp_set_num_threads(cores) ;
 for(int i=n; i<500;i++)
{
 for(int cores=1;cores<=4;cores++)
{
	
	d **a;
	a = new d* [n];
	d **a_orig;
	a_orig = new d* [n];
	d **u;
	u = new d* [n];
	d **l;
	l = new d* [n];

	auto start = std::chrono::high_resolution_clock::now();	
	#pragma omp parallel for
	z{
		a[i] = new d[n];
		u[i] = new d[n];
		l[i] = new d[n];
		a_orig[i] = new d[n];
	}
   
	vector<int> p(n,0);
	d drand48();

	z{
		p[i] = i;		
		for(int j = 0; j < n; j++){
			if( j < i ){
				u[i][j] = 0.0;
				l[i][j] = drand48();
			}
			else if(j == i){
				l[i][j] = 1.0;
				u[i][j] = drand48();
			}
			else{
				u[i][j] = drand48();
				l[i][j] = 0.0;
			}
			a[i][j] = drand48();
			a_orig[i][j] = a[i][j];		   
		}  
	}
	
	d maxx;
	int kdash;	
	
	for(int k = 0; k < n; k++){
		maxx = 0.0;
		auto startfor = std::chrono::high_resolution_clock::now();	
	#pragma omp parallel reduction(max:maxx)
		for(int i = k; i < n; i++){
			if(fabs(a[i][k])> maxx){
				maxx = fabs(a[i][k]);
				kdash = i;
			}
		}
		
		if(maxx==0.0){
			cerr<<"Singular matrix."<<endl;
		}
		
		int temp = p[k];
		p[k] = p[kdash];
		p[kdash] = temp;
		
		d* tmp = a[k];
		a[k] = a[kdash];
		a[kdash] = tmp;


		d tmp2=0.0;
		#pragma omp parallel for private(tmp2) if(k>500)
		for(int i = 0; i < k; i++){
			tmp2 = l[k][i];
			l[k][i] = l[kdash][i];
			l[kdash][i] = tmp2;
		}
		
		u[k][k] = a[k][k];
		#pragma omp parallel for if(k<n-500)
		for(int i = k+1; i < n; i++){
		   l[i][k] = a[i][k]*1.0/u[k][k];
		   u[k][i] = a[k][i];
		}

		d utemp[n-k-1] ;
		#pragma omp parallel for
		for(int i=0;i<n-k-1;i++){
			utemp[i]=u[k][i+k+1] ;
		}
		d *res_d ;
		d val ;
		int j=0;
		#pragma omp parallel for private(j,val,res_d)
		for(int i = k+1; i < n; i++){
			val=l[i][k] ;
			res_d=a[i] ;
			for(j = k+1; j<n; j++){
				res_d[j] -= val*utemp[j-k-1];	
			}
		}
	}
/*print_result(l,n);
  cout<<endl;
   print_result(u,n);
cout<<endl;
   print_result(a_orig,n); */

	auto end = std::chrono::high_resolution_clock::now();
	cout<<cores<<" "<<i<<" "<<chrono::duration_cast<chrono::microseconds>(end-start).count()/1000000.0<<endl ;
	}
}
	//d result = check_result(n,a_orig,l,u,p);
	//cout<<result<<endl;
	return 0;
}
