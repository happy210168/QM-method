#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdio>
#include<cstdlib>
using namespace std;

//variable
int n=0; //'-' numbers
int k[11]={0},j=0;
int round=1;
vector<int> A(10,0); //store '-' position
vector<int> B; //store minterm 
vector<vector<string> > group;
vector<vector<string> > tmpgroup;
vector<vector<string> > nextgroup;
vector<int> minterm;
vector<string> prime;
fstream ifp,ofp;
string str("11111111--");

//function 
void X_pos(string str1); //check '-' postion
void divide(); //transform input minterm into 10 groups store in vector<vector<string> >group 
void forloop(int count,string str1); //Generate all combination of don't care bits and transform to minterm store in B
int btod(string str1);	//binary to decimal
void print_min(string str1);
string invstr(string str1); //reverse input string
int onebit(string str1,string str2);  //1-bit different 
string combine(string str1,string str2);
int QM();


int main(){
	int i=0;
	int temp;
	group.resize(11);
	nextgroup.resize(11);
	//file processing
	ifp.open("input.txt",ios::in);
	ofp.open("output.txt",ios::out);
	if(!ifp || !ofp){ 
		cerr << "Failed to open file" << endl;
		exit(1);
	}
	
	//read input file
	while(ifp >> temp){
		if(i+1 > minterm.size()) minterm.resize(i+1);
		minterm.at(i) = temp;
		i++;
	}
			
	for(i=0;i<minterm.size();i++){
		ofp << minterm.at(i) << endl;
	}

	divide(); //ROUND1(first divide)
	printf("=========ROUND%d=========\n",round);
	for(int i=0;i<11;i++){
		for(int j=0;j<group.at(i).size();j++){
			print_min(group.at(i).at(j));
		}
		if(group.at(i).size()!=0) cout << "------------------------" << endl;
	}

	while(1){
		if(QM()){
			round++;
			printf("=========ROUND%d=========\n",round);
			for(int i=0;i<11;i++){
				for(int j=0;j<group.at(i).size();j++){
					print_min(group.at(i).at(j));
				}
				if(group.at(i).size()!=0) cout << "------------------------" << endl;
			}
		}//if end
	 	else break;	
	} //ROUND2
		

	
	
	return 0;
}

int QM(){
	int a,b,c,count=0;
	tmpgroup = group;
	for(a=0;a<11;a++){
		for(b=0;b<group[a].size();b++){
			for(c=0;c<group[a+1].size();c++){
				if(onebit(group[a].at(b),group[a+1].at(c))){
					count++;
					nextgroup[a].push_back(combine(group[a].at(b),group[a+1].at(c)));
					tmpgroup[a].at(b).at(10) = 'v';
					tmpgroup[a+1].at(c).at(10) ='v';
				} //if(onebit) 
			
			} //for loop k

		} //for loop j

	} //for loop i
	for(a=0;a<11;a++){
		for(b=0;b<tmpgroup[a].size();b++){
			if(tmpgroup[a].at(b).size()==10) prime.push_back(tmpgroup[a].at(b));
		}
	}
	group = nextgroup;
	if(count==0){
		
		return 0; 
	}
	return 1;
} //QM function end


string invstr(string str1){
	string inv;
	inv.resize(str1.size());
	int s = str1.size();
	for(int i=0;i<str1.size();i++,s--){
		inv.at(i) = str1.at(s-1);
	}
	return inv;
}

void divide(){
	int tmp;
	int count=0; //count '1' in str
	for(int i=0;i<minterm.size();i++){
		count=0;
		string str1;	
		tmp = minterm.at(i);
		//transform decimal to binary and store in group
		for(int j=0;j<10;j++){
			if((tmp>>j) & 1){
				str1.push_back('1');
				count++;
			}
			else str1.push_back('0');
		}	
		group.at(count).push_back(invstr(str1));				
	}
	

	
}

int onebit(string str1,string str2){  //onebit different
	int count=0;
	for(int i=0;i<10;i++){
		if(str1.at(i)!=str2.at(i))
			count++;
		if(count>1) 
			return 0;
	}
	return 1;
}

string combine(string str1,string str2){ //combine two str and store into nextgruop
	int df;
	for(int i=0;i<10;i++){
		if(str1.at(i)!=str2.at(i)){
			df = i;
		}
	}
	str1.at(df) = '-';	
	return str1;
}


void forloop(int count,string str1){   //input '-' numbers in str
	for(k[count]=0;k[count]<2;k[count]++){
		if(count==1){	
			for(j=1;j<=n;j++){
				str1.at(A[j-1]) = k[j] + '0';				
			}
			B.push_back(btod(str1));
	  }	
	else forloop(count-1,str1);  //use recursion to generate all combination of don't care bits
	}
}

void print_min(string str1){ //print [str] : minterm  e.g. 0000000000- : 0,1
	X_pos(str1);
	B.clear();
	if(n!=0) forloop(n,str1);
	cout << " " <<str1 << ":";
	if(n!=0){
	for(int i=0;i<B.size();i++){
		if(i!=B.size()-1) cout << " " << B.at(i) << ",";
		else cout << B.at(i) << endl;
	}
	}
	else cout << " " << btod(str1) << endl;
}

void X_pos(string str1){
	n=0;
	for(int i=0;i<10;i++) A[i]=0;
	for(int i=0;i<10;i++){
		if(str1.at(i)=='-')	n++; //compute '-'
	}	
	for(int i=0,j=0;i<10;i++){
		if(str1.at(i)=='-'){
			A[j] = i;
		 	j++;	//assign postion to A
		}
	}
}

int btod(string str1){
	int d=0;
	for(int i=0;i<10;i++){
		if(str1.at(i)=='1'){
			d += 1<<(9-i);
		}
	}
	return d;
}
