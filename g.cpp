      #include <bits/stdc++.h>
            using namespace std;
     
            int main() {
            	ios::sync_with_stdio(0);
            	cin.tie(0);
            	string s;
            	cin>>s;
            	int e=0;
            	for(int i=1;i<s.size()-1;i++){
            		if(s[i]=='e')e++;
            	}
            	cout<<'h';
            	e*=2;
            	while(e--){
            		cout<<'e';
            	}
            	cout<<'y'<<"\n";
            	return 0;
            }