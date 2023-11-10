#include <bits/stdc++.h>
using namespace std;
int main(int argc, char *argv[]){
	int a = atoi(argv[1]), b = atoi(argv[2]);
	cout << a << ' ' << b << endl;
	minstd_rand0 generator(clock());
	for(int i = 0; i < a; i++){
		for(int j = 0; j < b; j++){
			int mod = generator()%400+500; 
			cout << generator()%mod << ' ';
		}
		cout << endl;
	}
	for(int i = 0; i < a; i++){
		for(int j = 0; j < b; j++){
			if(i == j + 1)
				cout << 0;
			else if(i > j + 1)
				cout << min(50 + (i - j - 2)*10, 200);
			else cout << min(100 + (j - i)*20, 400);
			cout << ' ';
		}
		cout << endl;
		
	}
	return 0;
}
