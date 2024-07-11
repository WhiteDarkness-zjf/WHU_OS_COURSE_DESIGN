#include <iostream>
#include<string>
#include <vector>
#include <Windows.h>


using namespace std;

#define BLOCK_SIZE 4096     //一个磁盘块固定大小  单位：字节

//磁盘块结构体定义
struct Block
{
	int id;         //磁盘块号
	int size;       //块大小
	int next_id;    //存放同一文件的下一块号  -1表示此块是最后一块
	int flag;       //标志位，表示是否分配
	int capacity;   //容量 表示占用多少
	int file_id;    //所属文件的id号
	string data;    //本磁盘块存放的数据

	//初始化
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

//磁盘空间结构体
struct Disk
{
	vector<Block>block;

	//初始化
	Disk(int count) {
		for (int i = 0; i < count; i++) {
			Block b(i);
			block.push_back(b);
		}
	}
};


//判断磁盘空间是否已满
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

//文件结构体
struct File
{
	int id;               //文件id  从0开始编号
	string name;          //文件名
	int size;          //文件大小
	int in_which_file;    //表示所在目录号   0为根目录  只有根目录这个属性为-1
	int type;             //文件类型  0:目录文件  1:普通文件   2:可执行文件
	int status;           //文件状态  0：关闭   >0：打开  绝对值表示打开该文件的数目
	int read_power;       //读权限
	int write_power;      //写权限
	int exe_power;        //执行权限
	vector<int>block;     //文件占用的块数组
	vector<int>file_list; //该目录下的所有文件id

	//初始化
	File() {

	}

	File(int id, string name, int in_which_file, int type,Disk &disk) {


		this->id = id;
		this->name = name;
		this->in_which_file = in_which_file;
		this->type = type;
		this->status = 0;
		this->size = 4;              //初始化时先分配4个字节

		if (type != 2) {
			this->read_power = 1;
			this->write_power = 1;
			this->exe_power = 0;	
		}

		else {     //可执行文件
			this->read_power = 0;
			this->write_power = 0;
			this->exe_power = 1;
			
		}
		

		//分配一个磁盘块
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

//文件控制块结构体定义
struct FCB
{
	vector<File>all_file;
	FCB() {

	}
};



//根据id号查找文件在FCB中的索引位置
int find_index(int id, FCB& fcb) {
	int index = -1;   //用于存储当前页在FCB中的索引号
	for (int i = 0; i < fcb.all_file.size(); i++) {
		if (fcb.all_file[i].id == id) {
			index = i;
			break;
		}
	}
	return index;
}

//根据文件id查找文件名
string  id_to_name(FCB& fcb, int id) {
	for (int i = 0; i < fcb.all_file.size(); i++) {
		if (id == fcb.all_file[i].id) {
			return fcb.all_file[i].name;
		}
	}
	return "";
}

//根据文件名在指定目录查找文件id
int name_to_id(FCB& fcb, string name, int curr_page) {
	int index = find_index(curr_page, fcb);
	for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
		if (name == id_to_name(fcb, fcb.all_file[index].file_list[i])) {
			return fcb.all_file[index].file_list[i];
		}
	}
	return -1;
}

//在当前目录查找是否有重名文件
bool is_same_name(int curr_page, string name, FCB& fcb) {
	int index;   //用于存储当前页在FCB中的索引号
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

//查找空闲块数目
int empty_block(Disk& disk) {
	int res = 0;
	for (int i = 0; i < disk.block.size(); i++) {
		if (disk.block[i].flag == 0) {
			res++;
		}
	}
	return res;
}

//给文件分配一个磁盘块
void allocate_block(int id, FCB& fcb, Disk& disk) {
	if (1 > empty_block(disk)) {
		cout << "磁盘空间不足，分配失败！！！" << endl;
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


//创建文件
void create_file(FCB& fcb, Disk& disk, int& curr_page,int &index) {
	if (is_disk_full(disk)) {   //磁盘块分配完毕
		cout << "磁盘空间已满，创建失败！！！" << endl;
		return;
   }


	string name;
	cout << "输入你要创建的文件名" << endl;
	cin >> name;
	if (is_same_name(curr_page, name,fcb)) {    //文件重名
		cout << "文件名重复，创建失败！！！" << endl;
		return;
	}

	int temp_index = find_index(curr_page, fcb);    //当前目录在FCB中的索引

	if (fcb.all_file[temp_index].write_power == 0) {
		cout << "你没有对该目录的写权限，无法创建文件！！！" << endl;
		return;
	}

	if (fcb.all_file[temp_index].size % 1024==0) {
		//需要给目录文件增加磁盘块
		if (empty_block(disk) <= 1) {
			cout << "磁盘空间不足，创建失败！！！" << endl;
			return;
		}
	}

	//在当前目录创建文件
	cout << "输入你要创建的文件类型  0:目录文件  1:普通文件  2:可执行文件" << endl;
	int type;
	cin >> type;
	if (type != 0 && type != 1&&type!=2) {
		cout << "类型输入不合法，创建失败！！！" << endl;
		return;
	}
	File f(index, name, curr_page, type, disk);

	//修改目录信息和FCB信息
	fcb.all_file.push_back(f);

	
	fcb.all_file[temp_index].size += 4;
	fcb.all_file[temp_index].file_list.push_back(index);

	if (fcb.all_file[temp_index].size % 1024 == 0) {
		//需要给目录文件增加磁盘块
		allocate_block(curr_page, fcb, disk);
	}	

	index++;
	cout << "创建成功！" << endl;

}

//打开文件
void open_file(FCB& fcb, int& curr_page) {

	cout << "输入你想打开的文件名" << endl;
	string name;
	cin >> name;
	//当前目录查看有无该文件
	if (!is_same_name(curr_page, name, fcb)) {   //没有该文件
		cout << "当前目录不存在该文件，打开失败" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);
	if (fcb.all_file[index].type == 0) {    //目录文件  无法打开
		cout << "此文件是目录文件，打开失败！！！" << endl;
		return;
	}

	if (fcb.all_file[index].type == 2) {    //可执行文件  无法打开
		cout << "此文件是可执行文件，打开失败！！！" << endl;
		return;
	}

	fcb.all_file[index].status++;

}

//关闭文件
void close_file(FCB& fcb, int& curr_page) {

	cout << "输入你想关闭的文件名" << endl;
	string name;
	cin >> name;

	//当前目录查看有无该文件
	if (!is_same_name(curr_page, name, fcb)) {   //没有该文件
		cout << "当前目录不存在该文件，关闭失败" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);
	if (fcb.all_file[index].type == 0) {    //目录文件  无法关闭
		cout << "此文件是目录文件，关闭失败！！！" << endl;
		return;
	}

	if (fcb.all_file[index].status == 0) {   //文件本来就处于关闭状态
		cout << "文件处于关闭状态，无需关闭" << endl;
		return;
	}

	fcb.all_file[index].status--;

}


//列出当前目录下的所有文件
void list_all_file(FCB& fcb, int& curr_page) {
	
	int index = find_index(curr_page, fcb);

	if (fcb.all_file[index].read_power == 0) {
		cout << "你没有此目录的读权限！！！" << endl;
		return;
	}

	if (fcb.all_file[index].file_list.size() == 0) {  //目录为空
		cout << "当前目录为空" << endl;
		return;
	}

	cout << "当前目录文件如下：" << endl;
	for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
		int temp_index = find_index(fcb.all_file[index].file_list[i],fcb);
		cout << fcb.all_file[temp_index].name << "  ";
		if (fcb.all_file[temp_index].type == 0) {
			cout << "目录文件" << endl;
		}
		else if(fcb.all_file[temp_index].type == 1){
			cout << "普通文件" << endl;
		}

		else {
			cout << "可执行文件" << endl;
		}
	}
}

//根据当前目录查找文件路径
string find_path(FCB& fcb, int curr_page) {
	vector<int>path;
	int index;
	while (curr_page != -1) {
		path.push_back(curr_page);
		index = find_index(curr_page, fcb);
		curr_page = fcb.all_file[index].in_which_file;
	}

	string res = "";
	//倒序输出路径
	for (int i = path.size() - 1; i >= 0; i--) {
		index = find_index(path[i], fcb);
		res = res + "/" + fcb.all_file[index].name;
	}
	return res;
}


//返回上一级目录
void back_last_catalog(int& curr_page,FCB&fcb) {
	int index = find_index(curr_page, fcb);
	int last_catalog = fcb.all_file[index].in_which_file;
	if (last_catalog == -1) {   //此时已经在根目录
		cout << "此时已经在根目录！！！" << endl;
		return;
	}

	curr_page = last_catalog;

}

//进入目录
void enter_catalog(int& curr_page, FCB& fcb) {
	cout << "选择你要进入的目录" << endl;
	string catalog;
	cin >> catalog;


	if (!is_same_name(curr_page, catalog, fcb)) {
		cout << "输入的目录不存在当前目录下" << endl;
		return;
	}
	
	
	int id = name_to_id(fcb, catalog, curr_page);
	int index = find_index(id, fcb);

	if (fcb.all_file[index].type != 0) {   //不是目录文件
		cout << "输入的文件不是目录文件，进入失败！！！" << endl;
		return;
	}

	if (fcb.all_file[index].read_power == 0) {
		cout << "你没有此目录的读权限！！！" << endl;
		return;
	}
	
	curr_page = fcb.all_file[index].id;

}

//读文件
void read_file(FCB& fcb, int curr_page,Disk&disk) {
	cout << "选择你要读取的文件" << endl;
	string name;
	cin >> name;

	//当前目录查看有无该文件
	if (!is_same_name(curr_page, name, fcb)) {   //没有该文件
		cout << "当前目录不存在该文件，读取失败" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);
	if (fcb.all_file[index].type == 0) {    //目录文件  无法读取
		cout << "此文件是目录文件，读取失败！！！" << endl;
		return;
	}
	 
	if (fcb.all_file[index].type == 2) {       //可执行文件  无法读取
		cout << "此文件是可执行文件，读取失败！！！" << endl;
		return;
	}
	
	//查看文件是否已经打开
	if (fcb.all_file[index].status == 0) {   //文件未打开
		cout << "文件处于关闭状态，读取失败" << endl;
		return;
	}

	//查看是否有读权限
	if (fcb.all_file[index].read_power == 0) {   //没有读权限
		cout << "你没有权限读取此文件" << endl;
		return;
	}

	cout << "文件内容如下：" << endl;
	string res = "";
	for (int i = 0; i < fcb.all_file[index].block.size(); i++) {
		res += disk.block[fcb.all_file[index].block[i]].data;
	}

	if (res == "") {
		cout << "文件为空" << endl;
		return;
	}
	cout << res << endl;

}

//求一个文件最后一个磁盘块剩余的字节数
int get_remaining_byte(int id, FCB& fcb, Disk& disk) {
	int index = find_index(id, fcb);
	int remaining;
	int count = fcb.all_file[index].block.size();
	remaining = 1024 - disk.block[fcb.all_file[index].block[count - 1]].capacity;
	return remaining;
}


//写文件
void write_file(FCB& fcb, int curr_page, Disk& disk) {
	cout << "选择你要写入的文件" << endl;
	string name;
	cin >> name;

	//当前目录查看有无该文件
	if (!is_same_name(curr_page, name, fcb)) {   //没有该文件
		cout << "当前目录不存在该文件，写入失败" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);
	if (fcb.all_file[index].type == 0) {    //目录文件  无法写入
		cout << "此文件是目录文件，写入失败！！！" << endl;
		return;
	}

	if (fcb.all_file[index].type == 2) {    //可执行文件  无法写入
		cout << "此文件是可执行文件，写入失败！！！" << endl;
		return;
	}

	//查看文件是否已经打开
	if (fcb.all_file[index].status == 0) {   //文件未打开
		cout << "文件处于关闭状态，写入失败" << endl;
		return;
	}

	//查看是否有写权限
	if (fcb.all_file[index].write_power == 0) {   //没有写权限
		cout << "你没有权限写入此文件" << endl;
		return;
	}

	//开始写文件
	cout << "请输入你要写入的内容" << endl;
	string write_in = "";
	cin >> write_in;

	int write_size = write_in.size();   //写入内容的大小  单位：字节

	//判断空间是否足够
	int remaining_block = empty_block(disk);       //剩余空闲磁盘块
	int remaining_byte = get_remaining_byte(id, fcb, disk);    //写入文件最后一块磁盘剩余的字节数
	if (write_size > 1024 * remaining_block + remaining_byte) {    //空间不足
		cout << "磁盘空间不足，写入失败！！！" << endl;
		return;
	}

	int last_block_index = fcb.all_file[index].block.size() - 1;    //文件最后一个磁盘块索引

	if (write_size <= remaining_byte) {    //最后一个磁盘块可以放下写入的数据
		fcb.all_file[index].size += write_size;     //修改文件大小

		//修改磁盘块内容
		disk.block[fcb.all_file[index].block[last_block_index]].capacity += write_size;
		disk.block[fcb.all_file[index].block[last_block_index]].data += write_in;
		cout << "成功写入文件！！！" << endl;
		return;
	}

	//需要增加磁盘块

	fcb.all_file[index].size += write_size;     //修改文件大小
	int add_count = (write_size - remaining_byte) / 1024;    //需要增加的个数
	if ((write_size - remaining_byte) % 1024 != 0) {    //不能正好分配，需要加一
		add_count++;
	}

	int ptr_index = 0;     //记录当前写入数据所在位置
	string temp_str = write_in.substr(ptr_index, remaining_byte);
	disk.block[fcb.all_file[index].block[last_block_index]].capacity = 1024;
	disk.block[fcb.all_file[index].block[last_block_index]].data += temp_str;

	ptr_index += remaining_byte;      //指针前移

	for (int i = 0; i < add_count; i++) {

		//找到第一个空闲块号
		int first_block;
		for (int i = 0; i < disk.block.size(); i++) {
			if (disk.block[i].flag == 0) {
				first_block = i;
				break;
			}
		}

		if (i != add_count - 1) {    //不是文件最后一个块
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

		else {        //是最后一个块
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

	cout << "成功写入文件！！！" << endl;

}


//执行文件
void exe_file(FCB& fcb, int curr_page) {
	cout << "选择你要执行的文件" << endl;
	string name;
	cin >> name;

	//当前目录查看有无该文件
	if (!is_same_name(curr_page, name, fcb)) {   //没有该文件
		cout << "当前目录不存在该文件，执行失败" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);


	if (fcb.all_file[index].type != 2) {    //不是可执行文件  无法执行
		cout << "此文件不是可执行文件，执行失败！！！" << endl;
		return;
	}

	//查看是否有执行权限
	if (fcb.all_file[index].exe_power == 0) {   //没有执行权限
		cout << "你没有权限执行此文件" << endl;
		return;
	}

	int time = rand() % 5 + 1;
	for (int i = 0; i < time; i++) {
		Sleep(1000);
		cout << "执行中......" << endl;
	}

	cout << "执行完毕！" << endl;

}

//删除文件或目录
void delete_file(FCB& fcb, int curr_page, Disk& disk,string name) {
	

	//当前目录查看有无该文件
	if (!is_same_name(curr_page, name, fcb)) {   //没有该文件
		cout << "当前目录不存在该文件，删除失败" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);

	if (fcb.all_file[index].status > 0) {    //文件处于打开状态，无法删除
		cout << "文件处于打开状态，无法删除" << endl;
		return;
	}



	if (fcb.all_file[index].type == 1) {    //普通文件  直接删除

		//释放磁盘块
		for (int i = 0; i < fcb.all_file[index].block.size(); i++) {
			disk.block[fcb.all_file[index].block[i]].capacity = 0;
			disk.block[fcb.all_file[index].block[i]].data = "";
			disk.block[fcb.all_file[index].block[i]].file_id = -1;
			disk.block[fcb.all_file[index].block[i]].flag = 0;
			disk.block[fcb.all_file[index].block[i]].next_id = -1;
		}


		int catalog = fcb.all_file[index].in_which_file;
		int catalog_index = find_index(catalog, fcb);

		//删除表中的相关文件信息
		swap(fcb.all_file[index], fcb.all_file[fcb.all_file.size() - 1]);
		fcb.all_file.pop_back();



		//删除相关目录信息
		for (int i = 0; i < fcb.all_file[catalog_index].file_list.size(); i++) {
			if (fcb.all_file[catalog_index].file_list[i] == id) {
				swap(fcb.all_file[catalog_index].file_list[i], fcb.all_file[catalog_index].file_list[fcb.all_file[catalog_index].file_list.size() - 1]);
				fcb.all_file[catalog_index].file_list.pop_back();
				fcb.all_file[catalog_index].size -= 4;
			}
		}

	}

	else {                                 //目录文件   递归删除       
		for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
			delete_file(fcb, id, disk, id_to_name(fcb,fcb.all_file[index].file_list[i]));
		}

		//最后删除目录相关信息
		//释放磁盘块
		for (int i = 0; i < fcb.all_file[index].block.size(); i++) {
			disk.block[fcb.all_file[index].block[i]].capacity = 0;
			disk.block[fcb.all_file[index].block[i]].data = "";
			disk.block[fcb.all_file[index].block[i]].file_id = -1;
			disk.block[fcb.all_file[index].block[i]].flag = 0;
			disk.block[fcb.all_file[index].block[i]].next_id = -1;
		}



		int catalog = fcb.all_file[index].in_which_file;
		int catalog_index = find_index(catalog, fcb);

		//删除表中的相关文件信息
		swap(fcb.all_file[index], fcb.all_file[fcb.all_file.size() - 1]);
		fcb.all_file.pop_back();


		//删除相关目录信息
		for (int i = 0; i < fcb.all_file[catalog_index].file_list.size(); i++) {
			if (fcb.all_file[catalog_index].file_list[i] == id) {
				swap(fcb.all_file[catalog_index].file_list[i], fcb.all_file[catalog_index].file_list[fcb.all_file[catalog_index].file_list.size() - 1]);
				fcb.all_file[catalog_index].file_list.pop_back();
				fcb.all_file[catalog_index].size -= 4;
			}
		}
	}

}

//登录管理员权限
int login_admin(string password) {
	cout << "请输入密码" << endl;
	string pd;
	cin >> pd;
	if (pd != password) {   //密码错误
		cout << "密码输入错误！！！" << endl;
		return 0;
	}

	return 1;

}


//管理员界面
void show_admin_win() {
	cout << " -----------------------------------------------" << endl;
	cout << "|            1、 返回上一级目录                 |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            2、 列出当前目录下所有文件         |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            3、 修改文件权限                   |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            4、 查看磁盘空间使用情况           |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            5、 进入目录                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            6、 回到用户界面                   |" << endl;
	cout << "|                                               |" << endl;
	cout << " -----------------------------------------------" << endl;
}

//进入目录
void enter_catalog_admin(int& curr_page, FCB& fcb) {
	cout << "选择你要进入的目录" << endl;
	string catalog;
	cin >> catalog;

	if (!is_same_name(curr_page, catalog, fcb)) {
		cout << "输入的目录不存在当前目录下" << endl;
		return;
	}


	int id = name_to_id(fcb, catalog, curr_page);
	int index = find_index(id, fcb);

	if (fcb.all_file[index].type != 0) {   //不是目录文件
		cout << "输入的文件不是目录文件，进入失败！！！" << endl;
		return;
	}

	curr_page = fcb.all_file[index].id;

}

//修改文件权限
void modify_power(FCB& fcb, int curr_page, Disk& disk) {
	cout << "选择你要修改的文件" << endl;
	string name;
	cin >> name;

	//当前目录查看有无该文件
	if (!is_same_name(curr_page, name, fcb)) {   //没有该文件
		cout << "当前目录不存在该文件，修改失败" << endl;
		return;
	}

	int id = name_to_id(fcb, name, curr_page);
	int index = find_index(id, fcb);

	cout << "请输入修改后的权限：1表示有此权限，0表示没有    请输入三位二进制数    顺序依次为读、写、执行" << endl;
	string power;
	cin >> power;
	if (power == "000" || power == "001" || power == "010" || power == "011" || power == "100" || power == "101" || power == "110" || power == "111") {
		fcb.all_file[index].read_power = power[0] -= '0';
		fcb.all_file[index].write_power = power[1] -= '0';
		fcb.all_file[index].exe_power = power[2] -= '0';
	}

	else {
		cout << "输入数据不合法！！！" << endl;
	}

}


//查看磁盘空间使用情况
void show_disk_use(Disk& disk, FCB& fcb) {
	
	cout << "磁盘块号\t" << "是否被分配\t" << "磁盘使用空间\t" << "所属文件id\t" << "存放同一文件的下一磁盘快号\t" << endl;

	for (int i = 0; i < disk.block.size(); i++) {
		cout << disk.block[i].id << "\t\t" << disk.block[i].flag << "\t\t" << disk.block[i].capacity << "\t\t" << disk.block[i].file_id << "\t\t" << disk.block[i].next_id << endl;
		}

	cout << endl;
}


//列目录
void list_all_file_admin(FCB& fcb, int& curr_page) {

	int index = find_index(curr_page, fcb);


	if (fcb.all_file[index].file_list.size() == 0) {  //目录为空
		cout << "当前目录为空" << endl;
		return;
	}

	cout << "当前目录文件如下：" << endl;
	for (int i = 0; i < fcb.all_file[index].file_list.size(); i++) {
		int temp_index = find_index(fcb.all_file[index].file_list[i], fcb);
		cout << fcb.all_file[temp_index].name << "  ";
		if (fcb.all_file[temp_index].type == 0) {
			cout << "目录文件" << endl;
		}
		else {
			cout << "普通文件" << endl;
		}
	}
}



//主界面函数
void show_main() {
	cout << " -----------------------------------------------" << endl;
	cout << "|            1、 返回上一级目录                 |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            2、 列出当前目录下所有文件         |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            3、 创建文件                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            4、 打开文件                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            5、 关闭文件                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            6、 读取文件                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            7、 写入文件                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            8、 执行文件                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            9、 删除文件或目录                 |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            10、 进入目录                       |" << endl;
	cout << "|                                               |" << endl;
	cout << "|            11、 登录管理员权限                |" << endl;
	cout << "|                                               |" << endl;
	cout << " -----------------------------------------------" << endl;
}





int main() {

	Disk disk(100);

	int curr_page = 0;   //当前目录号   0表示根目录

	int index = 1;       //后续创建的文件id号  从1开始

	FCB fcb;

	File root(0, "root", -1, 0, disk);

	fcb.all_file.push_back(root);

	string password = "123456";
	
	lable1: while (1) {
		cout << "当前所在目录为：" << find_path(fcb, curr_page) << endl;
		show_main();
		cout << "决定你要执行的操作：";
		string str;
		int select;
		cin >> str;
		if (str == "1" || str == "2" || str == "3" || str == "4" || str == "5" || str == "6" || str == "7" || str == "8" || str == "9" || str == "10"||str=="11") {
			select = stoi(str);
		}
		else {
			cout << "输入数据不合法，请重新输入！！！" << endl;
			system("pause");
			system("cls");
			continue;
		}

		string ff;     //表示删除的文件或目录
		int res;       //表示登录密码正确与否

		switch (select) {
		case 1:     //返回上一级目录
				back_last_catalog(curr_page, fcb);
				system("pause");
				system("cls");
		    	break;
		    
		case 2:     //列出当前目录下所有文件
				list_all_file(fcb, curr_page);
				system("pause");
				system("cls");
		    	break;
		    
		case 3:     //创建文件
				create_file(fcb, disk, curr_page, index);
				system("pause");
				system("cls");
		    	break;

		case 4:      //打开文件
				open_file(fcb, curr_page);
				system("pause");
				system("cls");
				break;

		case 5:      //关闭文件
				close_file(fcb, curr_page);
				system("pause");
				system("cls");
				break;
		    
		case 6:     //读取文件
				read_file(fcb, curr_page, disk);
				system("pause");
				system("cls");
		    	break;
		    
		case 7:     //写入文件
				write_file(fcb, curr_page, disk);
				system("pause");
				system("cls");
		    	break;

		case 8:     //执行文件
			exe_file(fcb, curr_page);
			system("pause");
			system("cls");
			break;
		    
		case 9:     //删除文件或目录
				cout << "请输入你要删除的文件或目录" << endl;
				cin >> ff;
				delete_file(fcb, curr_page, disk,ff);
				system("pause");
				system("cls");
		    	break;
		    
		case 10:     //进入目录
				enter_catalog(curr_page, fcb);
				system("pause");
				system("cls");
		    	break;

		case 11:     //登录管理员权限
			res = login_admin(password);
			if (res == 1) {
				goto lable2;
			}

			system("pause");
			system("cls");
			break;
		    
		    default:    //输入错误
				cout << "输入数据不合法，请重新输入！！！" << endl;
				system("pause");
				system("cls");
		    	break;

		}
	}



	//管理员模式
lable2:
	system("cls");
	while (1) {
		cout << "当前所在目录为：" << find_path(fcb, curr_page) << endl;
		show_admin_win();
		cout << "决定你要执行的操作：";
		string str;
		int select;
		cin >> str;
		if (str == "1" || str == "2" || str == "3" || str == "4" || str == "5" || str == "6" ) {
			select = stoi(str);
		}
		else {
			cout << "输入数据不合法，请重新输入！！！" << endl;
			system("pause");
			system("cls");
			continue;
		}


		switch (select) {
		case 1:     //返回上一级目录
			back_last_catalog(curr_page, fcb);
			system("pause");
			system("cls");
			break;

		case 2:     //列出当前目录下所有文件
			list_all_file_admin(fcb, curr_page);
			system("pause");
			system("cls");
			break;

		case 3:     //修改文件权限
			modify_power(fcb, curr_page, disk);
			system("pause");
			system("cls");
			break;

		case 4:      //查看磁盘空间使用情况
			show_disk_use(disk, fcb);
			system("pause");
			system("cls");
			break;

		case 5:      //进入目录
			enter_catalog_admin(curr_page, fcb);
			system("pause");
			system("cls");
			break;

		case 6:     //回到用户界面
			system("cls");
			goto lable1;
			break;


		default:    //输入错误
			cout << "输入数据不合法，请重新输入！！！" << endl;
			system("pause");
			system("cls");
			break;

		}
	}

	return 0;
}