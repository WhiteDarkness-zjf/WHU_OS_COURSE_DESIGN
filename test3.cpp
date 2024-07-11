#include <iostream>
#include<string>
#include <vector>
#include <Windows.h>


using namespace std;

#define BLOCK_SIZE 4096     //һ�����̿�̶���С  ��λ���ֽ�

//���̿�ṹ�嶨��
struct Block
{
	int id;         //���̿��
	int size;       //���С
	int next_id;    //���ͬһ�ļ�����һ���  -1��ʾ�˿������һ��
	int flag;       //��־λ����ʾ�Ƿ����
	int capacity;   //���� ��ʾռ�ö���
	int file_id;    //�����ļ���id��
	string data;    //�����̿��ŵ�����

	//��ʼ��
	Block() {

	}

	Block(int id) {
		this->id = id;
		this->next_id = -1;
		this->size = BLOCK_SIZE;
		this->flag = 0;
		this->capacity = 0;
		this->file_id = -1;
		this->data = "";
	}

};

//���̿ռ�ṹ��
struct Disk
{
	vector<Block>block;

	//��ʼ��
	Disk(int count) {
		for (int i = 0; i < count; i++) {
			Block b(i);
			block.push_back(b);
		}
	}
};


//�жϴ��̿ռ��Ƿ�����
bool is_disk_full(Disk disk) {
	int flag = 1;
	for (int i = 0; i < disk.block.size(); i++) {
		if (disk.block[i].flag == 0) {
			flag = 0;
			break;
		}
	}
	return flag;
}

//�ļ��ṹ��
struct File
{
	int id;               //�ļ�id  ��0��ʼ���
	string name;          //�ļ���
	int size;          //�ļ���С
	int in_which_file;    //��ʾ����Ŀ¼��   0Ϊ��Ŀ¼  ֻ�и�Ŀ¼�������Ϊ-1
	int type;             //�ļ�����  0:Ŀ¼�ļ�  1:��ͨ�ļ�   2:��ִ���ļ�
	int status;           //�ļ�״̬  0���ر�   >0����  ����ֵ��ʾ�򿪸��ļ�����Ŀ
	int read_power;       //��Ȩ��
	int write_power;      //дȨ��
	int exe_power;        //ִ��Ȩ��
	vector<int>block;     //�ļ�ռ�õĿ�����
	vector<int>file_list; //��Ŀ¼�µ������ļ�id

	//��ʼ��
	File() {

	}

	File(int id, string name, int in_which_file, int type,Disk &disk) {


		this->id = id;
		this->name = name;
		this->in_which_file = in_which_file;
		this->type = type;
		this->status = 0;
		this->size = 4;              //��ʼ��ʱ�ȷ���4���ֽ�

		if (type != 2) {
			this->read_power = 1;
			this->write_power = 1;
			this->exe_power = 0;	
		}

		else {     //��ִ���ļ�
			this->read_power = 0;
			this->write_power = 0;
			this->exe_power = 1;
			
		}
		

		//����һ�����̿�
		for (int i = 0; i < disk.block.size(); i++) {
			if (disk.block[i].flag == 0) {
				disk.block[i].flag = 1;
				disk.block[i].capacity = 4;
				disk.block[i].file_id = this->id;
				block.push_back(i);
				break;
			}
		}

	}

};

//�ļ����ƿ�ṹ�嶨��
struct FCB
{
	vector<File>all_file;
	FCB() {

	}
};



//����id�Ų����ļ���FCB�е�����λ��
int find_index(int id, FCB& fcb) {
	int index = -1;   //���ڴ洢��ǰҳ��FCB�е�������
	for (int i = 0; i < fcb.all_file.size(); i++) {
		if (fcb.all_file[i].id == id) {
			index = i;
			break;
		}
	}
	return index;
}

//�����ļ�id�����ļ���
string  id_to_name(FCB& fcb, int id) {
	for (int i = 0; i < fcb.all_file.size(); i++) {
		if (id == fcb.all_file[i].id) {
			return fcb.all_file[i].name;
		}
	}
	return "";
}

//�����ļ�����ָ��Ŀ¼�����ļ�id
int name_to_id(FCB& fcb, string name, int curr_page) {
	int index = find_index(curr_page, fcb);
	for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
		if (name == id_to_name(fcb, fcb.all_file[index].file_list[i])) {
			return fcb.all_file[index].file_list[i];
		}
	}
	return -1;
}

//�ڵ�ǰĿ¼�����Ƿ��������ļ�
bool is_same_name(int curr_page, string name, FCB& fcb) {
	int index;   //���ڴ洢��ǰҳ��FCB�е�������
	for (int i = 0; i < fcb.all_file.size(); i++) {
		if (fcb.all_file[i].id == curr_page) {
			index = i;
			break;
		}
	}

	int flag = 0;

	for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
		if (name == id_to_name(fcb,fcb.all_file[index].file_list[i])) {
			flag = 1;
			break;
		}
	}

	return flag;

}

//���ҿ��п���Ŀ
int empty_block(Disk& disk) {
	int res = 0;
	for (int i = 0; i < disk.block.size(); i++) {
		if (disk.block[i].flag == 0) {
			res++;
		}
	}
	return res;
}

//���ļ�����һ�����̿�
void allocate_block(int id, FCB& fcb, Disk& disk) {
	if (1 > empty_block(disk)) {
		cout << "���̿ռ䲻�㣬����ʧ�ܣ�����" << endl;
		return;
	}
	
	
		for (int i = 0; i < disk.block.size(); i++) {
			if (disk.block[i].flag == 0) {
				disk.block[i].flag = 1;
				disk.block[i].file_id = id;
				disk.block[i].next_id = -1;
				disk.block[i].capacity = 0;
				disk.block[i].data = "";
				fcb.all_file[find_index(id, fcb)].block.push_back(i);
				break;
			}
		}
	
}


//�����ļ�
void create_file(FCB& fcb, Disk& disk, int& curr_page,int &index) {
	if (is_disk_full(disk)) {   //���̿�������
		cout << "���̿ռ�����������ʧ�ܣ�����" << endl;
		return;
   }


	string name;
	cout << "������Ҫ�������ļ���" << endl;
	cin >> name;
	if (is_same_name(curr_page, name,fcb)) {    //�ļ�����
		cout << "�ļ����ظ�������ʧ�ܣ�����" << endl;
		return;
	}

	int temp_index = find_index(curr_page, fcb);    //��ǰĿ¼��FCB�е�����

	if (fcb.all_file[temp_index].write_power == 0) {
		cout << "��û�жԸ�Ŀ¼��дȨ�ޣ��޷������ļ�������" << endl;
		return;
	}

	if (fcb.all_file[temp_index].size % 1024==0) {
		//��Ҫ��Ŀ¼�ļ����Ӵ��̿�
		if (empty_block(disk) <= 1) {
			cout << "���̿ռ䲻�㣬����ʧ�ܣ�����" << endl;
			return;
		}
	}

	//�ڵ�ǰĿ¼�����ļ�
	cout << "������Ҫ�������ļ�����  0:Ŀ¼�ļ�  1:��ͨ�ļ�  2:��ִ���ļ�" << endl;
	int type;
	cin >> type;
	if (type != 0 && type != 1&&type!=2) {
		cout << "�������벻�Ϸ�������ʧ�ܣ�����" << endl;
		return;
	}
	File f(index, name, curr_page, type, disk);

	//�޸�Ŀ¼��Ϣ��FCB��Ϣ
	fcb.all_file.push_back(f);

	
	fcb.all_file[temp_index].size += 4;
	fcb.all_file[temp_index].file_list.push_back(index);

	if (fcb.all_file[temp_index].size % 1024 == 0) {
		//��Ҫ��Ŀ¼�ļ����Ӵ��̿�
		allocate_block(curr_page, fcb, disk);
	}	

	index++;
	cout << "�����ɹ���" << endl;

}

//���ļ�
void open_file(FCB& fcb, int& curr_page) {

	cout << "��������򿪵��ļ���" << endl;
	string name;
	cin >> name;
	//��ǰĿ¼�鿴���޸��ļ�
	if (!is_same_name(curr_page, name, fcb)) {   //û�и��ļ�
		cout << "��ǰĿ¼�����ڸ��ļ�����ʧ��" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);
	if (fcb.all_file[index].type == 0) {    //Ŀ¼�ļ�  �޷���
		cout << "���ļ���Ŀ¼�ļ�����ʧ�ܣ�����" << endl;
		return;
	}

	if (fcb.all_file[index].type == 2) {    //��ִ���ļ�  �޷���
		cout << "���ļ��ǿ�ִ���ļ�����ʧ�ܣ�����" << endl;
		return;
	}

	fcb.all_file[index].status++;

}

//�ر��ļ�
void close_file(FCB& fcb, int& curr_page) {

	cout << "��������رյ��ļ���" << endl;
	string name;
	cin >> name;

	//��ǰĿ¼�鿴���޸��ļ�
	if (!is_same_name(curr_page, name, fcb)) {   //û�и��ļ�
		cout << "��ǰĿ¼�����ڸ��ļ����ر�ʧ��" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);
	if (fcb.all_file[index].type == 0) {    //Ŀ¼�ļ�  �޷��ر�
		cout << "���ļ���Ŀ¼�ļ����ر�ʧ�ܣ�����" << endl;
		return;
	}

	if (fcb.all_file[index].status == 0) {   //�ļ������ʹ��ڹر�״̬
		cout << "�ļ����ڹر�״̬������ر�" << endl;
		return;
	}

	fcb.all_file[index].status--;

}


//�г���ǰĿ¼�µ������ļ�
void list_all_file(FCB& fcb, int& curr_page) {
	
	int index = find_index(curr_page, fcb);

	if (fcb.all_file[index].read_power == 0) {
		cout << "��û�д�Ŀ¼�Ķ�Ȩ�ޣ�����" << endl;
		return;
	}

	if (fcb.all_file[index].file_list.size() == 0) {  //Ŀ¼Ϊ��
		cout << "��ǰĿ¼Ϊ��" << endl;
		return;
	}

	cout << "��ǰĿ¼�ļ����£�" << endl;
	for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
		int temp_index = find_index(fcb.all_file[index].file_list[i],fcb);
		cout << fcb.all_file[temp_index].name << "  ";
		if (fcb.all_file[temp_index].type == 0) {
			cout << "Ŀ¼�ļ�" << endl;
		}
		else if(fcb.all_file[temp_index].type == 1){
			cout << "��ͨ�ļ�" << endl;
		}

		else {
			cout << "��ִ���ļ�" << endl;
		}
	}
}

//���ݵ�ǰĿ¼�����ļ�·��
string find_path(FCB& fcb, int curr_page) {
	vector<int>path;
	int index;
	while (curr_page != -1) {
		path.push_back(curr_page);
		index = find_index(curr_page, fcb);
		curr_page = fcb.all_file[index].in_which_file;
	}

	string res = "";
	//�������·��
	for (int i = path.size() - 1; i >= 0; i--) {
		index = find_index(path[i], fcb);
		res = res + "/" + fcb.all_file[index].name;
	}
	return res;
}


//������һ��Ŀ¼
void back_last_catalog(int& curr_page,FCB&fcb) {
	int index = find_index(curr_page, fcb);
	int last_catalog = fcb.all_file[index].in_which_file;
	if (last_catalog == -1) {   //��ʱ�Ѿ��ڸ�Ŀ¼
		cout << "��ʱ�Ѿ��ڸ�Ŀ¼������" << endl;
		return;
	}

	curr_page = last_catalog;

}

//����Ŀ¼
void enter_catalog(int& curr_page, FCB& fcb) {
	cout << "ѡ����Ҫ�����Ŀ¼" << endl;
	string catalog;
	cin >> catalog;


	if (!is_same_name(curr_page, catalog, fcb)) {
		cout << "�����Ŀ¼�����ڵ�ǰĿ¼��" << endl;
		return;
	}
	
	
	int id = name_to_id(fcb, catalog, curr_page);
	int index = find_index(id, fcb);

	if (fcb.all_file[index].type != 0) {   //����Ŀ¼�ļ�
		cout << "������ļ�����Ŀ¼�ļ�������ʧ�ܣ�����" << endl;
		return;
	}

	if (fcb.all_file[index].read_power == 0) {
		cout << "��û�д�Ŀ¼�Ķ�Ȩ�ޣ�����" << endl;
		return;
	}
	
	curr_page = fcb.all_file[index].id;

}

//���ļ�
void read_file(FCB& fcb, int curr_page,Disk&disk) {
	cout << "ѡ����Ҫ��ȡ���ļ�" << endl;
	string name;
	cin >> name;

	//��ǰĿ¼�鿴���޸��ļ�
	if (!is_same_name(curr_page, name, fcb)) {   //û�и��ļ�
		cout << "��ǰĿ¼�����ڸ��ļ�����ȡʧ��" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);
	if (fcb.all_file[index].type == 0) {    //Ŀ¼�ļ�  �޷���ȡ
		cout << "���ļ���Ŀ¼�ļ�����ȡʧ�ܣ�����" << endl;
		return;
	}
	 
	if (fcb.all_file[index].type == 2) {       //��ִ���ļ�  �޷���ȡ
		cout << "���ļ��ǿ�ִ���ļ�����ȡʧ�ܣ�����" << endl;
		return;
	}
	
	//�鿴�ļ��Ƿ��Ѿ���
	if (fcb.all_file[index].status == 0) {   //�ļ�δ��
		cout << "�ļ����ڹر�״̬����ȡʧ��" << endl;
		return;
	}

	//�鿴�Ƿ��ж�Ȩ��
	if (fcb.all_file[index].read_power == 0) {   //û�ж�Ȩ��
		cout << "��û��Ȩ�޶�ȡ���ļ�" << endl;
		return;
	}

	cout << "�ļ��������£�" << endl;
	string res = "";
	for (int i = 0; i < fcb.all_file[index].block.size(); i++) {
		res += disk.block[fcb.all_file[index].block[i]].data;
	}

	if (res == "") {
		cout << "�ļ�Ϊ��" << endl;
		return;
	}
	cout << res << endl;

}

//��һ���ļ����һ�����̿�ʣ����ֽ���
int get_remaining_byte(int id, FCB& fcb, Disk& disk) {
	int index = find_index(id, fcb);
	int remaining;
	int count = fcb.all_file[index].block.size();
	remaining = 1024 - disk.block[fcb.all_file[index].block[count - 1]].capacity;
	return remaining;
}


//д�ļ�
void write_file(FCB& fcb, int curr_page, Disk& disk) {
	cout << "ѡ����Ҫд����ļ�" << endl;
	string name;
	cin >> name;

	//��ǰĿ¼�鿴���޸��ļ�
	if (!is_same_name(curr_page, name, fcb)) {   //û�и��ļ�
		cout << "��ǰĿ¼�����ڸ��ļ���д��ʧ��" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);
	if (fcb.all_file[index].type == 0) {    //Ŀ¼�ļ�  �޷�д��
		cout << "���ļ���Ŀ¼�ļ���д��ʧ�ܣ�����" << endl;
		return;
	}

	if (fcb.all_file[index].type == 2) {    //��ִ���ļ�  �޷�д��
		cout << "���ļ��ǿ�ִ���ļ���д��ʧ�ܣ�����" << endl;
		return;
	}

	//�鿴�ļ��Ƿ��Ѿ���
	if (fcb.all_file[index].status == 0) {   //�ļ�δ��
		cout << "�ļ����ڹر�״̬��д��ʧ��" << endl;
		return;
	}

	//�鿴�Ƿ���дȨ��
	if (fcb.all_file[index].write_power == 0) {   //û��дȨ��
		cout << "��û��Ȩ��д����ļ�" << endl;
		return;
	}

	//��ʼд�ļ�
	cout << "��������Ҫд�������" << endl;
	string write_in = "";
	cin >> write_in;

	int write_size = write_in.size();   //д�����ݵĴ�С  ��λ���ֽ�

	//�жϿռ��Ƿ��㹻
	int remaining_block = empty_block(disk);       //ʣ����д��̿�
	int remaining_byte = get_remaining_byte(id, fcb, disk);    //д���ļ����һ�����ʣ����ֽ���
	if (write_size > 1024 * remaining_block + remaining_byte) {    //�ռ䲻��
		cout << "���̿ռ䲻�㣬д��ʧ�ܣ�����" << endl;
		return;
	}

	int last_block_index = fcb.all_file[index].block.size() - 1;    //�ļ����һ�����̿�����

	if (write_size <= remaining_byte) {    //���һ�����̿���Է���д�������
		fcb.all_file[index].size += write_size;     //�޸��ļ���С

		//�޸Ĵ��̿�����
		disk.block[fcb.all_file[index].block[last_block_index]].capacity += write_size;
		disk.block[fcb.all_file[index].block[last_block_index]].data += write_in;
		cout << "�ɹ�д���ļ�������" << endl;
		return;
	}

	//��Ҫ���Ӵ��̿�

	fcb.all_file[index].size += write_size;     //�޸��ļ���С
	int add_count = (write_size - remaining_byte) / 1024;    //��Ҫ���ӵĸ���
	if ((write_size - remaining_byte) % 1024 != 0) {    //�������÷��䣬��Ҫ��һ
		add_count++;
	}

	int ptr_index = 0;     //��¼��ǰд����������λ��
	string temp_str = write_in.substr(ptr_index, remaining_byte);
	disk.block[fcb.all_file[index].block[last_block_index]].capacity = 1024;
	disk.block[fcb.all_file[index].block[last_block_index]].data += temp_str;

	ptr_index += remaining_byte;      //ָ��ǰ��

	for (int i = 0; i < add_count; i++) {

		//�ҵ���һ�����п��
		int first_block;
		for (int i = 0; i < disk.block.size(); i++) {
			if (disk.block[i].flag == 0) {
				first_block = i;
				break;
			}
		}

		if (i != add_count - 1) {    //�����ļ����һ����
			fcb.all_file[index].block.push_back(first_block);  
			disk.block[first_block].capacity = 1024;
			temp_str = write_in.substr(ptr_index, 1024);
			ptr_index += 1024;
			disk.block[first_block].data += temp_str;
			disk.block[first_block].file_id = id;
			disk.block[first_block].flag = 1;
			disk.block[fcb.all_file[index].block[last_block_index]].next_id = first_block;
			last_block_index++;
		}

		else {        //�����һ����
			fcb.all_file[index].block.push_back(first_block);
			disk.block[first_block].capacity = write_size - remaining_byte - 1024 * (add_count - 1);
			temp_str = write_in.substr(ptr_index, write_size - remaining_byte - 1024 * (add_count - 1));
			disk.block[first_block].data += temp_str;
			disk.block[first_block].file_id = id;
			disk.block[first_block].flag = 1;
			disk.block[fcb.all_file[index].block[last_block_index]].next_id = first_block;
			last_block_index++;
		}
	}

	cout << "�ɹ�д���ļ�������" << endl;

}


//ִ���ļ�
void exe_file(FCB& fcb, int curr_page) {
	cout << "ѡ����Ҫִ�е��ļ�" << endl;
	string name;
	cin >> name;

	//��ǰĿ¼�鿴���޸��ļ�
	if (!is_same_name(curr_page, name, fcb)) {   //û�и��ļ�
		cout << "��ǰĿ¼�����ڸ��ļ���ִ��ʧ��" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);


	if (fcb.all_file[index].type != 2) {    //���ǿ�ִ���ļ�  �޷�ִ��
		cout << "���ļ����ǿ�ִ���ļ���ִ��ʧ�ܣ�����" << endl;
		return;
	}

	//�鿴�Ƿ���ִ��Ȩ��
	if (fcb.all_file[index].exe_power == 0) {   //û��ִ��Ȩ��
		cout << "��û��Ȩ��ִ�д��ļ�" << endl;
		return;
	}

	int time = rand() % 5 + 1;
	for (int i = 0; i < time; i++) {
		Sleep(1000);
		cout << "ִ����......" << endl;
	}

	cout << "ִ����ϣ�" << endl;

}

//ɾ���ļ���Ŀ¼
void delete_file(FCB& fcb, int curr_page, Disk& disk,string name) {
	

	//��ǰĿ¼�鿴���޸��ļ�
	if (!is_same_name(curr_page, name, fcb)) {   //û�и��ļ�
		cout << "��ǰĿ¼�����ڸ��ļ���ɾ��ʧ��" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);

	if (fcb.all_file[index].status > 0) {    //�ļ����ڴ�״̬���޷�ɾ��
		cout << "�ļ����ڴ�״̬���޷�ɾ��" << endl;
		return;
	}



	if (fcb.all_file[index].type == 1) {    //��ͨ�ļ�  ֱ��ɾ��

		//�ͷŴ��̿�
		for (int i = 0; i < fcb.all_file[index].block.size(); i++) {
			disk.block[fcb.all_file[index].block[i]].capacity = 0;
			disk.block[fcb.all_file[index].block[i]].data = "";
			disk.block[fcb.all_file[index].block[i]].file_id = -1;
			disk.block[fcb.all_file[index].block[i]].flag = 0;
			disk.block[fcb.all_file[index].block[i]].next_id = -1;
		}


		int catalog = fcb.all_file[index].in_which_file;
		int catalog_index = find_index(catalog, fcb);

		//ɾ�����е�����ļ���Ϣ
		swap(fcb.all_file[index], fcb.all_file[fcb.all_file.size() - 1]);
		fcb.all_file.pop_back();



		//ɾ�����Ŀ¼��Ϣ
		for (int i = 0; i < fcb.all_file[catalog_index].file_list.size(); i++) {
			if (fcb.all_file[catalog_index].file_list[i] == id) {
				swap(fcb.all_file[catalog_index].file_list[i], fcb.all_file[catalog_index].file_list[fcb.all_file[catalog_index].file_list.size() - 1]);
				fcb.all_file[catalog_index].file_list.pop_back();
				fcb.all_file[catalog_index].size -= 4;
			}
		}

	}

	else {                                 //Ŀ¼�ļ�   �ݹ�ɾ��       
		for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
			delete_file(fcb, id, disk, id_to_name(fcb,fcb.all_file[index].file_list[i]));
		}

		//���ɾ��Ŀ¼�����Ϣ
		//�ͷŴ��̿�
		for (int i = 0; i < fcb.all_file[index].block.size(); i++) {
			disk.block[fcb.all_file[index].block[i]].capacity = 0;
			disk.block[fcb.all_file[index].block[i]].data = "";
			disk.block[fcb.all_file[index].block[i]].file_id = -1;
			disk.block[fcb.all_file[index].block[i]].flag = 0;
			disk.block[fcb.all_file[index].block[i]].next_id = -1;
		}



		int catalog = fcb.all_file[index].in_which_file;
		int catalog_index = find_index(catalog, fcb);

		//ɾ�����е�����ļ���Ϣ
		swap(fcb.all_file[index], fcb.all_file[fcb.all_file.size() - 1]);
		fcb.all_file.pop_back();


		//ɾ�����Ŀ¼��Ϣ
		for (int i = 0; i < fcb.all_file[catalog_index].file_list.size(); i++) {
			if (fcb.all_file[catalog_index].file_list[i] == id) {
				swap(fcb.all_file[catalog_index].file_list[i], fcb.all_file[catalog_index].file_list[fcb.all_file[catalog_index].file_list.size() - 1]);
				fcb.all_file[catalog_index].file_list.pop_back();
				fcb.all_file[catalog_index].size -= 4;
			}
		}
	}

}

//��¼����ԱȨ��
int login_admin(string password) {
	cout << "����������" << endl;
	string pd;
	cin >> pd;
	if (pd != password) {   //�������
		cout << "����������󣡣���" << endl;
		return 0;
	}

	return 1;

}


//����Ա����
void show_admin_win() {
	cout << " -----------------------------------------------" << endl;
	cout << "|            1�� ������һ��Ŀ¼                 |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            2�� �г���ǰĿ¼�������ļ�         |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            3�� �޸��ļ�Ȩ��                   |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            4�� �鿴���̿ռ�ʹ�����           |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            5�� ����Ŀ¼                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            6�� �ص��û�����                   |" << endl;
	cout << "|                                               |" << endl;
	cout << " -----------------------------------------------" << endl;
}

//����Ŀ¼
void enter_catalog_admin(int& curr_page, FCB& fcb) {
	cout << "ѡ����Ҫ�����Ŀ¼" << endl;
	string catalog;
	cin >> catalog;

	if (!is_same_name(curr_page, catalog, fcb)) {
		cout << "�����Ŀ¼�����ڵ�ǰĿ¼��" << endl;
		return;
	}


	int id = name_to_id(fcb, catalog, curr_page);
	int index = find_index(id, fcb);

	if (fcb.all_file[index].type != 0) {   //����Ŀ¼�ļ�
		cout << "������ļ�����Ŀ¼�ļ�������ʧ�ܣ�����" << endl;
		return;
	}

	curr_page = fcb.all_file[index].id;

}

//�޸��ļ�Ȩ��
void modify_power(FCB& fcb, int curr_page, Disk& disk) {
	cout << "ѡ����Ҫ�޸ĵ��ļ�" << endl;
	string name;
	cin >> name;

	//��ǰĿ¼�鿴���޸��ļ�
	if (!is_same_name(curr_page, name, fcb)) {   //û�и��ļ�
		cout << "��ǰĿ¼�����ڸ��ļ����޸�ʧ��" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);

	cout << "�������޸ĺ��Ȩ�ޣ�1��ʾ�д�Ȩ�ޣ�0��ʾû��    ��������λ��������    ˳������Ϊ����д��ִ��" << endl;
	string power;
	cin >> power;
	if (power == "000" || power == "001" || power == "010" || power == "011" || power == "100" || power == "101" || power == "110" || power == "111") {
		fcb.all_file[index].read_power = power[0] -= '0';
		fcb.all_file[index].write_power = power[1] -= '0';
		fcb.all_file[index].exe_power = power[2] -= '0';
	}

	else {
		cout << "�������ݲ��Ϸ�������" << endl;
	}

}


//�鿴���̿ռ�ʹ�����
void show_disk_use(Disk& disk, FCB& fcb) {
	
	cout << "���̿��\t" << "�Ƿ񱻷���\t" << "����ʹ�ÿռ�\t" << "�����ļ�id\t" << "���ͬһ�ļ�����һ���̿��\t" << endl;

	for (int i = 0; i < disk.block.size(); i++) {
		cout << disk.block[i].id << "\t\t" << disk.block[i].flag << "\t\t" << disk.block[i].capacity << "\t\t" << disk.block[i].file_id << "\t\t" << disk.block[i].next_id << endl;
		}

	cout << endl;
}


//��Ŀ¼
void list_all_file_admin(FCB& fcb, int& curr_page) {

	int index = find_index(curr_page, fcb);


	if (fcb.all_file[index].file_list.size() == 0) {  //Ŀ¼Ϊ��
		cout << "��ǰĿ¼Ϊ��" << endl;
		return;
	}

	cout << "��ǰĿ¼�ļ����£�" << endl;
	for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
		int temp_index = find_index(fcb.all_file[index].file_list[i], fcb);
		cout << fcb.all_file[temp_index].name << "  ";
		if (fcb.all_file[temp_index].type == 0) {
			cout << "Ŀ¼�ļ�" << endl;
		}
		else {
			cout << "��ͨ�ļ�" << endl;
		}
	}
}



//�����溯��
void show_main() {
	cout << " -----------------------------------------------" << endl;
	cout << "|            1�� ������һ��Ŀ¼                 |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            2�� �г���ǰĿ¼�������ļ�         |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            3�� �����ļ�                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            4�� ���ļ�                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            5�� �ر��ļ�                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            6�� ��ȡ�ļ�                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            7�� д���ļ�                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            8�� ִ���ļ�                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            9�� ɾ���ļ���Ŀ¼                 |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            10�� ����Ŀ¼                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            11�� ��¼����ԱȨ��                |" << endl;
	cout << "|                                               |" << endl;
	cout << " -----------------------------------------------" << endl;
}





int main() {

	Disk disk(100);

	int curr_page = 0;   //��ǰĿ¼��   0��ʾ��Ŀ¼

	int index = 1;       //�����������ļ�id��  ��1��ʼ

	FCB fcb;

	File root(0, "root", -1, 0, disk);

	fcb.all_file.push_back(root);

	string password = "123456";
	
	lable1: while (1) {
		cout << "��ǰ����Ŀ¼Ϊ��" << find_path(fcb, curr_page) << endl;
		show_main();
		cout << "������Ҫִ�еĲ�����";
		string str;
		int select;
		cin >> str;
		if (str == "1" || str == "2" || str == "3" || str == "4" || str == "5" || str == "6" || str == "7" || str == "8" || str == "9" || str == "10"||str=="11") {
			select = stoi(str);
		}
		else {
			cout << "�������ݲ��Ϸ������������룡����" << endl;
			system("pause");
			system("cls");
			continue;
		}

		string ff;     //��ʾɾ�����ļ���Ŀ¼
		int res;       //��ʾ��¼������ȷ���

		switch (select) {
		case 1:     //������һ��Ŀ¼
				back_last_catalog(curr_page, fcb);
				system("pause");
				system("cls");
		    	break;
		    
		case 2:     //�г���ǰĿ¼�������ļ�
				list_all_file(fcb, curr_page);
				system("pause");
				system("cls");
		    	break;
		    
		case 3:     //�����ļ�
				create_file(fcb, disk, curr_page, index);
				system("pause");
				system("cls");
		    	break;

		case 4:      //���ļ�
				open_file(fcb, curr_page);
				system("pause");
				system("cls");
				break;

		case 5:      //�ر��ļ�
				close_file(fcb, curr_page);
				system("pause");
				system("cls");
				break;
		    
		case 6:     //��ȡ�ļ�
				read_file(fcb, curr_page, disk);
				system("pause");
				system("cls");
		    	break;
		    
		case 7:     //д���ļ�
				write_file(fcb, curr_page, disk);
				system("pause");
				system("cls");
		    	break;

		case 8:     //ִ���ļ�
			exe_file(fcb, curr_page);
			system("pause");
			system("cls");
			break;
		    
		case 9:     //ɾ���ļ���Ŀ¼
				cout << "��������Ҫɾ�����ļ���Ŀ¼" << endl;
				cin >> ff;
				delete_file(fcb, curr_page, disk,ff);
				system("pause");
				system("cls");
		    	break;
		    
		case 10:     //����Ŀ¼
				enter_catalog(curr_page, fcb);
				system("pause");
				system("cls");
		    	break;

		case 11:     //��¼����ԱȨ��
			res = login_admin(password);
			if (res == 1) {
				goto lable2;
			}

			system("pause");
			system("cls");
			break;
		    
		    default:    //�������
				cout << "�������ݲ��Ϸ������������룡����" << endl;
				system("pause");
				system("cls");
		    	break;

		}
	}



	//����Աģʽ
lable2:
	system("cls");
	while (1) {
		cout << "��ǰ����Ŀ¼Ϊ��" << find_path(fcb, curr_page) << endl;
		show_admin_win();
		cout << "������Ҫִ�еĲ�����";
		string str;
		int select;
		cin >> str;
		if (str == "1" || str == "2" || str == "3" || str == "4" || str == "5" || str == "6" ) {
			select = stoi(str);
		}
		else {
			cout << "�������ݲ��Ϸ������������룡����" << endl;
			system("pause");
			system("cls");
			continue;
		}


		switch (select) {
		case 1:     //������һ��Ŀ¼
			back_last_catalog(curr_page, fcb);
			system("pause");
			system("cls");
			break;

		case 2:     //�г���ǰĿ¼�������ļ�
			list_all_file_admin(fcb, curr_page);
			system("pause");
			system("cls");
			break;

		case 3:     //�޸��ļ�Ȩ��
			modify_power(fcb, curr_page, disk);
			system("pause");
			system("cls");
			break;

		case 4:      //�鿴���̿ռ�ʹ�����
			show_disk_use(disk, fcb);
			system("pause");
			system("cls");
			break;

		case 5:      //����Ŀ¼
			enter_catalog_admin(curr_page, fcb);
			system("pause");
			system("cls");
			break;

		case 6:     //�ص��û�����
			system("cls");
			goto lable1;
			break;


		default:    //�������
			cout << "�������ݲ��Ϸ������������룡����" << endl;
			system("pause");
			system("cls");
			break;

		}
	}

	return 0;
}