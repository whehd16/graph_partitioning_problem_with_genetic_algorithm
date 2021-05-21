#define _GLIBCXX_USE_CXX11_ABI 0/1
#define _CRT_SECURE_NO_WARNINGS 
#include<iostream>
#include<string>
#include<vector>
#include<cstring>
#include<map>
#include<cstdlib>
#include<algorithm>
#include<queue>
#include<time.h>
#include<fstream>

using namespace std;
#define NUMBEROFNODES 500
#define GENERATION_SIZE 500
#define POPULATION_SIZE 30
#define EXPERIMENTS 100

int optimal = 0;
vector<int> optimals;

int cut_size(const vector<int>& clusterA, const vector<int>& clusterB, map< int, vector<int> >& m, int num_clusters) {
    //clusterA, B ���� �ε����� ����
    // A= { 1, 3 ,5 }, B = {2, 4, 6}

    int temp_cut_size = 0;

    vector<int> t(500, 0);
    /*cout << "============" << endl;
    cout << clusterA.size() << endl;*/
    for (int i = 0; i < clusterA.size(); i++) {
        for (int j = 0; j < m[clusterA[i]].size(); j++) {
            t[m[clusterA[i]][j]] += 1;
        }
    }
    /*cout << clusterB.size() << endl;
    cout << "============" << endl;*/
    for (int i = 0; i < clusterB.size(); i++) {
        if (t[clusterB[i]] != 0) {
            temp_cut_size += t[clusterB[i]];
        }
    }

    return temp_cut_size;
}

int fitness(const vector<vector<int>>& clusters, map< int, vector<int> >& m, int num_clusters) {

    int global_cut_size = 0;

    for (int i = 0; i < clusters.size() - 1; i++) {
        for (int j = i + 1; j < clusters.size(); j++) {
            global_cut_size += cut_size(clusters[i], clusters[j], m, num_clusters);
        }
    }

    return global_cut_size;
}

void evalutate(const vector< vector<int> >& chromosomes, vector<int>& fitness_list, map< int, vector<int> >& m, int num_clusters) {


    for (int i = 0; i < chromosomes.size(); i++) { //100���� ����ü ������ ����
        vector<vector<int>> clusters;

        for (int j = 0; j < num_clusters; j++) {
            vector<int> cluster;
            clusters.push_back(cluster);
        }
        for (int j = 0; j < chromosomes[i].size(); j++) { //����ü ���� ��ŭ ���鼭 �� Ŭ�����Ϳ� �ε����� �߰���
            //chromosomes[i] == 010101001 ...            
            clusters[chromosomes[i][j]].push_back(j);
        }

        //
        /*for (int j = 0; j < clusters.size(); j++) {
            cout << "j :" << j << " : " << clusters[j].size() << endl;
        }*/

        //
        fitness_list[i] = fitness(clusters, m, num_clusters);
    }
}

void initialize(vector< vector<int> >& chromosomes, int num_clusters, int num_nodes) {
    //clusters : balanced Ŭ�����Ͱ� �ǵ��� �� ��
    //Max cut �� ��쿣 balanced ������� �ʾƵ� ��.
    //count : ��� ����
    for (int pop = 0; pop < POPULATION_SIZE; pop++) {
        vector<int> chromosome;
        vector<int> cluster_count(num_clusters);

        int limit;
        int limit_count = 0;
        bool divided = false;

        if (num_nodes % num_clusters == 0) {
            limit = int(num_nodes / num_clusters);
        }
        else {
            limit = int(num_nodes / num_clusters) + 1;
        }
        //for�� ���鼭 �� ��帶�� Ŭ�����͸� �Ҵ�
        for (int i = 0; i < num_nodes; i++) {
            int rand_num = rand() % num_clusters;
            while (cluster_count[rand_num] >= limit) {
                rand_num = rand() % num_clusters;
            }

            cluster_count[rand_num]++;

            chromosome.push_back(rand_num);

            if (divided) {
                continue;
            }

            if (cluster_count[rand_num] == limit) {
                limit_count++;
                if (limit_count == num_nodes - int(num_nodes / num_clusters) * num_clusters) {
                    limit--;
                    divided = true;
                }
            }

        }
        chromosomes.push_back(chromosome);
        //cout << cluster_count[0] << " " << cluster_count[1] << endl;
    }
}

double drand(double low, double high)
{
    double d;

    d = (double)rand() / RAND_MAX;
    return (low + d * (high - low));
}


vector<float> make_roullte(vector<int>& fitness_list) {
    
    int fit_worst = *min_element(fitness_list.begin(), fitness_list.end());        
    int fit_best = *max_element(fitness_list.begin(), fitness_list.end());

    vector<float> roulette(POPULATION_SIZE, 0);

    if (fit_worst == fit_best) {
        roulette[0] = -1;
        return roulette;
    }
    //min �����϶� ������ worst, ������ best
    //max �����϶� ������ best, ������ worst

    for (int i = 0; i < fitness_list.size(); i++) {
        roulette[i] = (fit_worst + fitness_list[i]) + (fit_best - fit_worst) / 2;
        if (i > 0) {
            roulette[i] += roulette[i - 1];
        }
    }

    return roulette;
}

vector<int> selection(vector<float>& roulette, vector<int>& fitness_list) {

    vector<int> parents_index(2, -1);
    int parents_i = 0;

    while (parents_i < 2) {

        float point = drand(0.0, roulette.back());

        for (int i = 0; i < roulette.size(); i++) {
            if (roulette[i] > point) {
                if (i == 0) {
                    parents_index[i++] = i;
                    break;
                }
                else {
                    if (parents_index[0] == i) {
                        break;
                    }
                    else {
                        parents_index[parents_i++] = i;
                        break;
                    }
                }
            }
        }
    }

    return parents_index;

}
int compare(vector<int> c1, vector<int> c2) {
    int temp_count = 0;
    for (int i = 0; i < c1.size(); i++) {
        for (int j = 0; j < c2.size(); j++) {
            if (c1[i] == c2[j]) {
                temp_count += 1;
            }
        }
    }
    return temp_count;
}

void normalization(vector<int>& p1, vector<int>& p2, int cluster_num) {

    vector<vector<int>> c1, c2;
    vector<bool> check_p2(p2.size(), false);

    int before_count = 0;
    int after_count = 0;

    vector<vector<int>> count_matrix(cluster_num, vector <int>(cluster_num, 0));

    for (int i = 0; i < cluster_num; i++) {
        vector<int> temp1;
        vector<int> temp2;
        c1.push_back(temp1);
        c2.push_back(temp2);
    }

    for (int i = 0; i < p1.size(); i++) {
        c1[p1[i]].push_back(i);
    }
    for (int i = 0; i < p1.size(); i++) {
        c2[p2[i]].push_back(i);
    }

    priority_queue<vector<int>> pq;
    for (int i = 0; i < c1.size(); i++) {
        for (int j = 0; j < c2.size(); j++) {
            vector<int> vec = { compare(c1[i], c2[j]), i, j };
            pq.push(vec); //��ǥ�� ���� �ִ��� ������ i,j�� ���ϰ� j�� i�� ��ȯ��Ű�� ��.            
        }
    }    

    while (!pq.empty()) {
        int temp_size = pq.size();
        int temp_count = 0;
        vector<int> target = pq.top();
        while (temp_count != temp_size) {
            vector<int> temp_target = pq.top();
            if (target[1] == temp_target[1] || target[2] == temp_target[2]) {
                pq.pop();
            }
            temp_count += 1;
        }
        if (target[2] == target[1]) {
            continue;
        }
        else {
            for (int i = 0; i < p2.size(); i++) {
                if (p2[i] == target[2] && !check_p2[i]) {
                    p2[i] = target[1];
                    check_p2[i] = true;
                }
            }
        }
    }
}

vector<int> multi_point_xover(vector<int> p1, vector<int> p2, int num_points) {
    vector<int> check_random(p1.size(), 0);
    vector<int> multi_point;

    vector<int> offspring;

    int now_count = 0;

    while (now_count != num_points) {
        int rand_num = rand() % p1.size();
        if (check_random[rand_num] == 0) {
            multi_point.push_back(rand_num);
            check_random[rand_num] = 1;
            now_count += 1;
        }
    }

    sort(multi_point.begin(), multi_point.end());

    int temp_start = 0;
    int temp_end = multi_point[0];

    for (int i = 0; i < multi_point.size(); i++) {

        for (int j = temp_start; j <= temp_end; j++) {
            if (i % 2 == 0) { //�θ� 1
                offspring.push_back(p1[j]);
            }
            else {
                offspring.push_back(p2[j]);
            }
        }

        temp_start = temp_end + 1;
        if (i + 1 == multi_point.size() - 1) {
            temp_end = p1.size() - 1;
        }
        else if (i + 1 != multi_point.size()) {
            temp_end = multi_point[i + 1];
        }
    }

    return offspring;
}

void mutation(vector<int>& offspring, int cluster_num) {
    // mutation rates = 0.01    
    /*int c0 = 0;
    int c1 = 0;
    for (int i = 0; i < offspring.size(); i++) {
        if (offspring[i] == 0) {
            c0 += 1;
        }
        else {
            c1 += 1;
        }
    }*/

    for (int i = 0; i < offspring.size(); i++) {
        if (rand() % 1000 == 1) {
            offspring[i] = (offspring[i]+rand())%cluster_num;
        }
    }
}

vector<string> split(string s, string divid) {
    //divid�� �°� ���ڿ� �Ľ��� ���� ��ȯ
    vector<string> v;
    char* c = strtok((char*)s.c_str(), divid.c_str());
    while (c) {
        v.push_back(c);
        c = strtok(NULL, divid.c_str());
    }
    return v;
}

void show_best_fitness(vector<int> fitness) {
    sort(fitness.begin(), fitness.end());
    reverse(fitness.begin(), fitness.end());
    //cout << "Best : " << fitness[0] << endl;
    optimal = fitness[0];
}

int main(int argc, char* argv[]) {

    time_t start, end;
    start = time(NULL);

    string s;
    srand((unsigned int)time(NULL));
    int count = 0;
    //�Է� ���Ͽ� ���� �׷��� ��� �߰�
    map< int, vector<int> > m;

    while (getline(cin, s)) {
        vector<string> n = split(s, " ");
        if (n.size() > 3) { //�߰��� ��尡 �ִٸ�
            for (int j = 3; j < n.size(); j++) {
                m[count].push_back(stoi(n[j]) - 1);
            }
        }
        count++;
    }


    for (int t = 0; t < EXPERIMENTS; t++) {
        cout << t << "th experiments" << endl;
        start = time(NULL);
        // ���ڵ�
    //K���� Ŭ�����͸� �� Ŭ�����Ϳ�, ��� ���� �����ؼ� �ʱ�ȭ
        vector<vector<int>> chromosomes;
        vector<int> fitness_list(POPULATION_SIZE, 0);

        //chromosome, m ���� 0~499 ��� ��ȣ�� ���� ���� �������
        initialize(chromosomes, atoi(argv[1]), count);

        //fitness_list�� �� ���յ� ���
        int n_gen = 0;
        while (true) {
            evalutate(chromosomes, fitness_list, m, atoi(argv[1]));
            show_best_fitness(fitness_list);
            end = time(NULL);
            if (end - start >= 60) {
                cout << "time over" << endl;
                break;
            }

           /* cout << " ========================= " << endl;
            cout << n_gen << " GENERATION" << endl;*/
            if (n_gen > GENERATION_SIZE) {
                break;
            }

            //selection   
            vector<float>roulette = make_roullte(fitness_list);
            if (roulette[0] == -1) {
                cout << "convergence" << endl;
                break;
            }

            vector<vector<int>> new_chromosomes;
            new_chromosomes.push_back(chromosomes[max_element(fitness_list.begin(), fitness_list.end()) - fitness_list.begin()]);
            for (int i = 1; i < POPULATION_SIZE; i++) {

                vector<int> parents_index = selection(roulette, fitness_list);

                vector<int> p1 = chromosomes[parents_index[0]];
                vector<int> p2 = chromosomes[parents_index[1]];

                normalization(p1, p2, atoi(argv[1]));
                vector<int> new_offspring = multi_point_xover(p1, p2, 5);

                /*vector<int> temp_v(atoi(argv[1]), 0);
                for (int k = 0; k < new_offspring.size(); k++) {
                    temp_v[new_offspring[k]] += 1;
                }

                for (int k = 0; k < temp_v.size(); k++) {
                    cout << "k : " << temp_v[k] << endl;
                }
                cout << "============================" << endl;*/
                mutation(new_offspring, atoi(argv[1]));
                new_chromosomes.push_back(new_offspring);
            }

            chromosomes = new_chromosomes;
            n_gen++;
        }
        //���� ����        
        cout << "optimal : " << optimal << endl;
        optimals.push_back(optimal);
    }
    string filePath = "max_" + string(argv[1]) + "_inputA.txt";
    // write File
    ofstream writeFile(filePath.data());
    int sum = 0;

    if (writeFile.is_open()) {
        for (int n = 0; n < optimals.size(); n++) {

            writeFile << n;
            writeFile << " : ";
            writeFile << optimals[n];
            writeFile << "\n";
            sum += optimals[n];
        }


        writeFile << "\n";
        writeFile << "Best : ";
        //writeFile << optimals.back();
        writeFile << *max_element(optimals.begin(), optimals.end());
        writeFile << "\n";
        writeFile << "Avg : ";
        writeFile << sum / EXPERIMENTS;
        writeFile << "\n";
        writeFile << "Std : ";
        float avg = sum / EXPERIMENTS;
        float std_sum = 0;
        for (int n = 0; n < optimals.size(); n++) {
            std_sum += pow(float(optimals[n] - avg), 2);
        }
        writeFile << sqrt(std_sum / EXPERIMENTS);
        writeFile << "\n";

        writeFile.close();
    }
}