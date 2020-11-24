#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
   short code; // 1byte 0 ~ 255 그리고 256[eof] 중 하나를 저장 
   int freq; // 빈도수를 저장 
   struct Node * left; //left child 를 가르킴 
   struct Node * right; //right child를 가르킴

   int internal_index; // internal_node_array 에서의 index // -1 이면 leaf node  
};

struct Node * internal_node_array[256]; //internal node의 address값을 저장// internal node가 pop될때 주소값 참고용으로 사용 
int internal_array_index = 0; //internal_node_array current index
struct Node min_Heap[258]; //단어의 빈도수에 따라 min_heap을 구성할떄 사용되는 배열 //0 not use 1 ~ 256 -> code 257 : eof
int c_HeapSize = 0; //MinHeap의 사이즈 
struct Node * root; //Huffan tree's root

char huffmanCode[50] = { 0, };
void writeFinish(FILE* fout);
void writeBit(struct Node* head, char co, int h, short data, FILE * fout);
int bitShiftCnt = 0;//비트 시프트 횟수
int bitdata = 0;//비트를 저장할 변수


void writeData(struct Node * root, FILE* input, FILE* output) //output 파일에 bit 단위로 데이터를 write하는 함수. 
{
   unsigned char read_buffer; //input 파일로부터 1byte씩 데이터를 가져옴. 
   char write_buffer; //output 파일에 비트를 작성할 때 사용되는 buffer 
   int i; //for문 사용할때 사용 
   unsigned int remainder = 0, c_Bits = 0; //remainder --> 남아있는 비트를 저장, c_Bits --> 현재 비트수를 저장 
   struct Node * temp; // 허프만 트리를 순회할때 사용되는 변수 

   while (fread(&read_buffer, 1, 1, input)) { //1byte씩 input file (~~.cpd) 에서 가져옴 

      remainder = (remainder << 8) + read_buffer; // remainder에 미리 8bit를 확보하고 거기에 읽어온 byte 저장 
      c_Bits += 8; // 한번에 1byte 가져옴으로 +8 
      temp = root; //허프만 트리 root부터 탐색 시작 

      for (i = c_Bits - 1; i >= 0; --i) { //1byte를 1bit씩 검사 i = 7 -> MSB 검사 ~~ i = 0 -> LSB검사 

         if (((remainder >> i) & 1) == 0) // 현재 검사한 비트가 1 일경우 left 
            temp = temp->left;
         else // 현재 검사한 비트가 0 일경우 right 
            temp = temp->right;

         if (temp->left == NULL) {   //해당 노드가 leaf 노드임 --> left ,right 중 하나가 NULL이면 나머지 하나도 NULL  
            if (temp->code == 257) //eof 를 만나면 종료 
               return;
            write_buffer = temp->code;   //output에 쓰기위해 code 저장 
            fwrite(&write_buffer, 1, 1, output); //output file --> ~.dcpd에 write 
            c_Bits = i; // write 한 만큼 현재 bit수를 감소 
            temp = root; //다시 허프만 트리 루트부터 탐색 
         }
      }
   }
}
void printfHeap(struct Node * node) // heap에 데이터들이 제대로 삽입됫는지 확인할때 사용된 함수 
{
   if (node != NULL) {
      printf("node freq : %d // ", node->freq); //node의 freq 출력 
      if (node->left != NULL)
         printf("left freq : %d // ", node->left->freq); //node의 leftfreq 출력
      if (node->right != NULL)
         printf("right freq : %d // ", node->right->freq); //node의 right freq 출력
      printf("\n");
      printfHeap(node->left); // left로 탐색 
      printfHeap(node->right); // right로 탐색 
   }
}

void insert_minHeap(struct Node node) //허프만 트리에 해당 Node값을 넣음 
{
   //printf("code : %d insert_minheap -> freq : %d\n",node.code,node.freq);
   int temp = ++c_HeapSize; // 계속 부모로 올라갈떄 사용되는 변수 
   int parent = temp / 2;  // parent = child / 2 
   while (temp > 1) { //삽입된 노드의 위치를 결정
                  //printf("node.freq : %d parent.freq : %d\n",node.freq,min_Heap[parent].freq); 
      if (min_Heap[parent].freq > node.freq) { //true 이면 parent 와 child 자리를 바꿔줌
         min_Heap[temp] = min_Heap[parent]; // parent에 있던 노드를 child(temp)로 가져옴 
         temp = parent; //temp, parent 높이 1 증가. 
         parent = parent / 2;
      }
      else
         break;
   }
   //printf("%d position : %d\n",node.freq,temp);
   min_Heap[temp] = node; // 최종적으로 결정된 자리에 node 삽입. 
                     //printf("insert_minHeap : temp -> %d in_index : %d\n",temp,min_Heap[temp].internal_index);
}

void  min_Heap_restruct() //MinHeap restructre 
{
   int temp = 1;  //맨 위부터 탐색 
   int compare_target; // 자식들중 우선순위가 더큰 자식을 저장 
   int left = 2 * temp, right = 2 * temp + 1; //자식의 index 저장 
   struct Node n_temp = min_Heap[c_HeapSize--]; //비교대상 : 힙에 맨 마지막에 있는 Node 
                                     //printf("n_tmp freq : %d\n",n_temp.freq);
   while (temp < c_HeapSize) { //탐색 할 수 없을 떄(leaf)까지 아래로 내려감. 
                        //printf("left freq : %d\n",min_Heap[left].freq);
                        //printf("right freq : %d\n",min_Heap[right].freq);
      if (min_Heap[left].freq <= min_Heap[right].freq) //자식 중 비교 대상 정함. 
         compare_target = left;
      else
         compare_target = right;

      //printf("compare target : min_heap freq : %d\n",min_Heap[compare_target].freq);
      if (n_temp.freq > min_Heap[compare_target].freq && compare_target <= c_HeapSize) { //child가 parent보다 freq가 더 작으면서, child가 heapsize 안에 있을때 
         min_Heap[temp] = min_Heap[compare_target]; //parent와 child를 바꿈 
                                          //printf("temp : min_heap freq : %d\n",min_Heap[temp].freq);
         temp = compare_target; // parent, child update 
         left = 2 * temp, right = 2 * temp + 1;
      }
      else
         break;
   }
   min_Heap[temp] = n_temp; //최종적으로 결정된 자리에 node 저장 --> heap_restruct success 
                      //   printf("current [1] freq : %d\n",min_Heap[1].freq); 
}

struct Node * extractMin() // minheap에서 1번쨰 원소의 노드의 정보를 가진 노드를 동적할당 하여 주소를 리턴 
{
   struct Node * result; //최종적으로 반환될  Node 주소 
   if (min_Heap[1].internal_index != -1) { //빠져 나오려는 노드가 internal 노드일때 
      result = internal_node_array[min_Heap[1].internal_index];
      //min이 internal node일경우 에는 이미 전에 동적 할당을 했음으로 internal_array에서 주소를 가져오서 return 
   }
   else { // extract leaf Node 
      result = (struct Node *) malloc(sizeof(struct Node)); //leaf_node일떄는 새로 동적할당 
      result->code = min_Heap[1].code;
      result->freq = min_Heap[1].freq;
      result->left = min_Heap[1].left;
      result->right = min_Heap[1].right;
      result->internal_index = -1; //leaf_node는 internal_Node가 아님으로 -1으로 설정 
   }

   //printf("code : %d // freq : %d\n",result->code,result->freq);
   //printf("extractMin : freq -> %d\n",min_Heap[1].freq);
   min_Heap_restruct(); // extract후 heap 재정렬
                   //printf("extractMin : heapsize -> %d\n",c_HeapSize);
                   //printf("internal_index : %d \n",min_Heap[1].internal_index);
                   //   if(result == NULL)
                   //      printf("result == Null\n");
   return result;
}

int main(int argc, char **argv)
{
   FILE * input;   //입력으로 받아드릴 file stream --> *.cpd
   FILE * output; //결과로 내보낼 file stream 
   FILE * original_input;  //원본 파일 file stream
   unsigned char buff[1];               // 버퍼
   char * fileName = (char *)malloc(sizeof(char) * 10); // 확장자를 제외한 파일이름을 저장함 ex> 1.html -> 1
   char * original_fileName = (char *)malloc(sizeof(char) * 10); //original_fileName을 저장할 문자열 동적할당 ex> 1.html
   char * output_fileName = (char *)malloc(sizeof(char) * 10); //output_fileName을 저장할 문자열 동적할당 ex> 1.dcpd 

   unsigned char temp; // --> file로부터 1byte씩 받아올떄 사용되는 buffer 
   int freqs[256] = { 0, }; //code : 0~255에 해당하는 값에 대한 빈도를 저장 
   int x;
   struct Node n_temp; //Node를 임의로 저장하는 변수 
   struct Node * internal_temp; //Node * 값을 임의로 저장하는 변수 

   strncpy(fileName, argv[1], 1); // 확장자를 제외한 파일이름을 저장함 ex> 1.html -> 1
   fileName[1] = '\0';

   switch (fileName[0]) { //original_input file naming
   case '1':
      original_fileName = strcat(fileName, ".xml");
      break;
   case '2':
      original_fileName = strcat(fileName, ".html");
      break;
   case '3':
      original_fileName = strcat(fileName, ".fna");
      break;
   case '4':
      original_fileName = strcat(fileName, ".wav");
      break;
   case '5':
      original_fileName = strcat(fileName, ".bmp");
      break;
   }
   original_input = fopen(original_fileName, "rb"); //original_file을 open ex> 1.html open --> 허프만 트리를 구성하기위해  

   fileName[1] = '\0';
   output_fileName = strcat(fileName, ".cpd"); //~~.dcpd 

                                     //   printf("%s\n",original_fileName);
                                     //   printf("%s\n",output_fileName);

   output = fopen(output_fileName, "wb"); // *.dcpd에 데이터를 쓰기위해 fopen 
   while (fread(&temp, 1, 1, original_input)) { // 더이상 받아올게 없을 때 
      ++freqs[temp]; //해당 code의 빈도 1 증가 
   }

   //leaf node insert 
   for (x = 0; x < 256; ++x) {
      if (freqs[x] > 0) { //등장했던 문자 --> leaf node들 넣음. 
                     //insert_huffan_tree(x,freqs[x]); //huffman tree에 추가
                     //         printf("%d : %d\n",x,freqs[x]); // ok 문자 빈도 check good 
         n_temp.code = x; //n_temp(Node값을 임의로 저장하는 변수)에 값들을 저장하여 insert_MinHeap에 넘김 
         n_temp.freq = freqs[x];
         n_temp.left = NULL;
         n_temp.right = NULL;
         n_temp.internal_index = -1;
         //printf("%d // insert Leaf Node : code : %d freq : %d internal_index : %d\n",x,n_temp.code,n_temp.freq,n_temp.internal_index);
         insert_minHeap(n_temp); //해당 노드를 heap에 insert 
      }
   }
   //eof도 노드에 저장하여 insert_minHeap에 넘김 
   n_temp.code = 257;//257이였음
   n_temp.freq = 1;
   n_temp.left = NULL;
   n_temp.right = NULL;
   n_temp.internal_index = -1;
   insert_minHeap(n_temp); //eof도 minheap에 추가. 
                     //huffman_tree start
   while (c_HeapSize > 1) { //더이상 extract할게 없을떄 까지 
      internal_temp = (struct Node *) malloc(sizeof(struct Node)); //internalNode를 저장하기위해 동적할당  
      internal_temp->internal_index = internal_array_index; //나중에 heap에서 꺼내쓸때 해당 internal의 주소를 사용하기위해 배열에 따로저장 
      internal_node_array[internal_array_index++] = internal_temp; //나중에 internal node가 extract 될떄 주소를 사용하기위해 저장       
      internal_temp->left = extractMin(); // 가장 낮은 freq 추출
                                 //printf("left node -> freq : %d\n",internal_temp->left->freq); 
      internal_temp->right = extractMin(); // 그 다음으로 낮은 freq 추출
                                  //printf("right node -> freq : %d\n",internal_temp->right->freq);
      internal_temp->freq = internal_temp->left->freq + internal_temp->right->freq; // 제일 낮은 두개의 freq값 더함 
      internal_temp->code = -1; //internal_Node일떄는 code = -1 
                          //printf("heap_size : %d new node -> code : %d freq : %d\n",c_HeapSize,internal_temp->code,internal_temp->freq);

      insert_minHeap(*internal_temp); // internal node를 허프만 트리 insert
      //printf("\n");
   }
   root = extractMin(); //맨마지막 internalNode를 root로 지정.      
   input = fopen(argv[1], "rb"); //.cpd파일 open 
    
   rewind(input);
   while (fread(buff, sizeof(char), 1, input))
   {
      writeBit(root, '0', 0, buff[0], output);
   }
   writeBit(root, '0', 0, 257, output);////여기서 마지막가리키는거 수정
   writeFinish(output);
}

void writeBit(struct Node* head, char co, int h, short data, FILE * fout)
{
   int i = 0;
   if (h - 1 >= 0)
   {
      huffmanCode[h] = co;
   }
   if (head->left == NULL)//데이터 검사
   {
      if (head->code == data)
      {
         for (i = 1; i < h + 1; i++)
         {
            if (huffmanCode[i] == '0')
            {
               bitdata = (bitdata << 1);//0이면 비트를 쉬프트만 시켜도 0이 생성이됨
               bitShiftCnt++;//비트카운트증가

            }
            else if (huffmanCode[i] == '1')
            {
               bitdata = (bitdata << 1) + 1;//1이면 비트에 1값을 넣음
               bitShiftCnt++;//비트카운트 증가
            }
            while (bitShiftCnt > 7)
            {
               char temp[1] = { 0 };
               bitShiftCnt = 0;//다시 원래대로 초기화
               temp[0] = bitdata >> bitShiftCnt;
               fwrite(temp, sizeof(char), 1, fout);
               bitdata = bitdata >> 8;
            }
         }
      }
   }
   else
   {
      writeBit(head->left, '0', h + 1, data, fout);//왼쪽으로 갈때 비트값 0으로해줌
      writeBit(head->right, '1', h + 1, data, fout);//오른쪽으로 갈때 비트값 1로해줌
   }
}

void writeFinish(FILE* fout)
{
   if (bitShiftCnt > 0)//아직 비트가 남아있을경우
   {
      char temp[1] = { 0 };               // fwirte를 위해 temp를 배열로 선언  
      temp[0] = bitdata;
      temp[0] = temp[0] << (8 - bitShiftCnt);   // 남은 부분을 왼쪽으로 옮겨준다. 

      fwrite(temp, sizeof(char), 1, fout);   // 파일에 기록
   }
}
