// 在一個圓內，找出"最多不相交弦數"。

#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Pair{
public:
	int another;
	bool reverse;

	Pair(){ another = -1; reverse = false; }
};

int maxNIL(int start, int end);
void chords(int start, int end);

Pair *input;
int **mps;
vector<int> v;

int main()
{
	int total;
	int tmp1, tmp2;
	int i, j;
	vector<Pair> circle;

	scanf("%d", &total);
	input = new Pair[total];
	mps = new int*[total];
	for(i = 0; i < total; i++){
		mps[i] = new int[total];
	}
	for(i = 0; i < total; i++){
		for(j = 0; j < total; j++){			
			if(i == j)
				mps[i][j] = 0;
			else
				mps[i][j] = -1;
		}			
	}

	for(i = 0; i < total/2; i++){
		if(scanf("%d %d", &tmp1, &tmp2) == EOF)
			break;
		input[tmp1].another = tmp2;
		input[tmp1].reverse = false;
		input[tmp2].another = tmp1;
		input[tmp2].reverse = true;
	}

	cout<<maxNIL(0, total-1)<<endl;
	chords(0, total-1);
	sort(v.begin(), v.end());
	for(i = 0; i < v.size(); i++)
		cout<<v[i]<<" "<<input[v[i]].another<<endl;

	delete []input;
	for(i = 0; i < total; i++){
		delete []mps[i];
	}
	delete []mps;

	return 0;
}

int maxNIL(int start, int end)
{
	int tail = start+1;
	int front, back;

	if(end <= start)
		return 0;
	while(tail <= end){
		if(input[tail].another >= start && input[tail].another <= tail){
			if(input[tail].another-1 < 0)
				front = 0;
			else if(mps[start][input[tail].another-1] != -1)
				front = mps[start][input[tail].another-1];
			else
				front = maxNIL(start, input[tail].another-1);
			
			if(mps[input[tail].another+1][tail-1] != -1)
				back = mps[input[tail].another+1][tail-1];
			else
				back = maxNIL(input[tail].another+1, tail-1);
			if(mps[start][tail-1] > (front + back + 1))
				mps[start][tail] = mps[start][tail-1];
			else
				mps[start][tail] = front + back + 1;
		}
		else{
			mps[start][tail] = mps[start][tail-1];
		}
		//printf("mps[%d][%d] = %d\n", start, tail, mps[start][tail]);
		tail++;
	}

	return mps[start][end];

}

void chords(int start, int end)
{
	int i;
	int offset = end;

	if(end <= start)
		return ;
	if(start < 0 || end < 0)
		return ;
	if(mps[start][end] <= 0)
		return ;
	if(mps[start][end] == mps[start][start])
		return ;

	for(i = end-1; i >= start; i--){
		if(mps[start][i] == mps[start][offset])
			offset = i;
		else
			break;
	}
	if(input[offset].reverse)
		v.push_back(input[offset].another);
	else
		v.push_back(offset);
	chords(input[offset].another+1, offset-1);
	chords(start, input[offset].another-1);
}
