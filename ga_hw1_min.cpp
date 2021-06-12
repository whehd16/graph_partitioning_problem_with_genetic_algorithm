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
    //clusterA, B 에는 인덱스가 있음
    // A= { 1, 3 ,5 }, B = {2, 4, 6}
    
    int temp_cut_size = 0;
    
    vector<int> t(500, 0);
    /*cout << "============" << endl;
    cout << clusterA.size() << endl;*/
    for (int i = 0; i < clusterA.size(); i++){
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

int fitness(const vector<vector<int>> &clusters, map< int, vector<int> > &m, int num_clusters){     
    
    int global_cut_size = 0;
    
    for(int i = 0; i < clusters.size()-1; i++){
        for(int j = i+1; j < clusters.size(); j++){
            global_cut_size += cut_size(clusters[i], clusters[j], m, num_clusters);
        }
    }
    
    return global_cut_size;
}

void evalutate(const vector< vector<int> > &chromosomes, vector<int> &fitness_list, map< int, vector<int> > &m, int num_clusters){
    
    for(int i = 0; i < chromosomes.size(); i++){ //100개의 염색체 각각에 대해
        vector<vector<int>> clusters;

        for(int j = 0; j < num_clusters; j++){
            vector<int> cluster;
            clusters.push_back(cluster);
        }        
        for(int j = 0; j < chromosomes[i].size(); j++){ //염색체 길이 만큼 돌면서 각 클러스터에 인덱스를 추가함
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

void initialize(vector< vector<int> > &chromosomes, int num_clusters, int num_nodes){
    //clusters : balanced 클러스터가 되도록 할 것
    //Max cut 일 경우엔 balanced 고려하지 않아도 됨.
    //count : 노드 개수
    for(int pop = 0 ; pop < POPULATION_SIZE; pop++){
        vector<int> chromosome;
        vector<int> cluster_count(num_clusters);
        
        int limit;
        int limit_count = 0;
        bool divided = false;

        if ( num_nodes % num_clusters == 0){
            limit = int(num_nodes / num_clusters);
        }
        else{
            limit = int(num_nodes / num_clusters)+1;
        }        
        //for문 돌면서 각 노드마다 클러스터를 할당
        for(int i = 0; i < num_nodes; i++){               
            int rand_num = rand() % num_clusters;                                   
            while(cluster_count[rand_num] >= limit){                
                rand_num = rand() % num_clusters;              
            }
            
            cluster_count[rand_num]++;            
            
            chromosome.push_back(rand_num);

            if(divided){                
                continue;
            }

            if (cluster_count[rand_num] == limit){                
                limit_count++;                                                
                if (limit_count == num_nodes - int(num_nodes / num_clusters)*num_clusters){                                                                               
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
    int fit_worst = *max_element(fitness_list.begin(), fitness_list.end());
    int fit_best = *min_element(fitness_list.begin(), fitness_list.end());

    vector<float> roulette(POPULATION_SIZE, 0);

    if (fit_worst == fit_best) {
        roulette[0] = -1;
        return roulette;
    }
    //min 문제일때 높은게 worst, 낮은게 best
    //max 문제일때 높은게 best, 낮은게 worst
    
    for (int i = 0; i < fitness_list.size(); i++) {
        roulette[i] = (fit_worst - fitness_list[i]) + (fit_worst - fit_best) / 2;        
        if (i > 0) {
            roulette[i] += roulette[i - 1];
        }
    }
    
    return roulette;
}

vector<int> selection(vector<float>&roulette, vector<int>& fitness_list) {
    
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
            vector<int> vec = {compare(c1[i], c2[j]), i, j};
            pq.push(vec); //목표는 가장 최댓값을 가지는 i,j를 구하고 j를 i로 변환시키는 것.            
        }
    }
    //11 12 13 21 22 23 31 32 33   1->2가 제일 커서 j=2를 i=1로 바꿀거임
    //  23 31 33

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
            if (i % 2 == 0) { //부모 1
                offspring.push_back(p1[j]);
            }
            else {
                offspring.push_back(p2[j]);
            }
        }
        
        temp_start = temp_end+1;
        if (i + 1 == multi_point.size() - 1) {
            temp_end = p1.size()-1;
        }
        else if(i+1 != multi_point.size()) {
            temp_end = multi_point[i + 1];
        }        
    }        

    return offspring;
}

void balancing(vector<int>& offspring, int cluster_num) {    

    vector<int> t_count1(cluster_num, 0);

    if (NUMBEROFNODES % cluster_num == 0) { //2, 개수 같아야 함
        vector<int> c_num(cluster_num, 0);
        vector<vector<int>> cluster_index;

        for (int i = 0; i < cluster_num; i++) {
            vector<int> v;
            cluster_index.push_back(v);
        }
        for (int i = 0; i < offspring.size(); i++) {
            c_num[offspring[i]] += 1;
            cluster_index[offspring[i]].push_back(i);
        }
        if (c_num[0] == c_num[1]) {
            return;
        }
        else {           
            int max_elem = max_element(c_num.begin(), c_num.end()) - c_num.begin();
            queue<int> q;
            for (int i = 0; i < cluster_num; i++) {
                if (i != max_elem) {
                    for (int j = 0; j < (NUMBEROFNODES/2) - c_num[i]; j++) {
                        q.push(i);
                    }
                }
            }
            while (true) {
                if (q.empty()) {
                    break;
                }
                int target = q.front();
                q.pop();
                int random_index = rand() % c_num[max_elem];                
                offspring[cluster_index[max_elem][random_index]] = target;
                c_num[max_elem]--;
                cluster_index[max_elem].erase(cluster_index[max_elem].begin() + random_index);
            }                        
        }
    }
    else { //32 개수 하나 차이까지 허용 16, 15

        vector<int> c_num(cluster_num, 0);  
        vector<vector<int>> cluster_index;

        for (int i = 0; i < cluster_num; i++) {
            vector<int> v;
            cluster_index.push_back(v);
        }
        
        priority_queue<vector<int>> pq;

        for (int i = 0; i < offspring.size(); i++) {
            c_num[offspring[i]] += 1;
            cluster_index[offspring[i]].push_back(i);
        }

        while (*max_element(c_num.begin(), c_num.end()) - *min_element(c_num.begin(), c_num.end()) != 1) {
            int max_index = max_element(c_num.begin(), c_num.end()) - c_num.begin();
            int min_index = min_element(c_num.begin(), c_num.end()) - c_num.begin();

            //원소 하나씩 뺄 예정
            int temp_rand = rand() % cluster_index[max_index].size();
            offspring[cluster_index[max_index][temp_rand]] = min_index;
            c_num[max_index]--;
            c_num[min_index]++;
            cluster_index[max_index].erase(cluster_index[max_index].begin() + temp_rand);
        }                
    }    
}

void mutation(vector<int>& offspring, int cluster_num) {
    // mutation rates = 0.01    

    //int c0 = 0;
    //int c1 = 0;
    //for (int i = 0; i < offspring.size(); i++) {
    //    if (offspring[i] == 0) {
    //        c0 += 1;
    //    }
    //    else {
    //        c1 += 1;
    //    }
    //}

    ////cout << c0 << " "<<  c1 << endl;

    for (int i = 0; i < offspring.size(); i++) {
        if (rand() % 2000 == 1) {            
            int swap_index1 = rand() % offspring.size();
            int swap_index2 = swap_index1;
            while (swap_index1 == swap_index2) {
                swap_index2 = rand() % offspring.size();
            }
            
            int temp = offspring[swap_index1];
            offspring[swap_index1] = offspring[swap_index2];
            offspring[swap_index2] = temp;
        }
    }

    //c0 = 0;
    //c1 = 0;
    //for (int i = 0; i < offspring.size(); i++) {
    //    if (offspring[i] == 0) {
    //        c0 += 1;
    //    }
    //    else {
    //        c1 += 1;
    //    }
    //}
    //cout << c0 << " " << c1 << endl;
    //cout << "========================" << endl;
}

vector<string> split(string s, string divid) {
    //divid에 맞게 문자열 파싱한 벡터 반환
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
    //cout << "Best : " << fitness[0] << endl;
    optimal = fitness[0];
}

int main(int argc, char *argv[]){
            
    time_t start, end;
    vector<string> files;        
    string s;    
    srand((unsigned int)time(NULL));
    int count = 0;    
    //입력 파일에 대해 그래프 노드 추가
    map< int, vector<int> > m;

    while(getline(cin, s)){
        vector<string> n = split(s, " ");
        if(n.size() > 3){ //추가할 노드가 있다면
            for(int j = 3; j < n.size() ; j++){
                   m[count].push_back(stoi(n[j])-1);
            }
        }
        count++;
    }
    for (int t = 0; t < EXPERIMENTS; t++) {
        cout << t << " th experiments" << endl;
        start = time(NULL);

        // 인코딩
        //K개로 클러스터링 된 클러스터와, 노드 개수 전달해서 초기화
        vector<vector<int>> chromosomes;
        vector<int> fitness_list(POPULATION_SIZE, 0);

        //chromosome, m 에는 0~499 노드 번호에 대한 정보 담겨있음
        initialize(chromosomes, atoi(argv[1]), count);

        //fitness_list에 해 적합도 담김
        int n_gen = 0;
        while (true) {
            evalutate(chromosomes, fitness_list, m, atoi(argv[1]));
            show_best_fitness(fitness_list);
            end = time(NULL);
            if (end - start >= 60) {                
                cout << "time over" << endl;
                break;
            }

            /*cout << " ========================= " << endl;
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
            new_chromosomes.push_back(chromosomes[min_element(fitness_list.begin(), fitness_list.end()) - fitness_list.begin()]);
            for (int i = 1; i < POPULATION_SIZE; i++) {
                //cout << "selection" << endl;
                //cout << roulette[roulette.size()-1]<<endl;
                vector<int> parents_index = selection(roulette, fitness_list);

                //normalization
                vector<int> p1 = chromosomes[parents_index[0]];
                vector<int> p2 = chromosomes[parents_index[1]];

                normalization(p1, p2, atoi(argv[1]));
                //cout << "crossver" << endl;
                //crossover
                vector<int> new_offspring = multi_point_xover(p1, p2, 5);

                balancing(new_offspring, atoi(argv[1]));

                //cout << "mutation" << endl;
                //mutation
                mutation(new_offspring, atoi(argv[1]));
                new_chromosomes.push_back(new_offspring);
            }
            chromosomes = new_chromosomes;
            n_gen++;
            
        }
        //파일 오픈        
        cout << "optimal : " << optimal << endl;
        optimals.push_back(optimal);
    }    
    string filePath = "min_" + string(argv[1]) + "_inputA.txt";
    // write File
    ofstream writeFile(filePath.data());
    int sum = 0;
    if (writeFile.is_open()) {
        for (int n = 0; n < optimals.size()-1; n++) {
            writeFile << n;
            writeFile << " : ";
            writeFile << optimals[n];
            writeFile << "\n";
            sum += optimals[n];
        }                

        writeFile << "\n";
        writeFile << "Best : ";
        writeFile << *optimals.end();
        writeFile << "\n";
        writeFile << "Avg : ";
        writeFile << sum / EXPERIMENTS;
        writeFile << "Std : ";
        float avg = sum / EXPERIMENTS;
        float std_sum = 0;
        for (int n = 0; n < optimals.size(); n++) {
            std_sum += pow(float(optimals[n] - avg), 2);
        }
        writeFile << std_sum / EXPERIMENTS;       
        writeFile << "\n";

        writeFile.close();
    }
}



// for(int i = 0; i < chromosomes.size(); i++){
    //     for(int j = 0; j<chromosomes[i].size(); j++){
    //         cout << chromosomes[i][j] << " ";
    //     }
    //     cout << endl;        
    // }