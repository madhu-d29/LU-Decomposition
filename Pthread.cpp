#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <chrono>
#include <vector>
#define d double
#define z for(int i = 0; i < n; i++)
#define y for(int j = 0; j < n; j++)
using namespace std ;
double **a;
double **a_orig;
double **u;
double **l;
int n;
int cores;
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
void* assignParallel(void* pair_id_k){
	
	int id = (*(pair<int,int> *)pair_id_k).first ;
	int k = (*(pair<int,int> *)pair_id_k).second ;
	
	int total=n-1-k ;	
	
	int start = (k+1)+id*total/cores ;
	int end = min((k+1)+(id+1)*total/cores,n) ;
	for(int i=start;i<end;i++){		
		for(int j=k+1;j<n;j++){
			a[i][j] -= l[i][k]*u[k][j];
		}
	}
}
int main(int argc, char *argv[]) {
ios::sync_with_stdio(0);
	cin.tie(nullptr); 
     cout.tie(nullptr);

	 string arg1= argv[1] ;
    string arg2= argv[2] ;
	
	cores= stoi(arg1);
	n = stoi(arg2);
for(int i=n; i<500;i++)
{
 for(int cores=1;cores<=4;cores++)
{
    a = new d* [n];
	a_orig = new d* [n];
	u = new d* [n];
	l = new d* [n];

	auto start = std::chrono::high_resolution_clock::now();	
	
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
		for(int i = 0; i < k; i++){
			tmp2 = l[k][i];
			l[k][i] = l[kdash][i];
			l[kdash][i] = tmp2;
		}
		
		u[k][k] = a[k][k];
		
		for(int i = k+1; i < n; i++){
		   l[i][k] = a[i][k]*1.0/u[k][k];
		   u[k][i] = a[k][i];
		}

		pthread_t thread[cores] ;
		
		pair<int,int> arm[cores] ;
		for(int i=0;i<cores;i++){
			arm[i]= make_pair(i,k) ;
		}
		
		for(int i=0;i<cores;i++){
			pthread_create(&thread[i], NULL, &assignParallel, (void*)&arm[i]); 
		}
		
		for(int i=0;i<cores;i++)
			pthread_join(thread[i],NULL) ;
		
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
