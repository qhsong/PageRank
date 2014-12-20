#include<iostream>
#include<fstream>
#include<map>
#include<algorithm>

#include"Eigen/Sparse"
#include"Eigen/Dense"

using namespace std;
using namespace Eigen;

int cmp(const pair<int,double> &x,const pair<int,double> &y) {
	return x.second >y.second;
}

int main(int argc,char **argv){
	ifstream matrix("result2.txt"),urls("urls.txt");
	SparseMatrix<int> linkRelation(361070,361070);
	vector<Triplet<int> > tris;
	map<string,int> dict;
	int i=0,count=0;
	int cur = 100;
	string s;
	if(matrix.is_open() && urls.is_open()){
		while(!urls.eof()){
			urls>>i>>s;
			dict.insert(map<string,int>::value_type(s,i));
			count++;
		}
		while(!matrix.eof()){
			int count;
			string s1,s2;
			int f1,f2;
			matrix>>s1>>s2;

			f1 = dict[s1]; 
			f2 = dict[s2];	
			tris.push_back(Triplet<int>(f2,f1,1));

		}
		linkRelation.setFromTriplets(tris.begin(),tris.end());
		tris.clear();
		cout<<"Create finished"<<" "<<count<<endl;

		int count = i;
		int j;
		int tmpcount = 0;
		vector<Triplet<double> > gtris;
		for(int k=0 ; k<linkRelation.outerSize();++k){
			int pointcount = 0;
			for(SparseMatrix<int>::InnerIterator it(linkRelation,k); it;++it) {
				if(it.value())	pointcount++;
			}
			for(SparseMatrix<int>::InnerIterator it(linkRelation,k); it;++it) {
				gtris.push_back(Triplet<double>(it.col(),it.row(),(double)1.0/(double)pointcount));
			}
		}
		SparseMatrix<double> Gr(361070,361070);
		Gr.setFromTriplets(gtris.begin(),gtris.end());
		gtris.clear();
		SparseMatrix<double> Grinve = Gr.transpose();
		
		VectorXd r(361070);
		for(int i=0;i<count;i++) {
			r(i) = 1.0;
		}
		double constNum = 0.15/(double)count;
		double maxIt = 10000.0;
		int icount = 0;
		while( maxIt > 0.00001){
			//computer rN
			vector<pair<int,double> > pair_vec;
			for(int i=0;i<count;i++) {
				pair_vec.push_back(pair<int,double>(i,r(i)));
			}
		
			sort(pair_vec.begin(),pair_vec.end(),cmp);
			for(int i=0;i<5;i++) {
				string a;
				for(map<string,int>::iterator it = dict.begin();
								it!= dict.end();
								it++){
					if(it->second==pair_vec[i].first){
						a = it->first;
					}
				}
				cout<<pair_vec[i].first<<" "<<a<<" "<<pair_vec[i].second<<endl;
			}
			cout<<"======================"<<endl;

			double tmp = r.sum() * constNum;
			VectorXd rb = r;				
			r = 0.85 * Grinve * r;
			for(int i=0;i<count;i++){
				r(i) += tmp;
			}

			rb = rb - r;
			if(icount++!=0){
				maxIt = 0.0;
				for(int i=0;i<count;i++) {
					double tmp = abs(rb(i));
					if(maxIt<tmp) maxIt = tmp;
				}
			}			
			cout<<"Test rb and r:"<<endl;
			for(int i=0;i<5;i++) {
				cout<<rb(i)<<" "<<r(i)<<endl;
			}

			cout<<"This max int is:"<<maxIt<<endl;	

		}
		cout<<"Computer end"<<endl;
	}else{
		cout<<"Can't open the urls file"<<endl;
	}
	return 0;
}
