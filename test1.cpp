#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <ctime>
#include <random>

using namespace std;


//PCB�ṹ��Ķ���
struct PCB
{
	int id;           //����id�ţ�Ψһ��ʶ
	int reach_time;   //����ʱ��
	int cpu_time;     //����ʱ��
	int io_time;      //IOʱ��
	int need_time;    //ʣ��ʱ��
	int have_cpu;     //�Ƿ�õ�����Ӧ
	int first_time;   //��һ����Ӧʱ��
	int finish_time;  //���ʱ��
	int status;       //����״̬  -1:����̬  0:����̬  1:����̬  2:���̬
	int priority;     //���ȼ�

	//��ʼ��

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
	
		int reach_time = process[id - 1].reach_time + rand() % 5;   //������ɵ���ʱ��,���ǲ��ܱ�ǰһ��������
		int cpu_time = rand() % 5 + 1;    //����ʱ��
		int io_time = rand() % 5 + 1;     //ioʱ��
		int priority = rand() % 5 + 1;    //���ȼ���Խ�������ȼ�Խ��
		PCB p(id, reach_time, cpu_time, io_time, priority);
		process.push_back(p);
		//id++;         //�����������
	
}
	
//�鿴���н������
void show_process(vector<PCB>process) {
	cout << "id\t" << " " << "reach_time\t" << " " << "io_time\t" << " " << "cpu_time\t" << " " << "need_time\t" << " "<<"first_time\t"<<" " << "finish_time\t" << "status\t" << endl;
	for (int i = 1; i < process.size(); i++) {
		cout << process[i].id << "\t\t" << process[i].reach_time << "\t\t" << process[i].io_time << "\t\t" << process[i].cpu_time << "\t\t" << process[i].need_time << "\t" << process[i] .first_time<<"\t\t" << process[i].finish_time << "\t\t" << process[i].status << endl;
	}
}

//�������Ƿ��Ѿ����
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


//�������Ƿ��Ѿ��ڶ�����
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


//ʱ��Ƭ��ת�㷨
void rr(vector<PCB>& process,int time_interval) {
	int curr_time = 0;
	queue<PCB>q;     //��������

	//��ʼ����������
	for (int i = 1; i <= process.size() - 1; i++) {
		if (process[i].status == 0) {
			q.push(process[i]);
		}
	}

	//δ���ʱѭ��
	while (!is_finish(process)) {
		//��������Ϊ��
		if (q.empty()) {
			curr_time++;
			//ˢ�¾�������
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

		int used_time = 0; //��ǰ�����ڱ�ʱ��Ƭ�Ѿ�����ʱ��
		PCB p = q.front(); //��ȡ���е�һ�����̷���cpu
		q.pop();

		//���õ�һ����Ӧʱ��
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
			used_time++;    //ʱ��Ƭ��һ
			curr_time++;    //��ǰʱ���һ
			if (p.need_time == 0) {  //�������
				used_time = time_interval;  //��������ʱ��Ϊʱ��Ƭ��С�������Ϳ���ֹͣ��һ��ѭ��

				//���´˽���״̬
				process[p.id].finish_time = curr_time;
				process[p.id].status = 2;
				
			}

			

			//ˢ�¾�������
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
			
			if (p.need_time != 0&&used_time == time_interval) {  //ʱ��Ƭ����δ���
				process[p.id].status = 0;
				q.push(p);   //����������ж�β
			}

			//���ٽ���״̬
			cout << "��ǰʱ��Ϊ��" << curr_time << "  ����״̬����" << endl;
			show_process(process);
			cout << endl;

		}

	}
}



//���������
struct cmp {
	bool operator()(const PCB& p1, const PCB& p2) {
		return p1.cpu_time > p2.cpu_time;
	}
};

//�������Ƿ��Ѿ������ȶ�����
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

//�����ҵ�����㷨
void sjf(vector<PCB>& process) {
	int curr_time = 0;

	priority_queue<PCB,vector<PCB>,cmp>q;   //���ȶ���
	//��ʼ�����ȶ���
	for (int i = 1; i <= process.size() - 1; i++) {
		if (process[i].status == 0) {
			q.push(process[i]);
		}
	}

	//δ���ʱѭ��
	while (!is_finish(process)) {
		//��������Ϊ��
		if (q.empty()) {
			curr_time++;
			//ˢ�¾�������
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

		
		PCB p = q.top(); //��ȡ���е�һ�����̷���cpu
		q.pop();
		process[p.id].status = 1;
		
		//���õ�һ����Ӧʱ��
		if (p.have_cpu == 0) {
			p.have_cpu = 1;
			process[p.id].have_cpu = 1;
			process[p.id].first_time = curr_time;
		}

		    p.need_time = 0;
			process[p.id].need_time = 0;
			curr_time += p.cpu_time;    //��ǰʱ������

			//�������
			//���´˽���״̬
			process[p.id].finish_time = curr_time;
			process[p.id].status = 2;

			



			//ˢ�¾�������
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

			//���ٽ���״̬
			cout << "��ǰʱ��Ϊ��" << curr_time << "  ����״̬����" << endl;
			show_process(process);
			cout << endl;

	}
	
}

//�༶���������㷨   ����ʱ��Ƭ�ֱ�Ϊ1��2��4
void mlf(vector<PCB>& process) {
	int curr_time = 0;
	queue<PCB>q1, q2, q3;

	//��ʼ����һ������
	for (int i = 1; i <= process.size() - 1; i++) {
		if (process[i].status == 0) {
			q1.push(process[i]);
		}
	}

	//δ���ʱѭ��
	while (!is_finish(process)) {

		//��������ȫ��Ϊ��
		if (q1.empty()&& q2.empty()&& q3.empty()) {
			curr_time++;
			//ˢ�¾�������
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

		//��һ�����в�Ϊ��
		lable1: while (!q1.empty())
		{
			int used_time = 0; //��ǰ�����ڱ�ʱ��Ƭ�Ѿ�����ʱ��
			PCB p = q1.front(); //��ȡ���е�һ�����̷���cpu
			q1.pop();
			process[p.id].status = 1;
			//���õ�һ����Ӧʱ��
			if (p.have_cpu == 0) {
				p.have_cpu = 1;
				process[p.id].have_cpu = 1;
				process[p.id].first_time = curr_time;
			}
			while (used_time < 1)
			{

				p.need_time = p.need_time - 1;
				process[p.id].need_time--;
				used_time++;    //ʱ��Ƭ��һ
				curr_time++;    //��ǰʱ���һ
				if (p.need_time == 0) {  //�������
					used_time = 1;  //��������ʱ��Ϊʱ��Ƭ��С�������Ϳ���ֹͣ��һ��ѭ��

					//���´˽���״̬
					process[p.id].finish_time = curr_time;
					process[p.id].status = 2;

				}

				//ˢ�¾�������
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

				if (p.need_time != 0 && used_time == 1) {  //ʱ��Ƭ����δ���
					process[p.id].status = 0;
					q2.push(p);   //����ڶ����������ж�β
				}

				//���ٽ���״̬
				cout << "��ǰʱ��Ϊ��" << curr_time << "  ����״̬����" << endl;
				show_process(process);
				cout << endl;

			}
		}

		//��һ������Ϊ�յ��ڶ�����Ϊ��
		lable2:while (q1.empty() && !q2.empty()) {
			int used_time = 0; //��ǰ�����ڱ�ʱ��Ƭ�Ѿ�����ʱ��
			PCB p = q2.front(); //��ȡ���е�һ�����̷���cpu
			q2.pop();
			if(process[p.id].status!=2)
			{
				process[p.id].status = 1;
				while (used_time < 2)
				{

					p.need_time = p.need_time - 1;
					process[p.id].need_time--;
					used_time++;    //ʱ��Ƭ��һ
					curr_time++;    //��ǰʱ���һ
					if (p.need_time == 0) {  //�������
						used_time = 2;  //��������ʱ��Ϊʱ��Ƭ��С�������Ϳ���ֹͣ��һ��ѭ��

						//���´˽���״̬
						process[p.id].finish_time = curr_time;
						process[p.id].status = 2;

					}

					//ˢ�¾�������
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

					if (p.need_time != 0 && (used_time == 2 || (p.cpu_time - p.need_time)==3)) {  //ʱ��Ƭ����δ���
						process[p.id].status = 0;
						q3.push(p);   //����������������ж�β
					}

					//���ٽ���״̬
					cout << "��ǰʱ��Ϊ��" << curr_time << "  ����״̬����" << endl;
					show_process(process);
					cout << endl;

					//�鿴��һ�������Ƿ�Ϊ��
					if (!q1.empty()) {  //��Ϊ��
						if (!is_in_queue(q3, p))
						{
							q2.push(p);    //��ǰδ��ɽ��̷��뱾���ж�β
						}
						goto lable1;
					}

				}
			}
		}

		//��һ����������Ϊ�յ���������Ϊ��
	while (q1.empty() && q2.empty()&&!q3.empty()) {
		int used_time = 0; //��ǰ�����ڱ�ʱ��Ƭ�Ѿ�����ʱ��
		PCB p = q3.front(); //��ȡ���е�һ�����̷���cpu
		q3.pop();
		if(process[p.id].status != 2)
		{
			process[p.id].status = 1;
			while (used_time < 4)
			{

				p.need_time = p.need_time - 1;
				process[p.id].need_time--;
				used_time++;    //ʱ��Ƭ��һ
				curr_time++;    //��ǰʱ���һ
				if (p.need_time == 0) {  //�������
					used_time = 4;  //��������ʱ��Ϊʱ��Ƭ��С�������Ϳ���ֹͣ��һ��ѭ��

					//���´˽���״̬
					process[p.id].finish_time = curr_time;
					process[p.id].status = 2;

				}

				//ˢ�¾�������
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

				if (p.need_time != 0 && (used_time == 4 || (p.cpu_time - p.need_time-3)%4==0)) {  //ʱ��Ƭ����δ���
					process[p.id].status = 0;
					q3.push(p);   //����������������ж�β
				}

				//���ٽ���״̬
				cout << "��ǰʱ��Ϊ��" << curr_time << "  ����״̬����" << endl;
				show_process(process);
				cout << endl;

				//�鿴��һ�������Ƿ�Ϊ��
				if (!q1.empty()) {  //Ϊ��
					if (!is_in_queue(q3, p))
					{
						q3.push(p);    //��ǰδ��ɽ��̷��뱾���ж�β
					}
					goto lable1;
				}

				//�鿴�ڶ��������Ƿ�Ϊ��
				if (!q2.empty()) {  //Ϊ��
					if (!is_in_queue(q3, p))
					{
						q3.push(p);    //��ǰδ��ɽ��̷��뱾���ж�β
					}
					goto lable2;
				}

			}
		}
	}

	}

}

//չʾ����ָ��
void show_performance(vector<PCB>process) {

	int count = process.size() - 1;  //��������
	double avg_turn_time = 0;   //ƽ����תʱ��
	double avg_wait_time = 0;   //ƽ���ȴ�ʱ��
	double avg_response_time = 0;   //ƽ����Ӧʱ��
	double cpu_time = 0;            //cpu����ʱ��
	double cpu_use;                 //cpu������
	int finish_time = 0;            //���н��̽���ʱ��

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

	cout << "ƽ����תʱ�䣺" << avg_turn_time << endl;
	cout << "ƽ���ȴ�ʱ�䣺" << avg_wait_time << endl;
	cout << "ƽ����Ӧʱ�䣺" << avg_response_time << endl;
	cout << "cpu������   ��" << cpu_use << endl;
}


int main() {


	while(1)
	{
		int id = 1;  //��ǰ�������,��1��ʼ

		//���̶��У�����ά������
		vector<PCB>process;
		

		//���һ�����ý��̣���Ϊ��ʼ����
		PCB p;
		p.reach_time = 0;
		process.push_back(p);
		

		//��������
		int number;
		cout << "ѡ�񴴽���������" << endl;
		cin >> number;

		for (int i = 1; i <= number; i++) {
			create_process(id, process);
			id++;
		}

		vector<PCB>process1(process);  //rr   
		vector<PCB>process2(process);  //sjf
		vector<PCB>process3(process);  //mlf

		//չʾ���̳�ʼ״̬
		cout << "ʱ��Ϊ0ʱ����״̬���£�" << endl;
		show_process(process);
		cout << endl;

		//rr�㷨
		cout << "rr�㷨��" << endl;
	rr(process1, 2);
	cout << "rr�㷨ִ�����״̬�Լ��������ָ��" << endl;
	show_process(process1);
	show_performance(process1);
	cout << endl;

	//sjf�㷨��֤
	cout << "sjf�㷨��" << endl;
	sjf(process2);
	cout << "sjf�㷨ִ�����״̬�Լ��������ָ��" << endl;
	show_process(process2);
	show_performance(process2);
	cout << endl;

	//mlf�㷨��֤
	cout << "mlf�㷨��" << endl;
     mlf(process3);
	 cout << "mlf�㷨ִ�����״̬�Լ��������ָ��" << endl;
	show_process(process3);
	show_performance(process3);

	system("pause");
	system("cls");

	}

	

	//rr�㷨��֤
	//rr(process, 2);
	//show_process(process);

	//sjf�㷨��֤
	//sjf(process);
	//show_process(process);

	//mlf�㷨��֤
	//mlf(process);
	//show_process(process);
	//show_performance(process);

	return 0;
}


