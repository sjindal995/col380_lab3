#include <iostream>
#include <cstdlib>

using namespace std;

void merge(int* buf, int size, int first_dec, int second_dec, int final_dec){
	int new_arr[10];
	int first_i = (first_dec*((size/2)-1));
	int inc_first = 0;
	int inc_second = 0;
	if(first_dec == 1) inc_first = -1;
	else inc_first = 1;
	if(second_dec == 1) inc_second = -1;
	else inc_second = 1;
	int second_i = size/2 + (second_dec*((size/2)-1));
	int done_first = 0;
	int done_second = 0;

	int loop_i = (final_dec*(size-1));
	int loop_inc = 0;
	cout << first_i << inc_first << second_i << inc_second << loop_i << loop_inc << endl;
	if(final_dec == 1) loop_inc = -1;
	else loop_inc = 1;
	for(; (loop_i < size && loop_i >= 0); loop_i += loop_inc){
		if(done_first < size/2 && done_second < size/2){
			if(buf[first_i] <= buf[second_i]){
				new_arr[loop_i] = buf[first_i];
				first_i += inc_first;
				done_first++;
			}
			else{
				new_arr[loop_i] = buf[second_i];
				second_i += inc_second;
				done_second++;
			}
		}
		else if(done_first == size/2){
			new_arr[loop_i] = buf[second_i];
			second_i += inc_second;
			done_second++;
		}
		else if(done_second == size/2){
			new_arr[loop_i] = buf[first_i];
			first_i += inc_first;
			done_first++;
		}
	}
	for(int i=0;i<size;i++){
		buf[i] = new_arr[i];
	}
	// free(new_arr);
}

int main(){
	int* arr = (int*)malloc(10*sizeof(int));
	for(int i = 1; i <6;i++){
		arr[i-1] = 4*i;
	}
	for(int i = 6; i<11; i++){
		arr[i-1] = 11-i;
	}
	for(int i=0;i<10;i++){
		cout << arr[i] << " ";
	}
	cout << endl;
	merge(arr, 10, (0 > 1), 1, 0);
	for(int i=0;i<10;i++){
		cout << arr[i] << " ";
	}
	cout << endl;
}