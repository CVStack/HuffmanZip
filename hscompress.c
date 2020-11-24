#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
   short code; // 1byte 0 ~ 255 �׸��� 256[eof] �� �ϳ��� ���� 
   int freq; // �󵵼��� ���� 
   struct Node * left; //left child �� ����Ŵ 
   struct Node * right; //right child�� ����Ŵ

   int internal_index; // internal_node_array ������ index // -1 �̸� leaf node  
};

struct Node * internal_node_array[256]; //internal node�� address���� ����// internal node�� pop�ɶ� �ּҰ� ��������� ��� 
int internal_array_index = 0; //internal_node_array current index
struct Node min_Heap[258]; //�ܾ��� �󵵼��� ���� min_heap�� �����ҋ� ���Ǵ� �迭 //0 not use 1 ~ 256 -> code 257 : eof
int c_HeapSize = 0; //MinHeap�� ������ 
struct Node * root; //Huffan tree's root

char huffmanCode[50] = { 0, };
void writeFinish(FILE* fout);
void writeBit(struct Node* head, char co, int h, short data, FILE * fout);
int bitShiftCnt = 0;//��Ʈ ����Ʈ Ƚ��
int bitdata = 0;//��Ʈ�� ������ ����


void writeData(struct Node * root, FILE* input, FILE* output) //output ���Ͽ� bit ������ �����͸� write�ϴ� �Լ�. 
{
   unsigned char read_buffer; //input ���Ϸκ��� 1byte�� �����͸� ������. 
   char write_buffer; //output ���Ͽ� ��Ʈ�� �ۼ��� �� ���Ǵ� buffer 
   int i; //for�� ����Ҷ� ��� 
   unsigned int remainder = 0, c_Bits = 0; //remainder --> �����ִ� ��Ʈ�� ����, c_Bits --> ���� ��Ʈ���� ���� 
   struct Node * temp; // ������ Ʈ���� ��ȸ�Ҷ� ���Ǵ� ���� 

   while (fread(&read_buffer, 1, 1, input)) { //1byte�� input file (~~.cpd) ���� ������ 

      remainder = (remainder << 8) + read_buffer; // remainder�� �̸� 8bit�� Ȯ���ϰ� �ű⿡ �о�� byte ���� 
      c_Bits += 8; // �ѹ��� 1byte ���������� +8 
      temp = root; //������ Ʈ�� root���� Ž�� ���� 

      for (i = c_Bits - 1; i >= 0; --i) { //1byte�� 1bit�� �˻� i = 7 -> MSB �˻� ~~ i = 0 -> LSB�˻� 

         if (((remainder >> i) & 1) == 0) // ���� �˻��� ��Ʈ�� 1 �ϰ�� left 
            temp = temp->left;
         else // ���� �˻��� ��Ʈ�� 0 �ϰ�� right 
            temp = temp->right;

         if (temp->left == NULL) {   //�ش� ��尡 leaf ����� --> left ,right �� �ϳ��� NULL�̸� ������ �ϳ��� NULL  
            if (temp->code == 257) //eof �� ������ ���� 
               return;
            write_buffer = temp->code;   //output�� �������� code ���� 
            fwrite(&write_buffer, 1, 1, output); //output file --> ~.dcpd�� write 
            c_Bits = i; // write �� ��ŭ ���� bit���� ���� 
            temp = root; //�ٽ� ������ Ʈ�� ��Ʈ���� Ž�� 
         }
      }
   }
}
void printfHeap(struct Node * node) // heap�� �����͵��� ����� ���Ե̴��� Ȯ���Ҷ� ���� �Լ� 
{
   if (node != NULL) {
      printf("node freq : %d // ", node->freq); //node�� freq ��� 
      if (node->left != NULL)
         printf("left freq : %d // ", node->left->freq); //node�� leftfreq ���
      if (node->right != NULL)
         printf("right freq : %d // ", node->right->freq); //node�� right freq ���
      printf("\n");
      printfHeap(node->left); // left�� Ž�� 
      printfHeap(node->right); // right�� Ž�� 
   }
}

void insert_minHeap(struct Node node) //������ Ʈ���� �ش� Node���� ���� 
{
   //printf("code : %d insert_minheap -> freq : %d\n",node.code,node.freq);
   int temp = ++c_HeapSize; // ��� �θ�� �ö󰥋� ���Ǵ� ���� 
   int parent = temp / 2;  // parent = child / 2 
   while (temp > 1) { //���Ե� ����� ��ġ�� ����
                  //printf("node.freq : %d parent.freq : %d\n",node.freq,min_Heap[parent].freq); 
      if (min_Heap[parent].freq > node.freq) { //true �̸� parent �� child �ڸ��� �ٲ���
         min_Heap[temp] = min_Heap[parent]; // parent�� �ִ� ��带 child(temp)�� ������ 
         temp = parent; //temp, parent ���� 1 ����. 
         parent = parent / 2;
      }
      else
         break;
   }
   //printf("%d position : %d\n",node.freq,temp);
   min_Heap[temp] = node; // ���������� ������ �ڸ��� node ����. 
                     //printf("insert_minHeap : temp -> %d in_index : %d\n",temp,min_Heap[temp].internal_index);
}

void  min_Heap_restruct() //MinHeap restructre 
{
   int temp = 1;  //�� ������ Ž�� 
   int compare_target; // �ڽĵ��� �켱������ ��ū �ڽ��� ���� 
   int left = 2 * temp, right = 2 * temp + 1; //�ڽ��� index ���� 
   struct Node n_temp = min_Heap[c_HeapSize--]; //�񱳴�� : ���� �� �������� �ִ� Node 
                                     //printf("n_tmp freq : %d\n",n_temp.freq);
   while (temp < c_HeapSize) { //Ž�� �� �� ���� ��(leaf)���� �Ʒ��� ������. 
                        //printf("left freq : %d\n",min_Heap[left].freq);
                        //printf("right freq : %d\n",min_Heap[right].freq);
      if (min_Heap[left].freq <= min_Heap[right].freq) //�ڽ� �� �� ��� ����. 
         compare_target = left;
      else
         compare_target = right;

      //printf("compare target : min_heap freq : %d\n",min_Heap[compare_target].freq);
      if (n_temp.freq > min_Heap[compare_target].freq && compare_target <= c_HeapSize) { //child�� parent���� freq�� �� �����鼭, child�� heapsize �ȿ� ������ 
         min_Heap[temp] = min_Heap[compare_target]; //parent�� child�� �ٲ� 
                                          //printf("temp : min_heap freq : %d\n",min_Heap[temp].freq);
         temp = compare_target; // parent, child update 
         left = 2 * temp, right = 2 * temp + 1;
      }
      else
         break;
   }
   min_Heap[temp] = n_temp; //���������� ������ �ڸ��� node ���� --> heap_restruct success 
                      //   printf("current [1] freq : %d\n",min_Heap[1].freq); 
}

struct Node * extractMin() // minheap���� 1���� ������ ����� ������ ���� ��带 �����Ҵ� �Ͽ� �ּҸ� ���� 
{
   struct Node * result; //���������� ��ȯ��  Node �ּ� 
   if (min_Heap[1].internal_index != -1) { //���� �������� ��尡 internal ����϶� 
      result = internal_node_array[min_Heap[1].internal_index];
      //min�� internal node�ϰ�� ���� �̹� ���� ���� �Ҵ��� �������� internal_array���� �ּҸ� �������� return 
   }
   else { // extract leaf Node 
      result = (struct Node *) malloc(sizeof(struct Node)); //leaf_node�ϋ��� ���� �����Ҵ� 
      result->code = min_Heap[1].code;
      result->freq = min_Heap[1].freq;
      result->left = min_Heap[1].left;
      result->right = min_Heap[1].right;
      result->internal_index = -1; //leaf_node�� internal_Node�� �ƴ����� -1���� ���� 
   }

   //printf("code : %d // freq : %d\n",result->code,result->freq);
   //printf("extractMin : freq -> %d\n",min_Heap[1].freq);
   min_Heap_restruct(); // extract�� heap ������
                   //printf("extractMin : heapsize -> %d\n",c_HeapSize);
                   //printf("internal_index : %d \n",min_Heap[1].internal_index);
                   //   if(result == NULL)
                   //      printf("result == Null\n");
   return result;
}

int main(int argc, char **argv)
{
   FILE * input;   //�Է����� �޾Ƶ帱 file stream --> *.cpd
   FILE * output; //����� ������ file stream 
   FILE * original_input;  //���� ���� file stream
   unsigned char buff[1];               // ����
   char * fileName = (char *)malloc(sizeof(char) * 10); // Ȯ���ڸ� ������ �����̸��� ������ ex> 1.html -> 1
   char * original_fileName = (char *)malloc(sizeof(char) * 10); //original_fileName�� ������ ���ڿ� �����Ҵ� ex> 1.html
   char * output_fileName = (char *)malloc(sizeof(char) * 10); //output_fileName�� ������ ���ڿ� �����Ҵ� ex> 1.dcpd 

   unsigned char temp; // --> file�κ��� 1byte�� �޾ƿË� ���Ǵ� buffer 
   int freqs[256] = { 0, }; //code : 0~255�� �ش��ϴ� ���� ���� �󵵸� ���� 
   int x;
   struct Node n_temp; //Node�� ���Ƿ� �����ϴ� ���� 
   struct Node * internal_temp; //Node * ���� ���Ƿ� �����ϴ� ���� 

   strncpy(fileName, argv[1], 1); // Ȯ���ڸ� ������ �����̸��� ������ ex> 1.html -> 1
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
   original_input = fopen(original_fileName, "rb"); //original_file�� open ex> 1.html open --> ������ Ʈ���� �����ϱ�����  

   fileName[1] = '\0';
   output_fileName = strcat(fileName, ".cpd"); //~~.dcpd 

                                     //   printf("%s\n",original_fileName);
                                     //   printf("%s\n",output_fileName);

   output = fopen(output_fileName, "wb"); // *.dcpd�� �����͸� �������� fopen 
   while (fread(&temp, 1, 1, original_input)) { // ���̻� �޾ƿð� ���� �� 
      ++freqs[temp]; //�ش� code�� �� 1 ���� 
   }

   //leaf node insert 
   for (x = 0; x < 256; ++x) {
      if (freqs[x] > 0) { //�����ߴ� ���� --> leaf node�� ����. 
                     //insert_huffan_tree(x,freqs[x]); //huffman tree�� �߰�
                     //         printf("%d : %d\n",x,freqs[x]); // ok ���� �� check good 
         n_temp.code = x; //n_temp(Node���� ���Ƿ� �����ϴ� ����)�� ������ �����Ͽ� insert_MinHeap�� �ѱ� 
         n_temp.freq = freqs[x];
         n_temp.left = NULL;
         n_temp.right = NULL;
         n_temp.internal_index = -1;
         //printf("%d // insert Leaf Node : code : %d freq : %d internal_index : %d\n",x,n_temp.code,n_temp.freq,n_temp.internal_index);
         insert_minHeap(n_temp); //�ش� ��带 heap�� insert 
      }
   }
   //eof�� ��忡 �����Ͽ� insert_minHeap�� �ѱ� 
   n_temp.code = 257;//257�̿���
   n_temp.freq = 1;
   n_temp.left = NULL;
   n_temp.right = NULL;
   n_temp.internal_index = -1;
   insert_minHeap(n_temp); //eof�� minheap�� �߰�. 
                     //huffman_tree start
   while (c_HeapSize > 1) { //���̻� extract�Ұ� ������ ���� 
      internal_temp = (struct Node *) malloc(sizeof(struct Node)); //internalNode�� �����ϱ����� �����Ҵ�  
      internal_temp->internal_index = internal_array_index; //���߿� heap���� �������� �ش� internal�� �ּҸ� ����ϱ����� �迭�� �������� 
      internal_node_array[internal_array_index++] = internal_temp; //���߿� internal node�� extract �ɋ� �ּҸ� ����ϱ����� ����       
      internal_temp->left = extractMin(); // ���� ���� freq ����
                                 //printf("left node -> freq : %d\n",internal_temp->left->freq); 
      internal_temp->right = extractMin(); // �� �������� ���� freq ����
                                  //printf("right node -> freq : %d\n",internal_temp->right->freq);
      internal_temp->freq = internal_temp->left->freq + internal_temp->right->freq; // ���� ���� �ΰ��� freq�� ���� 
      internal_temp->code = -1; //internal_Node�ϋ��� code = -1 
                          //printf("heap_size : %d new node -> code : %d freq : %d\n",c_HeapSize,internal_temp->code,internal_temp->freq);

      insert_minHeap(*internal_temp); // internal node�� ������ Ʈ�� insert
      //printf("\n");
   }
   root = extractMin(); //�Ǹ����� internalNode�� root�� ����.      
   input = fopen(argv[1], "rb"); //.cpd���� open 
    
   rewind(input);
   while (fread(buff, sizeof(char), 1, input))
   {
      writeBit(root, '0', 0, buff[0], output);
   }
   writeBit(root, '0', 0, 257, output);////���⼭ ����������Ű�°� ����
   writeFinish(output);
}

void writeBit(struct Node* head, char co, int h, short data, FILE * fout)
{
   int i = 0;
   if (h - 1 >= 0)
   {
      huffmanCode[h] = co;
   }
   if (head->left == NULL)//������ �˻�
   {
      if (head->code == data)
      {
         for (i = 1; i < h + 1; i++)
         {
            if (huffmanCode[i] == '0')
            {
               bitdata = (bitdata << 1);//0�̸� ��Ʈ�� ����Ʈ�� ���ѵ� 0�� �����̵�
               bitShiftCnt++;//��Ʈī��Ʈ����

            }
            else if (huffmanCode[i] == '1')
            {
               bitdata = (bitdata << 1) + 1;//1�̸� ��Ʈ�� 1���� ����
               bitShiftCnt++;//��Ʈī��Ʈ ����
            }
            while (bitShiftCnt > 7)
            {
               char temp[1] = { 0 };
               bitShiftCnt = 0;//�ٽ� ������� �ʱ�ȭ
               temp[0] = bitdata >> bitShiftCnt;
               fwrite(temp, sizeof(char), 1, fout);
               bitdata = bitdata >> 8;
            }
         }
      }
   }
   else
   {
      writeBit(head->left, '0', h + 1, data, fout);//�������� ���� ��Ʈ�� 0��������
      writeBit(head->right, '1', h + 1, data, fout);//���������� ���� ��Ʈ�� 1������
   }
}

void writeFinish(FILE* fout)
{
   if (bitShiftCnt > 0)//���� ��Ʈ�� �����������
   {
      char temp[1] = { 0 };               // fwirte�� ���� temp�� �迭�� ����  
      temp[0] = bitdata;
      temp[0] = temp[0] << (8 - bitShiftCnt);   // ���� �κ��� �������� �Ű��ش�. 

      fwrite(temp, sizeof(char), 1, fout);   // ���Ͽ� ���
   }
}
