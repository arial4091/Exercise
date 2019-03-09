#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

int rows, columns;
int row_hint[31][17];
int column_hint[31][17];
int **answer, **one, **two;
int total = 0, should_total = 0;
vector< vector<int> > stack[31];
vector< vector<int> > col_pos[31];
int record[31][31][31];
struct NUM_POSS{
	int index, amount;
};

void all_overlap();
void detect_over();
bool detect_wrong();
void guess();
void overlap2();
void line_possible(int ans_number, char rc);
void local_solve();
int abs(int a){
	if(a < 0) return (a * -1);
	else return a;
}
bool compare(const NUM_POSS a, const NUM_POSS b){
	return (a.amount < b.amount);
}

int main()
{
	int i, j;
	char tmp, tmp2;
	int ad;
	int pret = 0;
	off_t oft;

	//讀進需要的資料
	scanf("%d %d", &rows, &columns);
	for(i = 1; i < rows+1; i++){
		j = 1;
		ad = 0;
		while(1){
			scanf("%d%c", &row_hint[i][j], &tmp);
			ad += row_hint[i][j];
			if(tmp != ' ') break;
			oft = ftello(stdin);
			scanf("%c", &tmp2);
			if(tmp2 == '\n') break;
			if(tmp2 != ' ') fseeko(stdin, oft, SEEK_SET);			
			++j;
		}
		row_hint[i][0] = j;
		row_hint[i][j+1] = ad;    //hint中第一格放這排有多少個數
		should_total += ad;
	}
	for(i = 1; i < columns+1; i++){
		j = 1;
		ad = 0;
		while(1){
			if(scanf("%d", &column_hint[i][j]) == EOF) break;
			ad += column_hint[i][j];
			if(scanf("%c", &tmp) == EOF) break;
			if(tmp == '\n') break;
			oft = ftello(stdin);
			if(scanf("%c", &tmp2) == EOF) break;
			if(tmp2 == '\n') break;
			if(tmp2 != ' ') fseeko(stdin, oft, SEEK_SET);
			++j;
		}
		column_hint[i][0] = j;
		column_hint[i][j+1] = ad;
	}

	//動態分配矩陣大小
	answer = new int*[rows + 2];
	for(i = 0; i < rows + 2; i++)
		answer[i] = new int[columns + 2];
	one = new int*[rows + 2];
	for(i = 0; i < rows + 2; i++)
		one[i] = new int[columns + 2];
	two = new int*[rows + 2];
	for(i = 0; i < rows + 2; i++)
		two[i] = new int[columns + 2];

	//初始化矩陣的值(=0)
	for(i = 0; i < rows+2; i++){
		for(j = 0; j < columns+2; j++){
			answer[i][j] = 0;
			one[i][j] = 0;
			two[i][j] = 0;
		}
	}

	all_overlap();
	while(pret != total){
		pret = total;	
		detect_over();
		overlap2();
	}
	
	if(total != should_total)
		guess();

	for(i = 1; i < rows+1; i++){
		for(j = 1; j < columns+1; j++){
			if(answer[i][j] == 1)
				printf("#");
			else
				printf(".");
		}
		printf("\n");
	}	

	return 0;
}

void all_overlap()
{
	int i, j, k;
	int loc;
	int count;	
	//將矩陣最右邊與最左邊的可能列出，有重複的即是答案中必有的
	for(i = 1; i < rows+1; i++){
		loc = 1;
		count = 1;
		for(j = 1; j <= row_hint[i][0]; j++){
			for(k = 0; k < row_hint[i][j]; k++){
				one[i][loc] = count;
				loc++;				
			}
			count++;
			++loc;
		}
	}
	for(i = 1; i < rows+1; i++){
		loc = columns;
		count = row_hint[i][0];
		for(j = row_hint[i][0]; j >= 1; j--){
			for(k = 0; k < row_hint[i][j]; k++){
				two[i][loc] = count;
				loc--;				
			}
			count--;
			--loc;
		}
	}
	for(i = 1; i < rows+1; i++){
		for(j = 1; j < columns+1; j++){
			if(one[i][j] != 0 && two[i][j] != 0){
				if(one[i][j] == two[i][j]){
					++total;
					answer[i][j] = 1;					
				}
			}
		}
	}
	for(i = 1; i < rows+1; i++){
		for(j = 1; j < columns+1; j++){
			one[i][j] = 0;
			two[i][j] = 0;
		}
	}
	//將矩陣最上面與最下面的可能列出，有重複的即是答案中必有的
	for(i = 1; i < columns+1; i++){
		loc = 1;
		count = 1;
		for(j = 1; j <= column_hint[i][0]; j++){
			for(k = 0; k < column_hint[i][j]; k++){
				one[loc][i] = count;
				loc++;				
			}
			count++;
			++loc;
		}
	}
	for(i = 1; i < columns+1; i++){
		loc = rows;
		count = column_hint[i][0];
		for(j = column_hint[i][0]; j >= 1; j--){
			for(k = 0; k < column_hint[i][j]; k++){
				two[loc][i] = count;
				loc--;				
			}
			count--;
			--loc;
		}
	}
	for(i = 1; i < rows+1; i++){
		for(j = 1; j < columns+1; j++){
			if(one[i][j] != 0 && two[i][j] != 0){
				if(one[i][j] == two[i][j]){
					if(answer[i][j] == 0){
						++total;
						answer[i][j] = 1;
					}
				}
			}
		}
	}
	for(i = 0; i < columns+2; i++){
		delete [] one[i];
		delete [] two[i];
	}
	delete [] one;delete [] two;
}

void detect_over()
{
	int i, j, k;
	int count;

	//檢測整個矩陣中，有哪一行或列是已經結束的
	for(i = 1; i < rows+1; i++){
		count = 0;
		for(j = 1; j < columns+1; j++){
			if(answer[i][j] == 1)
				count++;
		}
		if(count == row_hint[i][row_hint[i][0]+1]){
			for(j = 1; j < columns+1; j++){
				if(answer[i][j] == 0){
					answer[i][j] = -1;
				}
			}
		}
	}

	for(i = 1; i < columns+1; i++){
		count = 0;
		for(j = 1; j < rows+1; j++){
			if(answer[j][i] == 1)
				count++;
		}
		if(count == column_hint[i][column_hint[i][0]+1]){
			for(j = 1; j < rows+1; j++){
				if(answer[j][i] == 0)
					answer[j][i] = -1;
			}
		}
	}
}

bool detect_wrong(){
	int i, j, k;
	int one_num, count, len, hint_loc;

	for(i = 1; i < rows+1; i ++){
		one_num = 0;
		count = 0;
		for(j = 1; j < columns+1; j++){
			if(answer[i][j] == 1){				
				k = j + 1;
				while(1){
					if(answer[i][k] != 1){
						++count;
						break;
					}
					k++;
				}
				one_num += (k - j);
				j = k;
			}
		}		
		if(one_num > row_hint[i][row_hint[i][0]+1])
			return false;
		else if(one_num == row_hint[i][row_hint[i][0]+1]){
			if(count != row_hint[i][0])
				return false;
		}
	}
	for(i = 1; i < columns+1; i ++){
		one_num = 0;
		count = 0;
		for(j = 1; j < rows+1; j++){
			if(answer[j][i] == 1){				
				k = j + 1;
				while(1){
					if(answer[k][i] != 1){
						++count;
						break;
					}
					k++;
				}
				one_num += (k - j);
				j = k;
			}
		}		
		if(one_num > column_hint[i][column_hint[i][0]+1])
			return false;
		else if(one_num == column_hint[i][column_hint[i][0]+1]){
			if(count != column_hint[i][0])
				return false;
		}
	}
	return true;
}

void guess(){
	int i, j, k;
	int pre_total, one_num;
	vector<int> run(32, -1);
	vector<NUM_POSS> i_sort;
	struct NUM_POSS tmp;
	bool pop_record;
	bool repet;

	for(i = 1; i < columns+1; i++)
		line_possible(i, 'c');
	for(i = 1; i < rows+1; i++){
		line_possible(i, 'r');		
		tmp.index = i;
		tmp.amount = stack[i].size();
		i_sort.push_back(tmp);
	}
	sort(i_sort.begin(), i_sort.end(), compare);
	
	for(i = 0; i < rows; i++){
		if(stack[i_sort[i].index].size() == 0)
			continue;
		run[i_sort[i].index] = stack[i_sort[i].index].size();
		one_num = 0;
		for(j = 1; j < columns+1; j++){
			if(answer[i_sort[i].index][j] == 1)
				++one_num;
		}
		//printf("------------------%d------------------\n", i_sort[i].index);

		for(k = 1; k < rows+1; k++){
			for(j = 1; j < columns+1; j++){
				record[i_sort[i].index][k][j] = answer[k][j];		
			}
		}		
		total += (row_hint[i_sort[i].index-1][row_hint[i_sort[i].index-1][0]+1] - one_num);
		record[i_sort[i].index][0][0] = total;
		//if(one_num == row_hint[i_sort[i].index-1][row_hint[i_sort[i].index-1][0]+1]) continue;
		while(1){			
			while(1){
				pop_record = false;
				while(run[i_sort[i].index] == -1){
					i--;
					pop_record = true;
				}				
				if(pop_record){
					for(k = 1; k < rows+1; k++){
						for(j = 1; j < columns+1; j++)
							 answer[k][j] = record[i_sort[i].index][k][j];							 				
					}
					total = record[i_sort[i].index][0][0];
				}
				run[i_sort[i].index]--;
				if(run[i_sort[i].index] == -1) continue;
				repet = true;
				for(j = 1; j < columns+1; j++){				
					if(answer[i_sort[i].index][j] == 1){
						if(stack[i_sort[i].index][run[i_sort[i].index]][j] == -1){
							repet = false;
							break;
						}
					}
					else if(answer[i_sort[i].index][j] == -1){
						if(stack[i_sort[i].index][run[i_sort[i].index]][j] == 1){
							repet = false;
							break;
						}
					}
				}
				if(repet){
					for(j = 1; j < columns+1; j++){
						answer[i_sort[i].index][j] = stack[i_sort[i].index][run[i_sort[i].index]][j];		
					}
					if(detect_wrong())
						break;
					else{
						for(j = 1; j < columns+1; j++)
							answer[i_sort[i].index][j] = record[i_sort[i].index][i_sort[i].index][j];
					}
				}
			}
			//printf("2222------------------%d------------------\n", i_sort[i].index);
			
			pre_total = 0;
			while(total != pre_total){
				pre_total = total;
				local_solve();
				detect_over();				
			}
						
			if(detect_wrong()){
				break;
			}
			else{
				for(k = 1; k < rows+1; k++){
					for(j = 1; j < columns+1; j++)
						answer[k][j] = record[i_sort[i].index][k][j];							 				
				}
				total = record[i_sort[i].index][0][0];
			}
		}
		if(total == should_total)
			break;
	}
}

void overlap2()
{
	vector<int> a(16, 0); //possible
	vector<int> b(16, 0); //finish location
	vector<int> tmp(32, 0); 
	vector<int> c(32, -1);  //transform to the format of filling in stack
	int i, j, k, loc, counter, size;
	bool change, dup;

	for(k = 1; k < 31; k++){
		if(k > rows && k > columns) break;
		//printf("----------------------%d----------------------\n", k);
		if(k <= rows) {                //row part
			if((row_hint[k][0] + row_hint[k][row_hint[k][0]+1]) > columns){
				goto column_part;
			}
			size = row_hint[k][0] + 1;
			b[size-1] = columns - row_hint[k][size-1] + 1;
			for(i = size-2; i > 0; i--){
				b[i] = b[i+1] - row_hint[k][i] - 1;
			}
			a[1] = 1;
			for(i = 2; i < size; i++){
				a[i] = row_hint[k][i-1] + a[i-1] + 1;
			}
			for(i = 1; i < size; i++){
				loc = a[i];
				for(j = 0; j < row_hint[k][i]; j++){
					tmp[loc] += 1;
					++loc;
				}
			}
			counter = 1;
			while(1){
				change = false;
				for(i = 1; i < size; i++){
					if(a[i] == b[i]){
						change = true;
						a[i-1]++;
						break;
					}
				}
				if(change){
					for(j = i; j < size; j++)
						a[j] = a[j-1] + row_hint[k][j-1] + 1;			
				}
				else
					a[size-1]++;
		
				if(a[size-1] <= b[size-1]){
					for(i = 1; i < columns+1; i++)
						c[i] = -1;
					for(i = 1; i < size; i++){
						loc = a[i];
						for(j = 0; j < row_hint[k][i]; j++){
							c[loc] = 1;				
							++loc;
						}
					}
					dup = true;
					for(j = 1; j < columns+1; j++){
						if(answer[k][j] == 1){
							if(c[j] == -1){
								dup = false;
								break;
							}
						}
						else if(answer[k][j] == -1){
							if(c[j] == 1){
								dup = false;
								break;
							}
						}
					}
					if(dup){
						counter++;
						for(i = 1; i < columns+1; i++){
							if(c[i] == 1){
								tmp[i] += 1;
							}
						}
					}			
				}	

				if(a[1] == b[1]){			
					break;
				}
			}
			for(i = 0; i < columns+1; i++){
				if(tmp[i] == counter){					
					if(answer[k][i] == 0){
						answer[k][i] = 1;
						++total;
					}
				}
				else if(tmp[i] == 0)
					answer[k][i] = -1;
			}
			for(i = 1; i < columns+1; i++)
				tmp[i] = 0;
		}

		//column part
		column_part:
		if(k <= columns){
			if((column_hint[k][0] + column_hint[k][column_hint[k][0] + 1]) > rows)
				continue;
			size = column_hint[k][0] + 1;
			b[size-1] = rows - column_hint[k][size-1] + 1;
			for(i = size-2; i > 0; i--){
				b[i] = b[i+1] - column_hint[k][i] - 1;
			}
			a[1] = 1;
			for(i = 2; i < size; i++){
				a[i] = column_hint[k][i-1] + a[i-1] + 1;
			}
			for(i = 1; i < size; i++){
				loc = a[i];
				for(j = 0; j < column_hint[k][i]; j++){
					tmp[loc] += 1;
					++loc;
				}
			}
			counter = 1;
			while(1){
				change = false;
				for(i = 1; i < size; i++){
					if(a[i] == b[i]){
						change = true;
						a[i-1]++;
						break;
					}
				}
				if(change){
					for(j = i; j < size; j++)
						a[j] = a[j-1] + column_hint[k][j-1] + 1;			
				}
				else
					a[size-1]++;
		
				if(a[size-1] <= b[size-1]){
					for(i = 1; i < rows+1; i++)
						c[i] = -1;
					for(i = 1; i < size; i++){
						loc = a[i];
						for(j = 0; j < column_hint[k][i]; j++){
							c[loc] = 1;				
							++loc;
						}
					}
					dup = true;
					for(j = 1; j < rows+1; j++){
						if(answer[j][k] == 1){
							if(c[j] == -1){
								dup = false;
								break;
							}
						}
						else if(answer[j][k] == -1){
							if(c[j] == 1){
								dup = false;
								break;
							}
						}
					}
					if(dup){
						counter++;
						for(i = 1; i < rows+1; i++){
							if(c[i] == 1){
								tmp[i] += 1;
							}
						}
					}			
				}	

				if(a[1] == b[1]){			
					break;
				}
			}
			for(i = 0; i < rows+1; i++){
				if(tmp[i] == counter){
					if(answer[i][k] == 0){
						answer[i][k] = 1;
						++total;
					}
				}
				else if(tmp[i] == 0)
					answer[i][k] = -1;
			}
			for(i = 1; i < columns+1; i++)
				tmp[i] = 0;
		}
	}
}

void line_possible(int ans_number, char rc)
{	
	vector<int> a(16, 0); //possible
	vector<int> b(16, 0); //finish location
	vector<int> c(32, -1); //transform to the format of filling in stack
	int i, j, loc;
	bool change, dup;
	int size;

	if(rc == 'r'){
	size = row_hint[ans_number][0] + 1;
	if((row_hint[ans_number][0] + row_hint[ans_number][size]) > columns)
		return;

	b[size-1] = columns - row_hint[ans_number][size-1] + 1;
	for(i = size-2; i > 0; i--){
		b[i] = b[i+1] - row_hint[ans_number][i] - 1;
	}
	a[1] = 1;
	for(i = 2; i < size; i++){
		a[i] = row_hint[ans_number][i-1] + a[i-1] + 1;
	}
	for(i = 1; i < size; i++){
		loc = a[i];
		for(j = 0; j < row_hint[ans_number][i]; j++){
			c[loc] = 1;
			++loc;
		}
	}
	stack[ans_number].push_back(c);

	while(1){
		change = false;
		for(i = 1; i < size; i++){
			if(a[i] == b[i]){
				change = true;
				a[i-1]++;
				break;
			}
		}
		if(change){
			for(j = i; j < size; j++)
				a[j] = a[j-1] + row_hint[ans_number][j-1] + 1;			
		}
		else
			a[size-1]++;
		
		if(a[size-1] <= b[size-1]){
			for(i = 1; i < columns+1; i++)
				c[i] = -1;
			for(i = 1; i < size; i++){
				loc = a[i];
				for(j = 0; j < row_hint[ans_number][i]; j++){
					c[loc] = 1;				
					++loc;
				}
			}
			dup = true;

			for(j = 1; j < columns+1; j++){
				if(answer[ans_number][j] == 1){
					if(c[j] == -1){
						dup = false;
						break;
					}
				}
				else if(answer[ans_number][j] == -1){
					if(c[j] == 1){
						dup = false;
						break;
					}
				}
			}
			if(dup){
				stack[ans_number].push_back(c);
			}			
		}	

		if(a[1] == b[1]){			
			break;
		}
	}
	}

	if(rc == 'c'){
	size = column_hint[ans_number][0] + 1;
	if((column_hint[ans_number][0] + column_hint[ans_number][size]) > rows)
		return;

	b[size-1] = rows - column_hint[ans_number][size-1] + 1;
	for(i = size-2; i > 0; i--){
		b[i] = b[i+1] - column_hint[ans_number][i] - 1;
	}
	a[1] = 1;
	for(i = 2; i < size; i++){
		a[i] = column_hint[ans_number][i-1] + a[i-1] + 1;
	}
	for(i = 1; i < size; i++){
		loc = a[i];
		for(j = 0; j < column_hint[ans_number][i]; j++){
			c[loc] = 1;
			++loc;
		}
	}
	col_pos[ans_number].push_back(c);

	while(1){
		change = false;
		for(i = 1; i < size; i++){
			if(a[i] == b[i]){
				change = true;
				a[i-1]++;
				break;
			}
		}
		if(change){
			for(j = i; j < size; j++)
				a[j] = a[j-1] + column_hint[ans_number][j-1] + 1;			
		}
		else
			a[size-1]++;
		
		if(a[size-1] <= b[size-1]){
			for(i = 1; i < rows+1; i++)
				c[i] = -1;
			for(i = 1; i < size; i++){
				loc = a[i];
				for(j = 0; j < column_hint[ans_number][i]; j++){
					c[loc] = 1;				
					++loc;
				}
			}
			dup = true;

			for(j = 1; j < rows+1; j++){
				if(answer[j][ans_number] == 1){
					if(c[j] == -1){
						dup = false;
						break;
					}
				}
				else if(answer[j][ans_number] == -1){
					if(c[j] == 1){
						dup = false;
						break;
					}
				}
			}
			if(dup){
				col_pos[ans_number].push_back(c);
			}			
		}	

		if(a[1] == b[1]){			
			break;
		}
	}
	}
}

void local_solve()
{
	int i, j, k, counter;
	bool dup;
	vector<int> tmp(31, 0);
	//printf("local_solve\n");
	for(i = 1; i < columns+1; i++){
		counter = 0;
		for(j = 0; j < col_pos[i].size(); j++){
			dup = true;
			for(k = 1; k < rows+1; k++){				
				if(answer[k][i] == 1){
					if(col_pos[i][j][k] == -1){
						dup = false;
						break;
					}
				}
				if(answer[k][i] == -1){
					if(col_pos[i][j][k] == 1){
						dup = false;
						break;
					}
				}				
			}			
			if(dup){
				++counter;
				for(k = 1; k < rows+1; k++){
					if(col_pos[i][j][k] == 1)
						tmp[k] += 1;					
				}
			}
		}
		for(k = 1; k < rows+1; k++){
			if(tmp[k] == counter){
				if(answer[k][i] == 0){
					++total;
					answer[k][i] = 1;
				}
			}
			else if(tmp[k] == 0)
				answer[k][i] = -1;
		}

		for(k = 1; k < rows+1; k++)
			tmp[k] = 0;
	}
}