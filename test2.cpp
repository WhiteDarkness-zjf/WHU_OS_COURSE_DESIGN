#include <iostream>
#include <vector>
#include <queue>
#include <string>

using namespace std;

//物理块结构体
struct phy_block {
	int id;     //物理块号
	int flag;    //标志位，表示是否分配给进程
	int fflag;   //标志位，表示是否分配给页面

	phy_block(int id) {
		this->id = id;
		this->flag = 0;
		this->fflag = 0;
	}
};

//内存结构体
struct Ram
{
	vector<phy_block>pb_arr;    //物理块数组

	//初始化内存块
	Ram(int count) {
		for (int i = 0; i < count; i++) {
			phy_block pb(i);
			pb_arr.push_back(pb);
		}
	}
};


//页表项结构体
struct PageTableTerm {
	int phy_block_id;   //物理块号
	int flag;           //标志位，表示该页是否在内存中
	int time;          //剩余时间，用来表示最近使用情况
	int clock_flag;     //clock算法标志位
	
	//初始化
	PageTableTerm() {

	}

	PageTableTerm(int phy_block_id, int flag,int clock_flag) {
		this->phy_block_id = phy_block_id;
		this->flag = flag;
		this->clock_flag = clock_flag;
		time = 1000;
	}

};

//进程结构体
struct Process {
	int page_count;                     //进程页面个数
	vector<PageTableTerm>page_table;   //进程页表
	vector<int>physical_block;         //进程分配的物理块数组
	queue<int>page;                    //页号分配队列 便于后续的fifo

	//初始化
	Process(){

	}

	Process(int page_count) {
		this->page_count = page_count;
		//初始化页表
		for (int i = 0; i < page_count; i++) {
			PageTableTerm ptt(-1, 0, 1);
			page_table.push_back(ptt);
		}
	}

	//请求分配物理块
	void request_phy_block(int count,Ram &ram) {
		int number = 0;
		
		//找到第一个未分配的物理块分配
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

	//请求释放物理块
	void release_phy_block(int id,Ram&ram) {

		if (ram.pb_arr[id].fflag == 1) {
			cout << "物理块已分配给页面，释放失败" << endl;
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

//展示进程页表等信息
void show_process(Process& p) {
	cout << "该进程分配的物理块数为：" << p.physical_block.size() << endl;
	cout << "进程页表如下：" << endl;
	cout << "页号\t" << "物理块号\t" << "是否在内存中\t" << endl;
	for (int i = 0; i < p.page_table.size(); i++) {
		cout << i << "\t" << p.page_table[i].phy_block_id << "\t\t" << p.page_table[i].flag << endl << endl;
	}
}

//判断进程访问的页面是否在内存中
bool is_in_ram(int page, Process& p) {

	return p.page_table[page].flag == 1;
	
}

//判断分配的物理块是否全部使用
bool is_all_used(Process& p) {
	int count = 0;
	for (int i = 0; i < p.page_table.size(); i++) {
		if (p.page_table[i].flag == 1) {
			count++;
		}
	}
	return count == p.physical_block.size();
}

//fifo策略
void fifo(Process& p, int page,Ram &ram,int &count) {
	if (page > p.page_count - 1) {   //越界
		cout << "访问越界！！！" << endl;
		return;
	}

	if (is_in_ram(page, p)) { //访问的页面在内存中
		cout << "访问页面在内存中" << endl;
		return;
	}

	if (!is_in_ram(page, p) && !is_all_used(p)) {  //不在内存但是仍有空闲物理块
		for (int i = 0; i < p.physical_block.size(); i++) {
			if (ram.pb_arr[p.physical_block[i]].fflag == 0) {  //找到第一个不空闲块分配
				ram.pb_arr[p.physical_block[i]].fflag = 1;
				p.page_table[page].flag = 1;
				p.page_table[page].phy_block_id = p.physical_block[i];
				break;
			}
		}
		p.page.push(page);    //插入队列
		count++;
		return;
	}

	if (!is_in_ram(page, p) && is_all_used(p)) {    //不在内存但是空闲物理块已经分配完毕
		int temp = p.page.front();
		p.page.pop();
		p.page_table[temp].flag = 0;
		p.page_table[page].flag = 1;
		p.page_table[page].phy_block_id = p.page_table[temp].phy_block_id;
		p.page.push(page);
		count++;     //缺页次数加一
		return;
	}

}

//lru策略
void lru(Process& p, int page, Ram& ram, int& count) {
	if (page > p.page_count - 1) {   //越界
		cout << "访问越界！！！" << endl;
		return;
	}

	if (is_in_ram(page, p)) { //访问的页面在内存中
		cout << "访问页面在内存中" << endl;
		p.page_table[page].time = 1000;     //恢复到初始值
		return;
	}

	if (!is_in_ram(page, p) && !is_all_used(p)) {  //不在内存但是仍有空闲物理块
		for (int i = 0; i < p.physical_block.size(); i++) {
			if (ram.pb_arr[p.physical_block[i]].fflag == 0) {  //找到第一个不空闲块分配
				ram.pb_arr[p.physical_block[i]].fflag = 1;
				p.page_table[page].flag = 1;
				p.page_table[page].phy_block_id = p.physical_block[i];
				break;
			}
		}

		//将页表中其它在内存中的页面时间减一
			  
		for (int i = 0; i < p.page_table.size(); i++) {
			if (p.page_table[i].flag == 1) {
				p.page_table[i].time--;
			}
		}		
		count++;      //缺页次数加一
		return;
	}

	if (!is_in_ram(page, p) && is_all_used(p)) {    //不在内存但是空闲物理块已经分配完毕
		int temp;           //表示最近最久未使用的页面
		int mmin = 1000;

		for (int i = 0; i < p.page_table.size(); i++) {
			if (p.page_table[i].flag == 1) {
				if (p.page_table[i].time <= mmin) {   //有更久未使用的页面
					mmin = p.page_table[i].time;
					temp = i;
				}
			}
		}
		
		p.page_table[temp].flag = 0;
		p.page_table[page].flag = 1;
		p.page_table[page].phy_block_id = p.page_table[temp].phy_block_id;
		p.page_table[page].time = 1000;

		//将页表中其它在内存中的页面时间减一

		for (int i = 0; i < p.page_table.size(); i++) {
			if (p.page_table[i].flag == 1) {
				p.page_table[i].time--;
			}
		}

		count++;     //缺页次数加一
		return;
	}

}

//clock策略
void clock(Process& p, int page, Ram& ram, int& count,int &index) {

	if (page > p.page_count - 1) {   //越界
		cout << "访问越界！！！" << endl;
		return;
	}

	cout << "访问前指针索引为：" << index << endl;

	if (is_in_ram(page, p)) { //访问的页面在内存中
		cout << "访问页面在内存中" << endl;
		p.page_table[page].clock_flag = 1;     //恢复clock标志位
		return;
	}

	if (!is_in_ram(page, p) && !is_all_used(p)) {  //不在内存但是仍有空闲物理块
		for (int i = 0; i < p.physical_block.size(); i++) {
			if (ram.pb_arr[p.physical_block[i]].fflag == 0) {  //找到第一个不空闲块分配
				ram.pb_arr[p.physical_block[i]].fflag = 1;
				p.page_table[page].flag = 1;
				p.page_table[page].phy_block_id = p.physical_block[i];
				p.page_table[page].clock_flag = 1;
				break;
			}
		}

		count++;      //缺页次数加一
		return;
	}

	if (!is_in_ram(page, p) && is_all_used(p)) {    //不在内存但是空闲物理块已经分配完毕
		int temp;           //表示替换的页面

		while (1) {

			if (p.page_table[index].flag == 1 && p.page_table[index].clock_flag == 0) {
				temp = index;
				index = (index + 1) % p.page_count;
				break;
			}

			if (p.page_table[index].flag == 1 && p.page_table[index].clock_flag == 1) {
				p.page_table[index].clock_flag = 0;			
			}
			index = (index + 1) % p.page_count;       //指针前移
		}

		p.page_table[temp].flag = 0;
		p.page_table[page].flag = 1;
		p.page_table[page].phy_block_id = p.page_table[temp].phy_block_id;
		p.page_table[page].clock_flag = 1;


		count++;     //缺页次数加一
		return;
	}

}

//随机替换策略
void ran(Process& p, int page, Ram& ram, int& count) {
	if (page > p.page_count - 1) {   //越界
		cout << "访问越界！！！" << endl;
		return;
	}

	if (is_in_ram(page, p)) { //访问的页面在内存中
		cout << "访问页面在内存中" << endl;
		return;
	}

	if (!is_in_ram(page, p) && !is_all_used(p)) {  //不在内存但是仍有空闲物理块
		for (int i = 0; i < p.physical_block.size(); i++) {
			if (ram.pb_arr[p.physical_block[i]].fflag == 0) {  //找到第一个不空闲块分配
				ram.pb_arr[p.physical_block[i]].fflag = 1;
				p.page_table[page].flag = 1;
				p.page_table[page].phy_block_id = p.physical_block[i];
				break;
			}
		}
		count++;
		return;
	}

	if (!is_in_ram(page, p) && is_all_used(p)) {    //不在内存但是空闲物理块已经分配完毕
		int temp = rand() % p.page_count;
		while (p.page_table[temp].flag == 0) {
			temp = rand() % p.page_count;
		}
		
		
		p.page_table[temp].flag = 0;
		p.page_table[page].flag = 1;
		p.page_table[page].phy_block_id = p.page_table[temp].phy_block_id;
		count++;     //缺页次数加一
		return;
	}
}

//地址转换函数   逻辑地址为8位2进制串  前三位为页号
string address_convert(Process& p, string logical_address,Ram &ram) {
	string page_id = logical_address.substr(0, 3);
	string offset = logical_address.substr(3, 5);
	int count = 0;
	int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');

	if (page >= p.page_count) {
		cout << "地址越界！！！" << endl;
		return "";
	}

	if (p.page_table[page].flag == 0) {
		//执行页面置换策略  以lru为例
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


	Ram ram(100);   //设置内存大小为100

	cout << "请输入进程页面数" << endl;
	int num;
	cin >> num;
	Process p(num);

	cout << "请输入给进程分配的物理块数目" << endl;
	int bl;
	cin >> bl;
	p.request_phy_block(bl,ram);
	int count = 0;

	int index;

	lable1:
	cout << "请输入你要选择的页面置换算法：" << endl;
	cout << "1、fifo" << endl;
	cout << "2、lru" << endl;
	cout << "3、clock" << endl;
	cout << "4、随机置换" << endl;

	int select;
	cin >> select;

	switch (select) {
		//fifo测试
	case 1:
		count = 0;
		while (1)
		{
			cout << "fifo策略" << endl;
			
			cout << "请输入你要访问的地址" << endl;
			string pg;
			cin >> pg;
			if (!correct_address(pg)) {
				cout << "输入数据不合法，重新输入" << endl;
				system("pause");
				system("cls");
				continue;
			}

			string page_id = pg.substr(0, 3);
			string offset = pg.substr(3, 5);
			
			int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');


			fifo(p, page, ram, count);
			if (page < num&&page>=0) {   //未越界
				cout << "本次访问物理地址为：" << address_convert(p, pg, ram);
			}
			show_process(p);
			cout << "目前缺页总次数为：" << count << endl;
			system("pause");
			system("cls");
		}
		  break;

	case 2:
		count = 0;
		while (1) {
			//lru测试
			cout << "lru策略" << endl;
			
			cout << "请输入你要访问的地址" << endl;
			string pg;
			cin >> pg;
			if (!correct_address(pg)) {
				cout << "输入数据不合法，重新输入" << endl;
				system("pause");
				system("cls");
				continue;
			}

			string page_id = pg.substr(0, 3);
			string offset = pg.substr(3, 5);

			int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');
			lru(p, page, ram, count);
			show_process(p);

			if (page < num && page >= 0) {   //未越界
				cout << "本次访问物理地址为：" << address_convert(p, pg, ram);
			}

			cout << "缺页总次数为：" << count << endl << endl;
			system("pause");
			system("cls");
		}

		  break;

		//clock测试
	case 3:
		index = 0;
		count = 0;
		while (1) {
			cout << "clock策略" << endl;
			

			cout << "请输入你要访问的地址" << endl;
			string pg;
			cin >> pg;
			if (!correct_address(pg)) {
				cout << "输入数据不合法，重新输入" << endl;
				system("pause");
				system("cls");
				continue;
			}

			string page_id = pg.substr(0, 3);
			string offset = pg.substr(3, 5);

			int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');
			clock(p, page, ram, count, index);
			show_process(p);
			if (page < num && page >= 0) {   //未越界
				cout << "本次访问物理地址为：" << address_convert(p, pg, ram);
			}
			cout << "缺页总次数为：" << count << endl << endl;
			cout << "此时页面索引指针为：" << index << endl;
			system("pause");
			system("cls");
		}

		break;
		//随机策略测试
	case 4:
		count = 0;
		while (1) {
			cout << "随机策略" << endl;
			

			cout << "请输入你要访问的地址" << endl;
			string pg;
			cin >> pg;
			if (!correct_address(pg)) {
				cout << "输入数据不合法，重新输入" << endl;
				system("pause");
				system("cls");
				continue;
			}

			string page_id = pg.substr(0, 3);
			string offset = pg.substr(3, 5);

			int page = (page_id[0] - '0') * 4 + (page_id[1] - '0') * 2 + (page_id[2] - '0');
			ran(p, page, ram, count);
			show_process(p);
			if (page < num && page >= 0) {   //未越界
				cout << "本次访问物理地址为：" << address_convert(p, pg, ram);
			}
			cout << "缺页总次数为：" << count << endl;
		}
		  break;

	default:cout << "输入错误" << endl;
		system("pause");
		system("cls");
		goto  lable1;
	}

	

	//cout << "10110000的物理地址为：" << address_convert(p, "10110000", ram) << endl;

	return 0;
}



