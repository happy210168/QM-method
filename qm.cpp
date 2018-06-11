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
vector<vector<string> > group,tmpgroup,nextgroup;
vector<int> minterm;
vector<string> prime;
fstream ifp,ofp;

//function 
void X_pos(string str1); //check '-' postion
void divide(); //transform input minterm into 10 groups store in vector<vector<string> >group 
void forloop(int count,string str1); //Generate all combination of don't care bits and transform to minterm store in B
int btod(string str1);	//binary to decimal
void print_min(string str1,int a,int b);
string invstr(string str1); //reverse input string
int onebit(string str1,string str2);  //1-bit different 
string combine(string str1,string str2);
void print_round();
int QM();
string convert_bit(string str1);

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

	while(1){
		if(QM()){
			continue;
		}//if end
	 	else break;	
	} //ROUND2~

	cout << "================================Result================================"<< endl;
	cout << "                    |" ;
	for(i=0;i<minterm.size();i++) cout << "  " << minterm.at(i);
	cout << endl;
	cout << "--------------------+-------------------------------------------------" << endl;
		
	return 0;
}

void print_round(){
		printf("=========ROUND%d=========\n",round);
		for(int i=0;i<11;i++){
			for(int j=0;j<group.at(i).size();j++){
				print_min(group.at(i).at(j),i,j);
			}
			if(group.at(i).size()!=0) cout << "------------------------" << endl;
		}
		round++;
}


int QM(){
	tmpgroup.clear();
	tmpgroup.resize(11);
	tmpgroup = group;
	nextgroup.clear();
	nextgroup.resize(11);
	unsigned int a=0,b=0,c=0,count=0;
	for(a=0;a<group.size()-1;a++){
		if(!group.at(a).size()) continue;
		for(b=0;b<group.at(a).size();b++){
			for(c=0;c<group.at(a+1).size();c++){
				if(onebit(group.at(a).at(b),group.at(a+1).at(c))){
					string str1,str2,result;
					str1 = group.at(a).at(b); 
					str2 = group.at(a+1).at(c);
					result = combine(str1,str2);
					count ++;
					nextgroup.at(a).push_back(result);
					if(tmpgroup.at(a).at(b).front()!='v') tmpgroup.at(a).at(b) = "v " + tmpgroup.at(a).at(b);//tmpgroup.at(a).at(b).push_back('v'); 
					if(tmpgroup.at(a+1).at(c).front()!='v') tmpgroup.at(a+1).at(c) = "v " + tmpgroup.at(a+1).at(c);//tmpgroup.at(a+1).at(c).push_back('v');
				} //if(onebit) 
			} //for loop c
		} //for loop b
	} //for loop a
	for(a=0;a<group.size();a++){
		if(!group.at(a).size()) continue;
		for(b=0;b<group.at(a).size();b++){
			if(tmpgroup.at(a).at(b).front() != 'v'){
				prime.push_back(tmpgroup.at(a).at(b));
			}
		}
	}
	print_round();
	group.clear();
	group.resize(11);
	group = nextgroup;
	if(count==0){		
		return 0; 
	}
	else return 1;
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
		if(str1.at(i)!=str2.at(i)) count++;
	}
	if(count>1) return 0;
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

void print_min(string str1,int a,int b){ //print [str] : minterm  e.g. 0000000000- : 0,1
	X_pos(str1);
	B.clear();
	if(n!=0) forloop(n,str1);
	if(tmpgroup.at(a).at(b).at(0)=='v') cout << "v "<< str1 << ": ";
	else if(b>0 &&group.at(a).at(b-1) == group.at(a).at(b)) cout << "x "<< str1 << ": ";
	else cout << "  " << str1 << ": ";
	//cout << "　"<< str1 << ": ";

	if(n!=0){
	for(int i=0;i<B.size();i++){
		if(i!=B.size()-1) cout << B.at(i) << ",";
		else cout << B.at(i) << endl;
	}
	}
	else cout << " " << btod(str1) << endl;
}

void prime_term(string str1){
	X_pos(str1);
	B.clear();
	if(n!=0) forloop(n,str1);
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

string convert_bit(string str1){
	string bit;
	for(int i=0;i<str1.size();i++){
		if(str1.at(i)=='0'){
			bit.push_back('a'+i);
			bit.push_back('"');
		}
		if(str1.at(i)=='1'){
			bit.push_back('a'+i);
		}
	}
	return bit;
}

