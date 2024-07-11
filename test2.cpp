#include <iostream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

//�����ṹ��
struct phy_block {
	int id;     //������
	int flag;    //��־λ����ʾ�Ƿ���������
	int fflag;   //��־λ����ʾ�Ƿ�����ҳ��

	phy_block(int id) {
		this->id = id;
		this->flag = 0;
		this->fflag = 0;
	}
};

//�ڴ�ṹ��
struct Ram
{
	vector<phy_block>pb_arr;    //���������

	//��ʼ���ڴ��
	Ram(int count) {
		for (int i = 0; i < count; i++) {
			phy_block pb(i);
			pb_arr.push_back(pb);
		}
	}
};


//ҳ����ṹ��
struct PageTableTerm {
	int phy_block_id;   //������
	int flag;           //��־λ����ʾ��ҳ�Ƿ����ڴ���
	int time;          //ʣ��ʱ�䣬������ʾ���ʹ�����
	int clock_flag;     //clock�㷨��־λ
	
	//��ʼ��
	PageTableTerm() {

	}

	PageTableTerm(int phy_block_id, int flag,int clock_flag) {
		this->phy_block_id = phy_block_id;
		this->flag = flag;
		this->clock_flag = clock_flag;
		time = 1000;
	}

};

//���̽ṹ��
struct Process {
	int page_count;                     //����ҳ�����
	vector<PageTableTerm>page_table;   //����ҳ��
	vector<int>physical_block;         //���̷�������������
	queue<int>page;                    //ҳ�ŷ������ ���ں�����fifo

	//��ʼ��
	Process(){

	}

	Process(int page_count) {
		this->page_count = page_count;
		//��ʼ��ҳ��
		for (int i = 0; i < page_count; i++) {
			PageTableTerm ptt(-1, 0, 1);
			page_table.push_back(ptt);
		}
	}

	//������������
	void request_phy_block(int count,Ram &ram) {
		int number = 0;
		
		//�ҵ���һ��δ�������������
			for (int i = 0; i < ram.pb_arr.size(); i++) {
				if (ram.pb_arr[i].flag == 0) {
					number++;
					ram.pb_arr[i].flag = 1;
					this->physical_block.push_back(i);

					if (number == count) {
						break;
					}
				}
			}
		
	}

	//�����ͷ������
	void release_phy_block(int id,Ram&ram) {

		if (ram.pb_arr[id].fflag == 1) {
			cout << "������ѷ����ҳ�棬�ͷ�ʧ��" << endl;
			return;
		}
		ram.pb_arr[id].flag = 0;
		for (int i = 0; i < this->physical_block.size(); i++) {
			if (this->physical_block[i] == id) {
				swap(this->physical_block[i], this->physical_block[this->physical_block.size() - 1]);
				this->physical_block.pop_back();
				break;
			}
		}
	}

};

//չʾ����ҳ�����Ϣ
void show_process(Process& p) {
	cout << "�ý��̷�����������Ϊ��" << p.physical_block.size() << endl;
	cout << "����ҳ�����£�" << endl;
	cout << "ҳ��\t" << "������\t" << "�Ƿ����ڴ���\t" << endl;
	for (int i = 0; i < p.page_table.size(); i++) {
		cout << i << "\t" << p.page_table[i].phy_block_id << "\t\t" << p.page_table[i].flag << endl << endl;
	}
}

//�жϽ��̷��ʵ�ҳ���Ƿ����ڴ���
bool is_in_ram(int page, Process& p) {

	return p.page_table[page].flag == 1;
	
}

//�жϷ����������Ƿ�ȫ��ʹ��
bool is_all_used(Process& p) {
	int count = 0;
	for (int i = 0; i < p.page_table.size(); i++) {
		if (p.page_table[i].flag == 1) {
			count++;
		}
	}
	return count == p.physical_block.size();
}

//fifo����
void fifo(Process& p, int page,Ram &ram,int &count) {
	if (page > p.page_count - 1) {   //Խ��
		cout << "����Խ�磡����" << endl;
		return;
	}

	if (is_in_ram(page, p)) { //���ʵ�ҳ�����ڴ���
		cout << "����ҳ�����ڴ���" << endl;
		return;
	}

	if (!is_in_ram(page, p) && !is_all_used(p)) {  //�����ڴ浫�����п��������
		for (int i = 0; i < p.physical_block.size(); i++) {
			if (ram.pb_arr[p.physical_block[i]].fflag == 0) {  //�ҵ���һ�������п����
				ram.pb_arr[p.physical_block[i]].fflag = 1;
				p.page_table[page].flag = 1;
				p.page_table[page].phy_block_id = p.physical_block[i];
				break;
			}
		}
		p.page.push(page);    //�������
		count++;
		return;
	}

	if (!is_in_ram(page, p) && is_all_used(p)) {    //�����ڴ浫�ǿ���������Ѿ��������
		int temp = p.page.front();
		p.page.pop();
		p.page_table[temp].flag = 0;
		p.page_table[page].flag = 1;
		p.page_table[page].phy_block_id = p.page_table[temp].phy_block_id;
		p.page.push(page);
		count++;     //ȱҳ������һ
		return;
	}

}

//lru����
void lru(Process& p, int page, Ram& ram, int& count) {
	if (page > p.page_count - 1) {   //Խ��
		cout << "����Խ�磡����" << endl;
		return;
	}

	if (is_in_ram(page, p)) { //���ʵ�ҳ�����ڴ���
		cout << "����ҳ�����ڴ���" << endl;
		p.page_table[page].time = 1000;     //�ָ�����ʼֵ
		return;
	}

	if (!is_in_ram(page, p) && !is_all_used(p)) {  //�����ڴ浫�����п��������
		for (int i = 0; i < p.physical_block.size(); i++) {
			if (ram.pb_arr[p.physical_block[i]].fflag == 0) {  //�ҵ���һ�������п����
				ram.pb_arr[p.physical_block[i]].fflag = 1;
				p.page_table[page].flag = 1;
				p.page_table[page].phy_block_id = p.physical_block[i];
				break;
			}
		}

		//��ҳ�����������ڴ��е�ҳ��ʱ���һ
			  
		for (int i = 0; i < p.page_table.size(); i++) {
			if (p.page_table[i].flag == 1) {
				p.page_table[i].time--;
			}
		}		
		count++;      //ȱҳ������һ
		return;
	}

	if (!is_in_ram(page, p) && is_all_used(p)) {    //�����ڴ浫�ǿ���������Ѿ��������
		int temp;           //��ʾ������δʹ�õ�ҳ��
		int mmin = 1000;

		for (int i = 0; i < p.page_table.size(); i++) {
			if (p.page_table[i].flag == 1) {
				if (p.page_table[i].time <= mmin) {   //�и���δʹ�õ�ҳ��
					mmin = p.page_table[i].time;
					temp = i;
				}
			}
		}
		
		p.page_table[temp].flag = 0;
		p.page_table[page].flag = 1;
		p.page_table[page].phy_block_id = p.page_table[temp].phy_block_id;
		p.page_table[page].time = 1000;

		//��ҳ�����������ڴ��е�ҳ��ʱ���һ

		for (int i = 0; i < p.page_table.size(); i++) {
			if (p.page_table[i].flag == 1) {
				p.page_table[i].time--;
			}
		}

		count++;     //ȱҳ������һ
		return;
	}

}

//clock����
void clock(Process& p, int page, Ram& ram, int& count,int &index) {

	if (page > p.page_count - 1) {   //Խ��
		cout << "����Խ�磡����" << endl;
		return;
	}

	cout << "����ǰָ������Ϊ��" << index << endl;

	if (is_in_ram(page, p)) { //���ʵ�ҳ�����ڴ���
		cout << "����ҳ�����ڴ���" << endl;
		p.page_table[page].clock_flag = 1;     //�ָ�clock��־λ
		return;
	}

	if (!is_in_ram(page, p) && !is_all_used(p)) {  //�����ڴ浫�����п��������
		for (int i = 0; i < p.physical_block.size(); i++) {
			if (ram.pb_arr[p.physical_block[i]].fflag == 0) {  //�ҵ���һ�������п����
				ram.pb_arr[p.physical_block[i]].fflag = 1;
				p.page_table[page].flag = 1;
				p.page_table[page].phy_block_id = p.physical_block[i];
				p.page_table[page].clock_flag = 1;
				break;
			}
		}

		count++;      //ȱҳ������һ
		return;
	}

	if (!is_in_ram(page, p) && is_all_used(p)) {    //�����ڴ浫�ǿ���������Ѿ��������
		int temp;           //��ʾ�滻��ҳ��

		while (1) {

			if (p.page_table[index].flag == 1 && p.page_table[index].clock_flag == 0) {
				temp = index;
				index = (index + 1) % p.page_count;
				break;
			}

			if (p.page_table[index].flag == 1 && p.page_table[index].clock_flag == 1) {
				p.page_table[index].clock_flag = 0;			
			}
			index = (index + 1) % p.page_count;       //ָ��ǰ��
		}

		p.page_table[temp].flag = 0;
		p.page_table[page].flag = 1;
		p.page_table[page].phy_block_id = p.page_table[temp].phy_block_id;
		p.page_table[page].clock_flag = 1;


		count++;     //ȱҳ������һ
		return;
	}

}

//����滻����
void ran(Process& p, int page, Ram& ram, int& count) {
	if (page > p.page_count - 1) {   //Խ��
		cout << "����Խ�磡����" << endl;
		return;
	}

	if (is_in_ram(page, p)) { //���ʵ�ҳ�����ڴ���
		cout << "����ҳ�����ڴ���" << endl;
		return;
	}

	if (!is_in_ram(page, p) && !is_all_used(p)) {  //�����ڴ浫�����п��������
		for (int i = 0; i < p.physical_block.size(); i++) {
			if (ram.pb_arr[p.physical_block[i]].fflag == 0) {  //�ҵ���һ�������п����
				ram.pb_arr[p.physical_block[i]].fflag = 1;
				p.page_table[page].flag = 1;
				p.page_table[page].phy_block_id = p.physical_block[i];
				break;
			}
		}
		count++;
		return;
	}

	if (!is_in_ram(page, p) && is_all_used(p)) {    //�����ڴ浫�ǿ���������Ѿ��������
		int temp = rand() % p.page_count;
		while (p.page_table[temp].flag == 0) {
			temp = rand() % p.page_count;
		}
		
		
		p.page_table[temp].flag = 0;
		p.page_table[page].flag = 1;
		p.page_table[page].phy_block_id = p.page_table[temp].phy_block_id;
		count++;     //ȱҳ������һ
		return;
	}
}

//��ַת������   �߼���ַΪ8λ2���ƴ�  ǰ��λΪҳ��
string address_convert(Process& p, string logical_address,Ram &ram) {
	string page_id = logical_address.substr(0, 3);
	string offset = logical_address.substr(3, 5);
	int count = 0;
	int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');

	if (page >= p.page_count) {
		cout << "��ַԽ�磡����" << endl;
		return "";
	}

	if (p.page_table[page].flag == 0) {
		//ִ��ҳ���û�����  ��lruΪ��
		lru(p, page, ram, count);
	}
	int py = p.page_table[page].phy_block_id;
	string py_id = "";
	if (py >= 4) {
		py -= 4;
		py_id += "1";
	}
	else {
		py_id += "0";
	}

	if (py >= 2) {
		py -= 2;
		py_id += "1";
	}
	else {
		py_id += "0";
	}

	if (py >= 1) {
		py -= 1;
		py_id += "1";
	}
	else {
		py_id += "0";
	}
	
	string res = py_id + offset;
	return res;
}


bool correct_address(string str) {
	if (str.size() != 8) {
		return 0;
	}
	for (int i = 0; i < 8; i++) {
		if (str[i] != '0' && str[i] != '1') {
			return 0;
		}
	}
	return 1;
}

int main() {


	Ram ram(100);   //�����ڴ��СΪ100

	cout << "���������ҳ����" << endl;
	int num;
	cin >> num;
	Process p(num);

	cout << "����������̷�����������Ŀ" << endl;
	int bl;
	cin >> bl;
	p.request_phy_block(bl,ram);
	int count = 0;

	int index;

	lable1:
	cout << "��������Ҫѡ���ҳ���û��㷨��" << endl;
	cout << "1��fifo" << endl;
	cout << "2��lru" << endl;
	cout << "3��clock" << endl;
	cout << "4������û�" << endl;

	int select;
	cin >> select;

	switch (select) {
		//fifo����
	case 1:
		count = 0;
		while (1)
		{
			cout << "fifo����" << endl;
			
			cout << "��������Ҫ���ʵĵ�ַ" << endl;
			string pg;
			cin >> pg;
			if (!correct_address(pg)) {
				cout << "�������ݲ��Ϸ�����������" << endl;
				system("pause");
				system("cls");
				continue;
			}

			string page_id = pg.substr(0, 3);
			string offset = pg.substr(3, 5);
			
			int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');


			fifo(p, page, ram, count);
			if (page < num&&page>=0) {   //δԽ��
				cout << "���η��������ַΪ��" << address_convert(p, pg, ram);
			}
			show_process(p);
			cout << "Ŀǰȱҳ�ܴ���Ϊ��" << count << endl;
			system("pause");
			system("cls");
		}
		  break;

	case 2:
		count = 0;
		while (1) {
			//lru����
			cout << "lru����" << endl;
			
			cout << "��������Ҫ���ʵĵ�ַ" << endl;
			string pg;
			cin >> pg;
			if (!correct_address(pg)) {
				cout << "�������ݲ��Ϸ�����������" << endl;
				system("pause");
				system("cls");
				continue;
			}

			string page_id = pg.substr(0, 3);
			string offset = pg.substr(3, 5);

			int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');
			lru(p, page, ram, count);
			show_process(p);

			if (page < num && page >= 0) {   //δԽ��
				cout << "���η��������ַΪ��" << address_convert(p, pg, ram);
			}

			cout << "ȱҳ�ܴ���Ϊ��" << count << endl << endl;
			system("pause");
			system("cls");
		}

		  break;

		//clock����
	case 3:
		index = 0;
		count = 0;
		while (1) {
			cout << "clock����" << endl;
			

			cout << "��������Ҫ���ʵĵ�ַ" << endl;
			string pg;
			cin >> pg;
			if (!correct_address(pg)) {
				cout << "�������ݲ��Ϸ�����������" << endl;
				system("pause");
				system("cls");
				continue;
			}

			string page_id = pg.substr(0, 3);
			string offset = pg.substr(3, 5);

			int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');
			clock(p, page, ram, count, index);
			show_process(p);
			if (page < num && page >= 0) {   //δԽ��
				cout << "���η��������ַΪ��" << address_convert(p, pg, ram);
			}
			cout << "ȱҳ�ܴ���Ϊ��" << count << endl << endl;
			cout << "��ʱҳ������ָ��Ϊ��" << index << endl;
			system("pause");
			system("cls");
		}

		break;
		//������Բ���
	case 4:
		count = 0;
		while (1) {
			cout << "�������" << endl;
			

			cout << "��������Ҫ���ʵĵ�ַ" << endl;
			string pg;
			cin >> pg;
			if (!correct_address(pg)) {
				cout << "�������ݲ��Ϸ�����������" << endl;
				system("pause");
				system("cls");
				continue;
			}

			string page_id = pg.substr(0, 3);
			string offset = pg.substr(3, 5);

			int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');
			ran(p, page, ram, count);
			show_process(p);
			if (page < num && page >= 0) {   //δԽ��
				cout << "���η��������ַΪ��" << address_convert(p, pg, ram);
			}
			cout << "ȱҳ�ܴ���Ϊ��" << count << endl;
		}
		  break;

	default:cout << "�������" << endl;
		system("pause");
		system("cls");
		goto  lable1;
	}

	

	//cout << "10110000�������ַΪ��" << address_convert(p, "10110000", ram) << endl;

	return 0;
}



