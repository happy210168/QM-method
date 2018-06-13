#include<iostream>
#include<fstream>
#include<string>
#include<vector>
#include<cstdio>
#include<cstdlib>
#define N 10
using namespace std;

//variable
int n=0; //The number of '-' in a string  
int k[N+1]={0},j=0; //use in recursive forloop function
int round=1; //the number of round
int vline=0; //vertical line count in chart
vector<int> A(N,0); //store '-' position
vector<int> B; //store minterm
vector<vector<string> > group,tmpgroup,nextgroup; //store strings in every round
vector<int> minterm,Minterm,dontcare; //store input minterm 
string input ="    "; //store input string 
vector<string> prime; //store the string don't combine
fstream ifp,ofp; //file pointer
vector<vector<int> > Q; //2-D vector store result Chart
vector<int> index; //store row index (prime string index) in chart
vector<string> output; //function output result


//function 
void X_pos(string str1); //check '-' postion
void divide(); //transform input minterm into 10 groups store in vector<vector<string> >group 
void forloop(int count,string str1); //Generate all combination of don't care bits and transform to minterm store in B
int btod(string str1);	//binary to decimal
int bit_num(int num); //compute decimal bits
void print_min(string str1,int a,int b); //print out e.g. 111111111-: 1022,1023
string invstr(string str1); //reverse input string
int onebit(string str1,string str2);  //1-bit different 
string combine(string str1,string str2); //combine string to next round
void print_round(); //print out one round
void prime_term(string str1);	//store prime implicant's minterm 
void print_chart(); //print Result chart
void delete_duplicate_term(); //delete Duplicate term
int QM(); //compare with adjacent group 
void sort(vector<int>& vec); //sore vector 
string convert_bit(string str1); //e.g. convert 1111111111 to abcdefghij
int max_term(); //find index of prime implicant which have most X in chart 
void EPI(); //find essential prime implicant in chart and store in output
void PI(); //find prime implicant in chart and store in output
void vertical_line(); //draw vertical line in chart
int in(int u); //check this term index have been output or not
int isX(); //check don't care term 
int main(){
	int i=0;
	string temp;
	group.resize(N+1);
	nextgroup.resize(N+1);
	//file processing
	ifp.open("input.txt",ios::in);
	ofp.open("output.txt",ios::out);
	if(!ifp || !ofp){ 
		cerr << "Failed to open file" << endl;
		exit(1);
	}
	
	//read input file
	while(ifp >> input){
		if(input[0]==40){
			while(input[j]!=41) j++;
			for(int i=1;i<j;i++) temp[i-1] = input[i];
			minterm.push_back(atoi(temp.c_str()));
			dontcare.push_back(atoi(temp.c_str()));
		}
		else{
			minterm.push_back(atoi(input.c_str()));	//include don't care term
			Minterm.push_back(atoi(input.c_str())); //exclude don't care term
		}
	}

	sort(minterm);
	sort(Minterm);
	sort(dontcare);


	divide(); //ROUND1(Divide minterm into 11 group)

	while(1){
		if(QM()){
			continue;
		}//if end
	 	else break;	
	} //ROUND2~ROUNDn 
	
	delete_duplicate_term(); //delete Duplicate term in prime
	

	print_chart(); //print out Result

		
	return 0;
}

void print_chart(){
	int flag = 0;
	ofp << "====================================Result======================================="<< endl;
	ofp << "                     |" ;
	for(int i=0;i<Minterm.size();i++) ofp << "  " << Minterm.at(i);
	ofp << endl;
	ofp << "---------------------+-----------------------------------------------------------" << endl;
	
	for(int i=0;i<prime.size();i++){ 
		//abcdefghij term
		prime_term(prime.at(i));
		if(isX()) continue;
		
		ofp << convert_bit(prime.at(i));
		for(int a=0;a<21-convert_bit(prime.at(i)).size();a++) ofp << " ";
		ofp << "|" ;
		//"x"" or space " "
		for(int a=0;a<Minterm.size();a++){
			for(int b=0;b<B.size();b++){
				if(Minterm.at(a)== B.at(b)) flag=1;
			}
			if(flag){ofp << " "; for(int c=0;c<bit_num(Minterm.at(a));c++) ofp << " "; ofp << "x";  }
		 	else {ofp << "  "; for(int c=0;c<bit_num(Minterm.at(a));c++) ofp << " ";	}
			flag =0;
		}

		ofp << endl;
	}
	ofp << "---------------------+-----------------------------------------------------------" << endl;

	//print function 
	char s = 'a'; 
	ofp << "F("; 
	for(int i=0;i<N;i++){
	 	s = 'A'+ i;	
		if(i!=N-1) ofp << s << ",";
		else ofp << s << ")= ";
	}

	//function simplify
	//Q initialize
	vector<vector<int> > q;
	q.resize(prime.size());
	for(int a=0;a<prime.size();a++) q.at(a).resize(Minterm.size());
	Q = q;
	for(int i=0;i<prime.size();i++){
		for(int j=0;j<Minterm.size();j++) Q.at(i).at(j)=0;
	}

	//chart information to 2-D vector Q
	for(int i=0;i<prime.size();i++){
		prime_term(prime.at(i));
		for(int j=0;j<B.size();j++){
			for(int a=0;a<Minterm.size();a++){
				if(B.at(j) == Minterm.at(a)){
					Q[i][a]= 1;
				}
			}
		}
	}

	EPI(); //store essential prime implicant in output

	vertical_line(); 

	PI();

	for(int i=0;i<output.size();i++){
		if(i!=output.size()-1) ofp << output.at(i) << " + ";
		else ofp << output.at(i) << endl;
	}
	ofp << "=================================================================================";

}


int bit_num(int num){
	int count =0;
	if(num ==0) return 1;
	while(1){
		num /= 10;
		count ++;
		if(num==0) break;
	}
	return count;
}

void print_round(){
		ofp << "==============ROUND " << round <<  "===============\n";
		for(int i=0;i<N+1;i++){
			for(int j=0;j<group.at(i).size();j++){
				print_min(group.at(i).at(j),i,j);
			}
			if(group.at(i).size()!=0) ofp << " -------------------------------" << endl;
		}
		round++;
}


int QM(){
	tmpgroup.clear();
	tmpgroup.resize(N+1);
	tmpgroup = group;
	nextgroup.clear();
	nextgroup.resize(N+1);
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
					if(tmpgroup.at(a).at(b).front()!='v') tmpgroup.at(a).at(b) = "v " + tmpgroup.at(a).at(b);
					if(tmpgroup.at(a+1).at(c).front()!='v') tmpgroup.at(a+1).at(c) = "v " + tmpgroup.at(a+1).at(c);
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
	group.resize(N+1);
	group = nextgroup;
	if(count==0){		
		return 0; 
	}
	else return 1;
} //QM function end

void delete_duplicate_term(){
	vector<string> tmp;
	int flag=1;
	for(int i=0;i<prime.size();i++){
		if(tmp.size()==0) tmp.push_back(prime.at(i));
		else{
			flag=1;
			for(int a=0;a<tmp.size();a++){
				if(prime.at(i)==tmp.at(a)) flag=0;
			}
			if(flag) tmp.push_back(prime.at(i)); 			
		}
	}
	prime = tmp;
}

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
		for(int j=0;j<N;j++){
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
	for(int i=0;i<N;i++){
		if(str1.at(i)!=str2.at(i)) count++;
	}
	if(count>1) return 0;
	return 1;
}

string combine(string str1,string str2){ //combine two str and store into nextgruop
	int df;
	for(int i=0;i<N;i++){
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
	sort(B);
}

void sort(vector<int>& vec){
	int now;
	int tmp;
	for(int i=1;i<vec.size();i++){
		now = i;
		tmp = vec.at(i);
	while(now-1 >=0 && tmp<vec.at(now-1)){
		vec.at(now) = vec.at(now-1);
		now--;
	}
	vec.at(now) = tmp;
	}
}

int isX(){
		int count =0;
	for(int a=0;a<B.size();a++){
		count =0;
		for(int b=0;b<dontcare.size();b++){
			if(B.at(a) == dontcare.at(b)) count++;
		}
	}
	if(count == B.size()) return 1;
	else return 0;
}

void print_min(string str1,int a,int b){ //print [str] : minterm  e.g. 0000000000- : 0,1
	X_pos(str1);
	B.clear();
	if(n!=0) forloop(n,str1);
	prime_term(str1);
	for(int j=0;j<group.at(a).size();j++){
			if(j!=b && str1==group.at(a).at(j))
			tmpgroup.at(a).at(j) = "x "+ tmpgroup.at(a).at(j);				
	}

	if(tmpgroup.at(a).at(b).at(0)=='v') ofp << "v "<< str1 << " : "; //combined string
	else if(tmpgroup.at(a).at(b).at(0)=='x') ofp << "x " << str1 <<" : "; //duplicate string
	else if(isX()) ofp << "d " << str1 << " : "; //doncare string
	else ofp << "  " << str1 << " : ";

	if(n!=0){
	for(int i=0;i<B.size();i++){
		if(i!=B.size()-1) ofp << B.at(i) << ",";
		else ofp << B.at(i) << endl;
	}
	}
	else ofp << " " << btod(str1) << endl;
}

void prime_term(string str1){
	X_pos(str1);
	B.clear();
	if(n==0) B.push_back(btod(str1));
	if(n!=0) forloop(n,str1);
}

void X_pos(string str1){
	n=0;
	for(int i=0;i<N;i++) A[i]=0;
	for(int i=0;i<N;i++){
		if(str1.at(i)=='-')	n++; //compute '-'
	}	
	for(int i=0,j=0;i<N;i++){
		if(str1.at(i)=='-'){
			A[j] = i;
		 	j++;	//assign postion to A
		}
	}
}

//binary to decimal
int btod(string str1){
	int d=0;
	for(int i=0;i<N;i++){
		if(str1.at(i)=='1'){
			d += 1<<(N-1-i);
		}
	}
	return d;
}

string convert_bit(string str1){
	string bit;
	for(int i=0;i<str1.size();i++){
		if(str1.at(i)=='0'){
			bit.push_back('a'+i);
			bit.push_back('\'');
		}
		if(str1.at(i)=='1'){
			bit.push_back('a'+i);
		}
	}
	return bit;
}

//1's max
int max_term(){
	int count =0;
	int max=0;
	int max_index = 0;
	for(int i=0;i<Q.size();i++){
		count = 0;
		for(int j=0;j<Q.at(i).size();j++){
			if(Q[i][j]==1) count++;
		}
		if(max < count && !in(i)){
			max = count;
			max_index = i;
		}
	}
	return max_index;
	
}


void EPI(){
	int count=0;
	for(int i=0;i<Q.size();i++){
		for(int j=0;j<Q.at(i).size();j++){
			count = 0;
			if(Q.at(i).at(j)==1){
				for(int a=0;a<Q.size();a++){
					if(a!=i && Q.at(a).at(j)==1) count++;
				}
			}
			else {count = 1;continue;}
			if(Q.at(i).at(j)==1 && count==0) break;
		}
		if(count==0) output.push_back( convert_bit(prime.at(i)));
	}

}

void vertical_line(){
	int count=0;
	for(int i=0;i<Q.size();i++){
		for(int j=0;j<Q.at(i).size();j++){
			count = 0;
			if(Q.at(i).at(j)==1){
				for(int a=0;a<Q.size();a++){
					if(a!=i && Q.at(a).at(j)==1) count++;
				}
			}
			else {count = 1;continue;}
			if(Q.at(i).at(j)==1 && count==0) break;
		}
		if(count==0){
			index.push_back(i);
			for(int j=0;j<Q.at(i).size();j++){
				if(Q.at(i).at(j)==1){
					vline++;
					for(int a=0;a<Q.size();a++){	
						if(a!=i) Q.at(a).at(j)=0;
					}
				}
			}	
		}
	}
}

int in(int u){
	for(int i=0;i<index.size();i++){
		if(u==index.at(i)) return 1;
	}
	return 0;
}


void PI(){
	while(vline!=Minterm.size()){
		int i = max_term();
		output.push_back( convert_bit(prime.at(i)));		
		index.push_back(i);
		for(int j=0;j<Q.at(i).size();j++){
			if(Q.at(i).at(j)==1){
				vline++;
				for(int a=0;a<Q.size();a++){	
					if(a!=i) Q.at(a).at(j)=0;
				}
			}
		}	
	}
}
