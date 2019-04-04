#define DISK_PAGE_SIZE 3
#define MEM_FRAME_SIZE 3
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <limits.h>

using namespace std;

class Page{
public:
	vector<int> arr;
	int validEntries;
	//initializes an empty page with invalid entries i.e. -1
	Page(){
		arr.resize(DISK_PAGE_SIZE, -1);
		validEntries = 0;
	}

	void writePage();
	void fillPage(vector<int> &v);
};

void Page :: writePage(){

	for(int i = 0; i < DISK_PAGE_SIZE; i++){
		if(this->arr[i] == -1)
			break;
		cout << this->arr[i] << " ";
	}
	cout << endl;
}

//fills page with contents from vector v
void Page :: fillPage(vector<int> &v){
	
	if(v.size() < DISK_PAGE_SIZE){
		cout << "vector should contain atleast as many entries as a page" << endl;
		exit(1);
	}
	int i = 0;
	this->validEntries = 0;
	for(i; i < v.size() && i < DISK_PAGE_SIZE; i++){
		if(v[i] == -1){
			this->validEntries = i;
		}
		this->arr[i] = v[i];
	}
}

class Frame{
public:
	vector<int> arr;
	int validEntries;
	//initializes an empty page with invalid entries i.e. -1
	Frame()
	{
		arr.resize(MEM_FRAME_SIZE, -1);
		/*for(int i  = 0; i < MEM_FRAME_SIZE; i++)
			arr[i] = -1;*/
		validEntries = 0;
	}
	
	//fill Frame with given vector
	void fillFrame(vector<int> &v);
	void printFrame();
};

//fills frame with data from vector v
void Frame :: fillFrame(vector<int> &v){
	int i = 0;
	bool flag = false;
	while(i < v.size() && i < MEM_FRAME_SIZE){
		this->arr[i] = v[i];
		if(v[i] == -1){
			flag = true;
			this->validEntries = i;
			i++;
			break;
		}
		i++;
	}
	while(i < MEM_FRAME_SIZE){
		this->arr[i] = -1;
		i++;
	}
	if(!flag)
		this->validEntries = MEM_FRAME_SIZE;
}

//Prints all valid entries of a frame
void Frame :: printFrame()
{
	for(int i = 0; i < this->validEntries; i++)
	{
		cout << this->arr[i] << endl;
	}
}

class DiskFile{

public:
	vector<Page> data;
	int totalPages;
	int size; //in terms of number of entries
	
	DiskFile(){
		totalPages = 0;
		size = 0;
	}
	
	DiskFile(int s){
		size = 0;
		totalPages = s;
		data.resize(totalPages);
	}


	void readDiskFile();
	void writeDiskFile();
	void DiskFileCopy( DiskFile &f, int startPage, int endPage);
};

//creates DiskFile by reading input file through cin
void DiskFile :: readDiskFile(){
	
	int i = 0;
	this->size  = 0;
	this->totalPages = 0; 
	Page t;
	vector<int> temp(DISK_PAGE_SIZE,-1);
	bool flag = true;
	int x;
	while(cin >> x){
		flag = false;
		temp[i++] = x;
		this->size++;
		
		if(i == DISK_PAGE_SIZE){
			t.fillPage(temp);
			this->data.push_back(t);
			flag = true;
			this->totalPages++;
			i = 0;
		}
	}

	if(!flag){
		
		while(i != DISK_PAGE_SIZE){
			temp[i++] = -1;	
		}
		t.fillPage(temp);
		this->data.push_back(t);
		this->totalPages++;
	}
}

//writes disk file to console
void DiskFile :: writeDiskFile(){
	cout << "Contents of Disk File: " <<endl;
	for(int  i = 0; i < totalPages; i++){
		cout << "Page: " << i << ": ";
		this->data[i].writePage();
	}
}

//copies contents of disk file f into destination disk file from startPage to endPage
void DiskFile :: DiskFileCopy(DiskFile &f, int startPage, int endPage){
	int j = 0; //start of source file
	int i = startPage; //start of destination file

	while(i <= endPage && j < f.totalPages && i < this->totalPages){
		this->data[i].validEntries = f.data[j].validEntries;
		for(int k = 0; k < DISK_PAGE_SIZE; k++){
			this->data[i].arr[k] = f.data[j].arr[k];
		}
		i++;
		j++;
	}
}

class MainMemory{

public:
	vector<Frame> data;
	int totalFrames;
	vector<bool> valid;

	MainMemory(int s){
		totalFrames = s;
		data.resize(s);
		valid.resize(s);
		for(int i = 0; i < s; i++)
			valid[i] = false;
		cout << "Mem created" << endl;
	}

	int loadPage(DiskFile &f, int n);
	int getEmptyFrame();
	int getValidEntries(int f);
	int getVal(int f, int i);
	void setVal(int f, int i, int val);
	void writeFrame(DiskFile &inputFile, int f, int p);
	void freeFrame(int f);
};

//loads nth page of disk file f to an empty frame if available
int MainMemory :: loadPage(DiskFile &f, int n){

	if(n >= f.totalPages)
		return -1; // invalid page

	for(int i = 0; i < this->totalFrames; i++){
		if(!this->valid[i]){
			this->data[i].fillFrame(f.data[n].arr);
			this->valid[i] = true;
			return i;
		}
	}
	return -1; //no empty frame is available
}

//returns an empty frame if available
int MainMemory :: getEmptyFrame(){

	vector<int> v(MEM_FRAME_SIZE, -1);

	for(int i = 0; i < this->totalFrames; i++){
		if(!this->valid[i]){
			this->data[i].fillFrame(v);
			this->valid[i] = true;
			return i;
		}
	}
	return -1; //no empty frame is available

}

//returns number of valid entries in frame f
int MainMemory :: getValidEntries(int f){
	if(f >= this->totalFrames || !this->valid[f])
		return -1;
	return this->data[f].validEntries;
}


// returns value stored at location i in frame f
int MainMemory :: getVal(int f, int i){
	if(!this->valid[f] || i >= MEM_FRAME_SIZE)
		return -1;
	return this->data[f].arr[i];
}

//assigns value val to ith location of frame f
void MainMemory :: setVal(int f, int i, int val){
	if(!this->valid[f] || i >= MEM_FRAME_SIZE){
		cout << "accessing invalid address" << endl;
		exit(1);
	}

	this->data[f].arr[i] = val;
	if(this->data[f].validEntries == i)
		this->data[f].validEntries = i+1;

}

//write Frame f to file at page p
void MainMemory :: writeFrame(DiskFile &inputFile, int f, int p){
	inputFile.data[p].validEntries = getValidEntries(f);

	for(int i = 0; i <	MEM_FRAME_SIZE; i++){
		inputFile.data[p].arr[i] = this->data[f].arr[i];
	}	
}

//clears frame f
void MainMemory :: freeFrame(int f){
	if(f < totalFrames)
		this->valid[f] = false;
}	

class ExtMergeSort{
public:
	int runSize; // size of run in terms of number of pages
	int totalPass; // number of passes performed
	int totalRuns;
	int B;

	ExtMergeSort(int b1){
		B=b1;
		runSize = 0;
		totalPass = 0;
		totalRuns = -1;
	}

	void firstPass(DiskFile &inputFile, MainMemory &memory);
	void sortFrame(MainMemory &memory, int f);
	void merge(DiskFile &inputFile, MainMemory &memory, int *,int *);
	void twoWaySort(DiskFile &inputFile, MainMemory &memory);

};

//creates initial runs of 1 page size
void ExtMergeSort :: firstPass(DiskFile &inputFile, MainMemory &memory){
	
	int numberofFullRuns = inputFile.totalPages / B;
	int partialRunSize = inputFile.totalPages % B;
	vector<int> temp;
	temp.resize(B*DISK_PAGE_SIZE);
	int frame = -1;
	
	for(int i = 0; i < numberofFullRuns; i++)
	{
		temp.clear();
		for(int j = i*B; j < (i+1)*B; j++)
		{
			
			frame = memory.loadPage(inputFile, j);
			// temp.assign(memory.data[f].arr.begin(), memory.data[f].arr.begin()+memory.getValidEntries(frame));
			for(int index =0; index < memory.getValidEntries(frame); index++){
				temp.push_back(memory.data[frame].arr[index]);
			}
			// memory.writeFrame(inputFile, frame, i);
			memory.freeFrame(frame);
		}
		

		sort(temp.begin(),temp.end());

		int temp_index=0;
		for(int j = i*B; j < (i+1)*B; j++)
		{
			frame = memory.loadPage(inputFile, j);
			
			for(int index =0; index < memory.getValidEntries(frame); index++){
				memory.data[frame].arr[index] = temp[temp_index++];
			}
			memory.writeFrame(inputFile, frame, j);
			memory.freeFrame(frame);
		}
		
		
	}
	temp.clear();
	for(int j = B*numberofFullRuns ; j < B*numberofFullRuns + partialRunSize ; j++){
		frame = memory.loadPage(inputFile, j);
		// temp.assign(memory.data[f].arr.begin(), memory.data[f].arr.begin()+memory.getValidEntries(frame));
		for(int index =0; index < memory.getValidEntries(frame); index++){
			temp.push_back(memory.data[frame].arr[index]);
		}
		// memory.writeFrame(inputFile, frame, i);
		memory.freeFrame(frame);		
	}
	sort(temp.begin(),temp.end());

	int temp_index=0;
	for(int j = B*numberofFullRuns ; j < B*numberofFullRuns + partialRunSize ; j++)
	{
		frame = memory.loadPage(inputFile, j);
		
		for(int index =0; index < memory.getValidEntries(frame); index++){
			memory.data[frame].arr[index] = temp[temp_index++];
		}
		memory.writeFrame(inputFile, frame, j);
		memory.freeFrame(frame);
	}
	 


	
	// load each page to main memory frame and sort
	// for(int i = 0; i < inputFile.totalPages; i++){
	// 	frame = memory.loadPage(inputFile, i);
	// 	this->sortFrame(memory, frame);
	// 	memory.writeFrame(inputFile, frame, i);
	// 	memory.freeFrame(frame);
	// }
	
	runSize = B;
	totalPass = 1;
	totalRuns = inputFile.totalPages % B ==0 ? inputFile.totalPages/B : inputFile.totalPages/B +1 ;
	cout << "First Pass Performed" << endl;
	inputFile.writeDiskFile(); //print file to cout
}

//sorts each frame
void ExtMergeSort :: sortFrame(MainMemory &memory, int f){
	sort(memory.data[f].arr.begin(), memory.data[f].arr.begin()+memory.getValidEntries(f));
}

//Performs merging of 2 runs
void ExtMergeSort :: merge(DiskFile &inputFile, MainMemory &memory,int * run_start_index, int * run_end_index){
	int k =B-1;
	int finalRunSize = run_end_index[k-1] - run_start_index[0] + 1;
	DiskFile tempFile(finalRunSize);
	int kthRunIndex [k];
	int kthFrame[k];
	int index_in_kth_Frame[k];

	int resFrame = memory.getEmptyFrame();	//frame to store result
	int resIndex = 0;
	int number_of_Active_runs =0;

	for(int i = 0; i < k; i++)
	{
		index_in_kth_Frame[i]=0;
		if(run_start_index[i] <= run_end_index[i]){
			kthRunIndex[i]=run_start_index[i];
			kthFrame[i] = memory.loadPage(inputFile, kthRunIndex[i]);
			number_of_Active_runs++;
			if(kthFrame[i] == -1){
				cout << "Can't proceed further due to unavailability of memory or invalid Page access" << endl;
				exit(1); 				
			}
		}
		else
		{
			// cout<<i+1<<"th run invalid"<<endl;
			kthRunIndex[i]= run_end_index[i] +1;
			kthFrame[i]=-1;
		}
		
		/* code */
	}
	// cout<<"Final size "<<finalRunSize<<"Active "<<number_of_Active_runs<<endl;
	

/***************************************/
	bool flag = false;
	int currPage = 0;
	// int l = leftStart;
	// int r = mid + 1;

	// int leftFrame = memory.loadPage(inputFile, l);
	// int rightFrame = memory.loadPage(inputFile, r);
	// int resFrame = memory.getEmptyFrame();	//frame to store result
	if( resFrame == -1){
		cout << "Can't proceed further due to unavailability of memory or invalid Page access" << endl;
		exit(1); 
	}

	// int leftIndex = 0;
	// int rightIndex = 0;
	// int resIndex = 0;
/*****************************************/
	bool allEntriesAreValid =true;
	int val;
	int min_index=-1;
	int minimum_val;
	while(number_of_Active_runs >0){

		allEntriesAreValid = true;

		for(int i = 0; i < k; i++)
		{
			if (kthFrame[i]!=-1 && index_in_kth_Frame[i] >= memory.getValidEntries(kthFrame[i])){
				allEntriesAreValid=false;
			}
		}
		

		if(allEntriesAreValid){	
			minimum_val = INT_MAX;
			
			min_index=-1;
			for(int i = 0; i < k; i++)
			{
				if (kthFrame[i]!=-1 ){
					val = memory.getVal(kthFrame[i], index_in_kth_Frame[i]);
					if(val < minimum_val){
						minimum_val = val;
						min_index =i;
					}
				}
			}
			// cout<<"got min value as "<<minimum_val<<"at run "<<min_index+1<<endl;

			memory.setVal(resFrame, resIndex,minimum_val);
			flag = true;
			if(min_index == -1){
				cout<<"Wrong value of minimum index "<<endl;
				exit(1);
			}
			index_in_kth_Frame[min_index]++;

			resIndex++;
			if(resIndex == MEM_FRAME_SIZE){
				memory.writeFrame(tempFile, resFrame, currPage);
				flag = false;
				memory.freeFrame(resFrame);
				resFrame = memory.getEmptyFrame();
				currPage++; 
				resIndex = 0;
			}
		}

		for(int i = 0; i < k; i++)
		{
			if (kthFrame[i]!=-1 && index_in_kth_Frame[i] >= memory.getValidEntries(kthFrame[i])){
				memory.freeFrame(kthFrame[i]);
				kthRunIndex[i]++;
				if(kthRunIndex[i] <= run_end_index[i]){
					index_in_kth_Frame[i]=0;
					kthFrame[i] = memory.loadPage(inputFile,kthRunIndex[i]);
				}
				else{
					kthFrame[i]= -1;
					number_of_Active_runs--;
				}
			}
		}
	}
	

	if(flag)
		memory.writeFrame(tempFile, resFrame, currPage);
	memory.freeFrame(resFrame);
	for(int i = 0; i < k; i++)
	{
		if(kthFrame[i]!= -1)
			memory.freeFrame(kthFrame[i]);
		
	}
	
	// memory.freeFrame(leftFrame);
	// memory.freeFrame(rightFrame);
	inputFile.DiskFileCopy(tempFile, run_start_index[0], run_end_index[k-1]);

}

//Performs 2 way merge sort on inputFile using memory
void ExtMergeSort :: twoWaySort(DiskFile &inputFile, MainMemory &memory){
	int k = B-1;
	if(memory.totalFrames < 3)
		cout << "Error: Two way merge sort requires atleast 3 frames" << endl; 
	
	this->firstPass(inputFile, memory);

	int leftStart;
	int run_start_index [k];
	int run_end_index [k];
	for(this->runSize = B; this->runSize < inputFile.totalPages; this->runSize *= (B-1)){
		cout << "runSize: " << this->runSize << endl;
		for(leftStart = 0; leftStart < inputFile.totalPages-1; leftStart += (B-1)*this->runSize){

			for(int pointer_index = 0; pointer_index < k ; pointer_index++){
				run_start_index[pointer_index]= (leftStart + pointer_index*this->runSize);
				run_end_index[pointer_index] = min(inputFile.totalPages-1,leftStart + (pointer_index+1)*(this->runSize)-1);
			}

			int mid = leftStart + this->runSize-1;
			int rightEnd = min(leftStart + 2*this->runSize-1, inputFile.totalPages-1);
			cout << "calling merge for < ";
			for(int pointer_index = 0; pointer_index < k ; pointer_index++)
			{
				cout <<"("<<run_start_index[pointer_index]<<","<<run_end_index[pointer_index]<<") ";
			}
			
			cout << ">" << endl;
			this->merge(inputFile, memory, run_start_index,run_end_index);
		}
		totalPass++;
	}

	cout << "Total Passes required: " << totalPass << endl;
}

int main()
{
	int x;
	//reads size of main memory in terms of number of frames available
	cin >> x;

	//create main memory of x frames
	MainMemory mm(x);


	//create a file by taking input from cin
	DiskFile f;
	f.readDiskFile();
	f.writeDiskFile();

	ExtMergeSort *e = new ExtMergeSort(x);

	//call 2 way externalmerge sort
	e->twoWaySort(f,mm);

	//output file by using cout
	f.writeDiskFile(); 

}