#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include <random>

using namespace std;


//PCB结构体的定义
struct PCB
{
	int id;           //进程id号，唯一标识
	int reach_time;   //到达时间
	int cpu_time;     //运行时间
	int io_time;      //IO时间
	int need_time;    //剩余时间
	int have_cpu;     //是否得到过响应
	int first_time;   //第一次响应时间
	int finish_time;  //完成时间
	int status;       //进程状态  -1:阻塞态  0:就绪态  1:运行态  2:完成态
	int priority;     //优先级

	//初始化

	PCB() {

	}

	PCB(int id,int reach_time,int cpu_time,int io_time,int priority) {
		this->id = id;
		this->reach_time = reach_time;
		this->cpu_time = cpu_time;
		this->io_time = io_time;
		this->priority = priority;
		this->need_time = cpu_time;
		this->finish_time = 0;
		this->first_time = 0;
		this->have_cpu = 0;
		this->status = -1;
	}

	

};



void create_process(int& id, vector<PCB> &process) {

	//srand((unsigned int)time(0));
	
		int reach_time = process[id - 1].reach_time + rand() % 5;   //随机生成到达时间,但是不能比前一个进程早
		int cpu_time = rand() % 5 + 1;    //运行时间
		int io_time = rand() % 5 + 1;     //io时间
		int priority = rand() % 5 + 1;    //优先级，越大则优先级越高
		PCB p(id, reach_time, cpu_time, io_time, priority);
		process.push_back(p);
		//id++;         //进程序号自增
	
}
	
//查看所有进程情况
void show_process(vector<PCB>process) {
	cout << "id\t" << " " << "reach_time\t" << " " << "io_time\t" << " " << "cpu_time\t" << " " << "need_time\t" << " "<<"first_time\t"<<" " << "finish_time\t" << "status\t" << endl;
	for (int i = 1; i < process.size(); i++) {
		cout << process[i].id << "\t\t" << process[i].reach_time << "\t\t" << process[i].io_time << "\t\t" << process[i].cpu_time << "\t\t" << process[i].need_time << "\t" << process[i] .first_time<<"\t\t" << process[i].finish_time << "\t\t" << process[i].status << endl;
	}
}

//检查进程是否都已经完成
bool is_finish(vector<PCB>&process) {
	bool flag = 1;
	for (int i = 1; i <= process.size() - 1; i++) {
		if (process[i].status != 2) {
			flag = 0;
			break;
		}
	}
	return flag;
}


//检查进程是否已经在队列中
bool is_in_queue(queue<PCB> q, PCB p) {
	int flag = 0;
	while (!q.empty()) {
		PCB temp = q.front();
		q.pop();
		if (temp.id == p.id) {
			flag = 1;
			break;
		}
	}
	return flag;
}


//时间片轮转算法
void rr(vector<PCB>& process,int time_interval) {
	int curr_time = 0;
	queue<PCB>q;     //就绪队列

	//初始化就绪队列
	for (int i = 1; i <= process.size() - 1; i++) {
		if (process[i].status == 0) {
			q.push(process[i]);
		}
	}

	//未完成时循环
	while (!is_finish(process)) {
		//就绪队列为空
		if (q.empty()) {
			curr_time++;
			//刷新就绪队列
			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status != 2 && (curr_time - process[i].reach_time) >= process[i].io_time) {
					process[i].status = 0;
				}
			}

			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status == 0) {
					q.push(process[i]);
				}
			}
			continue;
		}

		int used_time = 0; //当前进程在本时间片已经运行时间
		PCB p = q.front(); //挑取队列第一个进程分配cpu
		q.pop();

		//设置第一次响应时间
		if (p.have_cpu == 0) {
			p.have_cpu = 1;
			process[p.id].have_cpu = 1;
			process[p.id].first_time = curr_time;
		}

		process[p.id].status = 1;
		while(used_time<time_interval)
		{
			
			p.need_time = p.need_time - 1;
			process[p.id].need_time--;
			used_time++;    //时间片加一
			curr_time++;    //当前时间加一
			if (p.need_time == 0) {  //运行完成
				used_time = time_interval;  //设置运行时间为时间片大小，这样就可以停止下一次循环

				//更新此进程状态
				process[p.id].finish_time = curr_time;
				process[p.id].status = 2;
				
			}

			

			//刷新就绪队列
			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status != 2 && process[i].status != 1 && (curr_time - process[i].reach_time) >= process[i].io_time) {
					process[i].status = 0;
				}
			}

			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status == 0&&!is_in_queue(q,process[i])) {
					q.push(process[i]);
				}
			}
			
			if (p.need_time != 0&&used_time == time_interval) {  //时间片到但未完成
				process[p.id].status = 0;
				q.push(p);   //插入就绪队列队尾
			}

			//跟踪进程状态
			cout << "当前时间为：" << curr_time << "  进程状态如下" << endl;
			show_process(process);
			cout << endl;

		}

	}
}



//重载运算符
struct cmp {
	bool operator()(const PCB& p1, const PCB& p2) {
		return p1.cpu_time > p2.cpu_time;
	}
};

//检查进程是否已经在优先队列中
bool is_in_queue2(priority_queue<PCB, vector<PCB>, cmp> q, PCB p) {
	int flag = 0;
	while (!q.empty()) {
		PCB temp = q.top();
		q.pop();
		if (temp.id == p.id) {
			flag = 1;
			break;
		}
	}
	return flag;
}

//最短作业优先算法
void sjf(vector<PCB>& process) {
	int curr_time = 0;

	priority_queue<PCB,vector<PCB>,cmp>q;   //优先队列
	//初始化优先队列
	for (int i = 1; i <= process.size() - 1; i++) {
		if (process[i].status == 0) {
			q.push(process[i]);
		}
	}

	//未完成时循环
	while (!is_finish(process)) {
		//就绪队列为空
		if (q.empty()) {
			curr_time++;
			//刷新就绪队列
			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status != 2&& (curr_time - process[i].reach_time) >= process[i].io_time) {
					process[i].status = 0;
				}
			}

			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status == 0) {
					q.push(process[i]);
				}
			}
			continue;
		}

		
		PCB p = q.top(); //挑取队列第一个进程分配cpu
		q.pop();
		process[p.id].status = 1;
		
		//设置第一次响应时间
		if (p.have_cpu == 0) {
			p.have_cpu = 1;
			process[p.id].have_cpu = 1;
			process[p.id].first_time = curr_time;
		}

		    p.need_time = 0;
			process[p.id].need_time = 0;
			curr_time += p.cpu_time;    //当前时间增加

			//运行完成
			//更新此进程状态
			process[p.id].finish_time = curr_time;
			process[p.id].status = 2;

			



			//刷新就绪队列
			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status != 2 && process[i].status != 1 && (curr_time - process[i].reach_time) >= process[i].io_time) {
					process[i].status = 0;
				}
			}

			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status == 0&&!is_in_queue2(q,process[i])) {
					q.push(process[i]);
				}
			}

			//跟踪进程状态
			cout << "当前时间为：" << curr_time << "  进程状态如下" << endl;
			show_process(process);
			cout << endl;

	}
	
}

//多级反馈队列算法   队列时间片分别为1，2，4
void mlf(vector<PCB>& process) {
	int curr_time = 0;
	queue<PCB>q1, q2, q3;

	//初始化第一级队列
	for (int i = 1; i <= process.size() - 1; i++) {
		if (process[i].status == 0) {
			q1.push(process[i]);
		}
	}

	//未完成时循环
	while (!is_finish(process)) {

		//就绪队列全部为空
		if (q1.empty()&& q2.empty()&& q3.empty()) {
			curr_time++;
			//刷新就绪队列
			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status != 2 && (curr_time - process[i].reach_time) >= process[i].io_time) {
					process[i].status = 0;
				}
			}

			for (int i = 1; i <= process.size() - 1; i++) {
				if (process[i].status == 0) {
					q1.push(process[i]);
				}
			}
			continue;
		}

		//第一级队列不为空
		lable1: while (!q1.empty())
		{
			int used_time = 0; //当前进程在本时间片已经运行时间
			PCB p = q1.front(); //挑取队列第一个进程分配cpu
			q1.pop();
			process[p.id].status = 1;
			//设置第一次响应时间
			if (p.have_cpu == 0) {
				p.have_cpu = 1;
				process[p.id].have_cpu = 1;
				process[p.id].first_time = curr_time;
			}
			while (used_time < 1)
			{

				p.need_time = p.need_time - 1;
				process[p.id].need_time--;
				used_time++;    //时间片加一
				curr_time++;    //当前时间加一
				if (p.need_time == 0) {  //运行完成
					used_time = 1;  //设置运行时间为时间片大小，这样就可以停止下一次循环

					//更新此进程状态
					process[p.id].finish_time = curr_time;
					process[p.id].status = 2;

				}

				//刷新就绪队列
				for (int i = 1; i <= process.size() - 1; i++) {
					if (process[i].status != 2 && process[i].status != 1 && (curr_time - process[i].reach_time) >= process[i].io_time) {
						process[i].status = 0;
					}
				}

				for (int i = 1; i <= process.size() - 1; i++) {
					if (process[i].status == 0 && !is_in_queue(q1, process[i]) && !is_in_queue(q2, process[i]) && !is_in_queue(q3, process[i])) {
						q1.push(process[i]);
					}
				}

				if (p.need_time != 0 && used_time == 1) {  //时间片到但未完成
					process[p.id].status = 0;
					q2.push(p);   //插入第二级就绪队列队尾
				}

				//跟踪进程状态
				cout << "当前时间为：" << curr_time << "  进程状态如下" << endl;
				show_process(process);
				cout << endl;

			}
		}

		//第一级队列为空但第二级不为空
		lable2:while (q1.empty() && !q2.empty()) {
			int used_time = 0; //当前进程在本时间片已经运行时间
			PCB p = q2.front(); //挑取队列第一个进程分配cpu
			q2.pop();
			if(process[p.id].status!=2)
			{
				process[p.id].status = 1;
				while (used_time < 2)
				{

					p.need_time = p.need_time - 1;
					process[p.id].need_time--;
					used_time++;    //时间片加一
					curr_time++;    //当前时间加一
					if (p.need_time == 0) {  //运行完成
						used_time = 2;  //设置运行时间为时间片大小，这样就可以停止下一次循环

						//更新此进程状态
						process[p.id].finish_time = curr_time;
						process[p.id].status = 2;

					}

					//刷新就绪队列
					for (int i = 1; i <= process.size() - 1; i++) {
						if (process[i].status != 2 && process[i].status != 1 && (curr_time - process[i].reach_time) >= process[i].io_time) {
							process[i].status = 0;
						}
					}

					for (int i = 1; i <= process.size() - 1; i++) {
						if (process[i].status == 0 && !is_in_queue(q1, process[i]) && !is_in_queue(q2, process[i]) && !is_in_queue(q3, process[i])) {
							q1.push(process[i]);
						}
					}

					if (p.need_time != 0 && (used_time == 2 || (p.cpu_time - p.need_time)==3)) {  //时间片到但未完成
						process[p.id].status = 0;
						q3.push(p);   //插入第三级就绪队列队尾
					}

					//跟踪进程状态
					cout << "当前时间为：" << curr_time << "  进程状态如下" << endl;
					show_process(process);
					cout << endl;

					//查看第一级队列是否为空
					if (!q1.empty()) {  //不为空
						if (!is_in_queue(q3, p))
						{
							q2.push(p);    //当前未完成进程放入本队列队尾
						}
						goto lable1;
					}

				}
			}
		}

		//第一、二级队列为空但第三级不为空
	while (q1.empty() && q2.empty()&&!q3.empty()) {
		int used_time = 0; //当前进程在本时间片已经运行时间
		PCB p = q3.front(); //挑取队列第一个进程分配cpu
		q3.pop();
		if(process[p.id].status != 2)
		{
			process[p.id].status = 1;
			while (used_time < 4)
			{

				p.need_time = p.need_time - 1;
				process[p.id].need_time--;
				used_time++;    //时间片加一
				curr_time++;    //当前时间加一
				if (p.need_time == 0) {  //运行完成
					used_time = 4;  //设置运行时间为时间片大小，这样就可以停止下一次循环

					//更新此进程状态
					process[p.id].finish_time = curr_time;
					process[p.id].status = 2;

				}

				//刷新就绪队列
				for (int i = 1; i <= process.size() - 1; i++) {
					if (process[i].status != 2 && process[i].status != 1 && (curr_time - process[i].reach_time) >= process[i].io_time) {
						process[i].status = 0;
					}
				}

				for (int i = 1; i <= process.size() - 1; i++) {
					if (process[i].status == 0 && !is_in_queue(q1, process[i]) && !is_in_queue(q2, process[i]) && !is_in_queue(q3, process[i])) {
						q1.push(process[i]);
					}
				}

				if (p.need_time != 0 && (used_time == 4 || (p.cpu_time - p.need_time-3)%4==0)) {  //时间片到但未完成
					process[p.id].status = 0;
					q3.push(p);   //插入第三级就绪队列队尾
				}

				//跟踪进程状态
				cout << "当前时间为：" << curr_time << "  进程状态如下" << endl;
				show_process(process);
				cout << endl;

				//查看第一级队列是否为空
				if (!q1.empty()) {  //为空
					if (!is_in_queue(q3, p))
					{
						q3.push(p);    //当前未完成进程放入本队列队尾
					}
					goto lable1;
				}

				//查看第二级队列是否为空
				if (!q2.empty()) {  //为空
					if (!is_in_queue(q3, p))
					{
						q3.push(p);    //当前未完成进程放入本队列队尾
					}
					goto lable2;
				}

			}
		}
	}

	}

}

//展示性能指标
void show_performance(vector<PCB>process) {

	int count = process.size() - 1;  //进程数量
	double avg_turn_time = 0;   //平均周转时间
	double avg_wait_time = 0;   //平均等待时间
	double avg_response_time = 0;   //平均响应时间
	double cpu_time = 0;            //cpu运行时间
	double cpu_use;                 //cpu利用率
	int finish_time = 0;            //所有进程结束时间

	for (int i = 1; i <= process.size() - 1; i++) {
		avg_turn_time += (double)(process[i].finish_time - process[i].reach_time - process[i].io_time);
		avg_wait_time += (double)(process[i].finish_time - process[i].reach_time - process[i].cpu_time - process[i].io_time);
		avg_response_time += (double)(process[i].first_time - process[i].reach_time - process[i].io_time);
		cpu_time += process[i].cpu_time;
		finish_time = max(finish_time, process[i].finish_time);
	}

	avg_turn_time /= count;
	avg_wait_time /= count;
	avg_response_time /= count;
	cpu_use = cpu_time / finish_time;

	cout << "平均周转时间：" << avg_turn_time << endl;
	cout << "平均等待时间：" << avg_wait_time << endl;
	cout << "平均响应时间：" << avg_response_time << endl;
	cout << "cpu利用率   ：" << cpu_use << endl;
}


int main() {


	while(1)
	{
		int id = 1;  //当前进程序号,从1开始

		//进程队列，用于维护进程
		vector<PCB>process;
		

		//添加一个无用进程，作为起始参照
		PCB p;
		p.reach_time = 0;
		process.push_back(p);
		

		//创建进程
		int number;
		cout << "选择创建进程数量" << endl;
		cin >> number;

		for (int i = 1; i <= number; i++) {
			create_process(id, process);
			id++;
		}

		vector<PCB>process1(process);  //rr   
		vector<PCB>process2(process);  //sjf
		vector<PCB>process3(process);  //mlf

		//展示进程初始状态
		cout << "时间为0时进程状态如下：" << endl;
		show_process(process);
		cout << endl;

		//rr算法
		cout << "rr算法：" << endl;
	rr(process1, 2);
	cout << "rr算法执行完毕状态以及相关性能指标" << endl;
	show_process(process1);
	show_performance(process1);
	cout << endl;

	//sjf算法验证
	cout << "sjf算法：" << endl;
	sjf(process2);
	cout << "sjf算法执行完毕状态以及相关性能指标" << endl;
	show_process(process2);
	show_performance(process2);
	cout << endl;

	//mlf算法验证
	cout << "mlf算法：" << endl;
     mlf(process3);
	 cout << "mlf算法执行完毕状态以及相关性能指标" << endl;
	show_process(process3);
	show_performance(process3);

	system("pause");
	system("cls");

	}

	

	//rr算法验证
	//rr(process, 2);
	//show_process(process);

	//sjf算法验证
	//sjf(process);
	//show_process(process);

	//mlf算法验证
	//mlf(process);
	//show_process(process);
	//show_performance(process);

	return 0;
}


